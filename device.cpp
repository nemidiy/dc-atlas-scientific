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
