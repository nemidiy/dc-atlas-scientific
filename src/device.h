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

#ifndef __DC_ATLAS_DEVICE_H__
#define __DC_ATLAS_DEVICE_H__
#include <map>
#include <string>
#include <vector>
#include <Arduino.h>
#include <functional>

#include "device_response.h"

#define DEFAULT_I2C_ADDRESS_PH 99
#define DEFAULT_I2C_ADDRESS_EC 100
#define DEFAULT_I2C_ADDRESS_TEMP 101
#define DEFAULT_I2C_ADDRESS_ORP 102
#define DEFAULT_I2C_ADDRESS_PUMP 103
#define DEFAULT_I2C_ADDRESS_FLOW 104
#define DEFAULT_I2C_ADDRESS_DO 105

#define I2C_ADDRESS_SCAN_START 90
#define I2C_ADDRESS_SCAN_FINISH 120

namespace dc {

namespace atlas {

struct Device{

    friend class DeviceManager;

    typedef enum {
      PH_SENSOR   = 0,
      EC_SENSOR   = 1,
      TEMP_SENSOR = 2,
      ORP_SENSOR  = 3,
      DO_SENSOR   = 4,
      FLOW_SENSOR = 5,
      PUMP        = 6,
      UNKNOWN     = 7,
    } device_type;

    typedef std::function<double (Device* dev)> read_function;

    //constructor
    Device(int i2c_addr, device_type t);

    //copy constructor
    Device(const Device& d);

    //destructor
    ~Device();

    device_type get_device_type(){
      return dev_type;
    }

    std::string get_device_type_string();

    void set_name(const std::string& name);

    static void send_i2c_command(
        int address,
        const char* cmd);

    static DeviceResponse::i2c_response_code read_i2c_response(
        int address,
        char* buffer,
        int buffer_size);

    double get_last_value(){
      return last_value;
    }

  private:

    //i2c address for the device
    int i2c_addr;

    //device_type
    device_type dev_type;

    double last_value;

    std::string name;
};

} // namespace atlas

} // namespace dc

#endif
