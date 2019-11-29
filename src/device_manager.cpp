/*
Copyright 2019 Nicolas Emiliani

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "device_manager.h"
#include "device_command.h"
#include "info_response.h"
#include "read_response.h"
#include <sstream>

using namespace dc::atlas;

DeviceManager* DeviceManager::instance;

DeviceManager::response_callback DeviceManager::null_callback = [](
            dc::atlas::Device* dev,
            dc::atlas::DeviceResponse::i2c_response_code code,
            char* buffer,
            unsigned int buffer_size) -> void* {
        // do nothing, it's just a clear
        return NULL;
    };

DeviceManager::response_callback DeviceManager::read_double_callback = [](
            dc::atlas::Device* dev,
            dc::atlas::DeviceResponse::i2c_response_code code,
            char* buffer,
            unsigned int buffer_size) -> void* {
    if(code == DeviceResponse::SUCCESS){
      // let's build the read response
      DeviceResponse* response =
        DeviceResponse::build_response(
            DeviceCommand::READ, code, buffer, buffer_size);
      ReadResponse* read = static_cast<ReadResponse*>(response);
      dev->last_value = read->get_value_double();
      delete read;
    }else if(code == DeviceResponse::NOT_FINISHED){
      //TODO raise exception
    }else{
      //TODO raise exception
    }
    return NULL;
  };

DeviceManager::DeviceManager(){
  randomSeed(millis());
}

DeviceManager::~DeviceManager(){

}

DeviceManager* DeviceManager::get_instance(DeviceManager* i){
  if(i){
    DeviceManager::instance = i;
  }else if(!instance){
    DeviceManager::instance = new DeviceManager;
  }
  return DeviceManager::instance;
}

double DeviceManager::get_device_value_double(const std::string& dev_name) const{
    std::map<std::string, dc::atlas::Device*>::const_iterator it;
    if((it = devices.find(dev_name)) != devices.end()){
       return it->second->get_last_value();
    }
    return -999;
}

void DeviceManager::loop(){
    fetch_responses();
    send_scheduled();
}

void DeviceManager::send_scheduled(){
    unsigned long now = millis();
    for(auto it = scheduling_queue.begin(); it != scheduling_queue.end(); ){
        if(now < it->exec_time_millis){
            //not ready to execute
            ++it;
            continue;
        }
        Device* dev = get_device(it->device_name);
        if(!dev){
            Serial.print("ERROR: unkown device ");
            Serial.println(it->device_name.c_str());
            continue;
        }
        //send the command
        Device::send_i2c_command(dev->i2c_addr, it->command.c_str());
        // set the time to fetch the reponse
        it->fetch_time_millis = millis() + it->response_time_millis;
        // move the schedule to the response queue
        response_queue.push_back(*it);
        //remove from the scheduling queue
        it = scheduling_queue.erase(it);
    }
}

void DeviceManager::fetch_responses(){
    unsigned long now = millis();
    for(auto it = response_queue.begin(); it != response_queue.end(); ){
        if(now < it->fetch_time_millis){
            //not ready to fetch the response
            ++it;
            continue;
        }
        // we are ready to fetch the response
        int response_data_size = 20;
        char response_data[response_data_size];
        Device* dev = get_device(it->device_name);
        DeviceResponse::i2c_response_code code =
            Device::read_i2c_response(dev->i2c_addr, response_data, response_data_size);
        // execute the callback
        it->callback(dev, code, response_data, response_data_size);
        if(it->auto_interval){
            // it has the auto interval set so we need to queue it back
            it->exec_time_millis = millis() + it->auto_interval;
            scheduling_queue.push_back(*it);
        }
        it = response_queue.erase(it);
    }
}

int DeviceManager::auto_discovery(){
      // we scan from address 90 to 120 to see who is out there
      // on the I2C bus
      for(int i = 90; i < 121; ++i){
        Serial.println("---------------------------------------------");
        
        int response_data_size = 20;
        char response_data[response_data_size];
        DeviceResponse::i2c_response_code code = DeviceResponse::NO_DATA;
        Serial.print("sending command to port ");
        Serial.println(i);
        Device::send_i2c_command(i, "i\0");
        // we now wait 300ms for the response according to documentation
        delay(300);
        //call the circuit and request 20 bytes (this may be more than we need)
        code = Device::read_i2c_response(
          i, response_data, response_data_size);
        Serial.print("\t* response code is ");
        switch(code){
          case DeviceResponse::SUCCESS:{
            Serial.println("DeviceResponse::SUCCESS");
            Serial.print("\t* response data is [");
            Serial.print(response_data);
            Serial.println("]");

            // let's build the INFO response
            DeviceResponse* response =
              DeviceResponse::build_response(
                  DeviceCommand::INFO, code, response_data, response_data_size);
            InfoResponse* info = static_cast<InfoResponse*>(response);

            Serial.print("\t* device_type ");
            Serial.println(info->get_device_type());
            Serial.print("\t* firmware version ");
            Serial.println(info->get_version().c_str());
            
            // we can now build the device and save it
            Device* dev = new Device(i, info->get_device_type());
            add_device(dev);
            delete info;
            break;
          }
          case DeviceResponse::NOT_FINISHED:{
            Serial.println("DeviceResponse::NOT_FINISHED");
            //delay 600 ms and retry
            delay(600);
            code = Device::read_i2c_response(
                i, response_data, response_data_size);
            if(code != DeviceResponse::SUCCESS)
              continue;
            DeviceResponse* response =
              DeviceResponse::build_response(
                DeviceCommand::INFO, code, response_data, response_data_size);
            delete response;
            break;
          }
          case DeviceResponse::FAILURE:{
            Serial.println("DeviceResponse::FAILURE");
            break;
          }
          case DeviceResponse::NO_DATA:{
            Serial.println("DeviceResponse::NO_DATA");
            break;
          }
        }
      }
      return 0;
}

std::string DeviceManager::add_device(
        dc::atlas::Device* dev, const std::string& name){
    std::string dev_name;
    if(!name.size()){
        std::ostringstream os;
        os << dev->get_device_type_string() << "_" << random(1000, 1999);
        dev_name = os.str();
    }else{
        dev_name = name;
    }
    devices.insert(make_pair(dev_name, dev));
    dev->set_name(dev_name);
    return dev_name;
}

void DeviceManager::schedule_command(
            const std::string& command,
            const std::string& device_name,
            response_callback& callback,
            unsigned long int exec_millis,
            unsigned long int response_millis,
            unsigned long int auto_interval){

    schedule s;
    s.command = command;
    s.device_name = device_name;
    s.callback = callback;
    s.response_time_millis = response_millis;
    s.exec_time_millis = millis() + exec_millis;
    s.fetch_time_millis = 0;
    s.auto_interval = auto_interval;
    scheduling_queue.push_back(s);
}

Device* DeviceManager::get_device(const std::string& name) const {
    std::map<std::string, dc::atlas::Device*>::const_iterator it;
    if((it = devices.find(name)) != devices.end()){
       return it->second;
    }
    return NULL;
}
