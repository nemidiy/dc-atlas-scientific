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

#ifndef __DC_ATLAS_READ_RESPONSE_H__
#define __DC_ATLAS_READ_RESPONSE_H__
#include <Arduino.h>
#include <string>
#include <vector>
#include "device.h"
#include "device_response.h"

namespace dc {

namespace atlas {

struct ReadResponse: public DeviceResponse {

  ReadResponse(i2c_response_code code, const std::string& response_data);

  virtual ~ReadResponse(){};

  std::string get_value_str();

  double get_value_double();

};

} // namespace atlas

} // namespace dc
#endif
