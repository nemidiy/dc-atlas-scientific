#include "device.h"
#include "device_command.h"
#include "info_response.h"
#include "read_response.h"

#include <Wire.h>
#include <vector>

using namespace gj::atlas;

Device::Device(int i2c_addr, device_type t){
  this->i2c_addr = i2c_addr;
  this->dev_type = t;
}

Device::Device(const Device& d){
  this->i2c_addr = d.i2c_addr;
  this->dev_type = d.dev_type;
}

Device::~Device(){

}

double Device::read(){
  double value = 0.00;
  send_i2c_command(i2c_addr, "R\0");
  // we now wait 600ms for the response according to documentation
  delay(600);
  int response_data_size = 20;
  char response_data[response_data_size];
  DeviceResponse::i2c_response_code code = DeviceResponse::NO_DATA;
  code = read_i2c_response(i2c_addr, response_data, response_data_size);
  if(code == DeviceResponse::SUCCESS){
    // let's build the INFO response
    DeviceResponse* response =
      DeviceResponse::build_response(
          DeviceCommand::READ, code, response_data, response_data_size);
    ReadResponse* read = static_cast<ReadResponse*>(response);
    value = read->get_value_double();
    delete read;
  }else{
    //TODO raise exception
  }
  return value;
}

void Device::send_i2c_command(int address ,const char* cmd){
  Wire.beginTransmission(address);
  Wire.write(cmd);
  Wire.endTransmission();
}

DeviceResponse::i2c_response_code Device::read_i2c_response(
              int address, char* buffer, int buffer_size){

  Wire.requestFrom(address, buffer_size, 1);
  //the first byte is the response code, we read this separately.
  byte code = Wire.read();
  DeviceResponse::i2c_response_code response_code = DeviceResponse::NO_DATA;

  switch (code) {
    case 1:
      //means the command was successful.
      response_code = DeviceResponse::SUCCESS;
      break;

    case 2:
      //means the command has failed.
      response_code = DeviceResponse::FAILURE;
      break;

    case 254:
     //command has not yet been finished.
      response_code = DeviceResponse::NOT_FINISHED;
      break;

    case 255:
      //means there is no further data to send.
      response_code = DeviceResponse::NO_DATA;
      break;
  }

  // let's pull whatever data remaining from the wire.
  byte i = 0;
  while (Wire.available()) {
    buffer[i] = Wire.read();
    if (buffer[i] == 0){
      Wire.endTransmission();
      break;
    }
    ++i;
  }

  return response_code;
}

int Device::auto_discovery(
    std::map<device_type, Device*> &devices){
      // we scan from address 90 to 120 to see who is out there
      // on the I2C bus
      for(int i = 90; i < 121; ++i){
        Serial.println("---------------------------------------------");
        
        int response_data_size = 20;
        char response_data[response_data_size];
        DeviceResponse::i2c_response_code code = DeviceResponse::NO_DATA;
        Serial.print("sending command to port ");
        Serial.println(i);
        send_i2c_command(i, "i\0");
        // we now wait 300ms for the response according to documentation
        delay(300);
        //call the circuit and request 20 bytes (this may be more than we need)
        code = read_i2c_response(i, response_data, response_data_size);
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
            devices.insert(std::make_pair(info->get_device_type(), dev));
            delete info;
            break;
          }
          case DeviceResponse::NOT_FINISHED:{
            Serial.println("DeviceResponse::NOT_FINISHED");
            //delay 600 ms and retry
            delay(600);
            code = read_i2c_response(i, response_data, response_data_size);
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
