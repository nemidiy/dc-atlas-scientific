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

#include "read_response.h"
#include <cstdlib>
using namespace dc::atlas;

ReadResponse::ReadResponse(
    i2c_response_code code, const std::string& response_data):
      DeviceResponse(code, response_data){

}

std::string ReadResponse::get_value_str(){
  if(partitioned_data.size() != 1)
    return "NULL";
  return partitioned_data[1];
}

double ReadResponse::get_value_double(){
  if(partitioned_data.size() != 1)
    return -999;
  return std::atof(partitioned_data[0].c_str());
}
