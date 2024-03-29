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

#include "device.h"
#include "device_command.h"
#include "info_response.h"
#include "read_response.h"

#include <Wire.h>
#include <vector>

using namespace dc::atlas;

Device::Device(int i2c_addr, device_type t){
  this->i2c_addr = i2c_addr;
  this->dev_type = t;
  this->last_value = -888;
}

Device::Device(const Device& d){
  this->i2c_addr = d.i2c_addr;
  this->dev_type = d.dev_type;
  this->last_value = d.last_value;
}

Device::~Device(){

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

std::string Device::get_device_type_string(){
    switch(dev_type){
        case PH_SENSOR   : return "PH";
        case EC_SENSOR   : return "EC";
        case TEMP_SENSOR : return "TEMP";
        case ORP_SENSOR  : return "ORP";
        case DO_SENSOR   : return "DO";
        case FLOW_SENSOR : return "FLOW";
        case PUMP        : return "PUMP";
    }
    return "UNK";
}

void Device::set_name(const std::string& name){
    this->name = name;
}