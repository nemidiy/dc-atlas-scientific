#include "device_manager.h"
#include "device_command.h"
#include "info_response.h"

using namespace gj::atlas;

#define READ_TIME_MILLIS 900

DeviceManager::DeviceManager(unsigned long int loop_millis){
  this->loop_millis = loop_millis;
  this->last_run = 0;
}

DeviceManager::~DeviceManager(){

}

void DeviceManager::add_device(
            Device::device_type t,
            Device* dev){
  all_devs.insert(std::make_pair(t, dev));
}

double DeviceManager::get_device_value(Device::device_type t){
  
  std::map<gj::atlas::Device::device_type, gj::atlas::Device*>::iterator it;
  if((it = all_devs.find(t)) != all_devs.end()){
    return it->second->get_last_value();
  }
  return -999;
}

void DeviceManager::loop(){
  unsigned long int now = millis();
  if(now - last_run >= READ_TIME_MILLIS){
    // we can fetch the values from devices
    fetch_responses();
  }

  if(now - last_run >= loop_millis){
    //moment to send reads
    last_run = now;
    send_reads();
  }

}

void DeviceManager::send_reads(){
  for(auto& kv: all_devs){
    auto read_funct = kv.second->read();
    reads.insert(make_pair(kv.first, read_funct));
  }
}

void DeviceManager::fetch_responses(){
  for(auto it = reads.cbegin(); it != reads.cend(); ){
    //execute the lambda
    double value = it->second(all_devs[it->first]);
    //save the value
    all_devs[it->first]->last_value = value;
    reads.erase(it++);
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
            all_devs.insert(std::make_pair(info->get_device_type(), dev));
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