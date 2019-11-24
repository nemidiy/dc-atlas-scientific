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

#ifndef __DC_ATLAS_DEVICE_RESPONSE_H__
#define __DC_ATLAS_DEVICE_RESPONSE_H__
#include <Arduino.h>
#include <string>
#include <vector>
#include "device_command.h"

namespace dc {

namespace atlas {

struct DeviceResponse {

  typedef enum {
    SUCCESS = 1,
    FAILURE = 2,
    NOT_FINISHED = 254,
    NO_DATA = 255,
  } i2c_response_code;

  DeviceResponse(i2c_response_code code, const std::string& response_data);

  virtual ~DeviceResponse(){};

  void set_partitioned_data(
        const std::vector<std::string>& partitioned_data);

  static void split_response(
      const std::string& response_data,
      std::vector<std::string>& parts);

  static DeviceResponse* build_response(
      DeviceCommand::device_command com,
      i2c_response_code code,
      const char* response_data,
      int response_data_size);

protected:

  i2c_response_code code;

  std::string raw_response_data;

  std::vector<std::string> partitioned_data;

};

} // namespace atlas

} // namespace dc
#endif
