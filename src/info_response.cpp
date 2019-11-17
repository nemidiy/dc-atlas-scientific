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

#include "info_response.h"

using namespace dc::atlas;

InfoResponse::InfoResponse(
        i2c_response_code code, const std::string& response_data):
            DeviceResponse(code, response_data){

}

Device::device_type InfoResponse::get_device_type(){
    if(partitioned_data.size() != 3)
        return Device::UNKNOWN;

    if(partitioned_data[1] == "RTD")
        return Device::TEMP_SENSOR;
    else if(partitioned_data[1] == "pH")
        return Device::PH_SENSOR;
    else if(partitioned_data[1] == "ORP")
        return Device::ORP_SENSOR;
    else if(partitioned_data[1] == "EC")
        return Device::EC_SENSOR;
    else if(partitioned_data[1] == "DO")
        return Device::DO_SENSOR;
    else if(partitioned_data[1] == "FLO")
        return Device::FLOW_SENSOR;
    return Device::UNKNOWN;
}

std::string InfoResponse::get_version(){
    if(partitioned_data.size() != 3)
        return "None";
    return partitioned_data[2];
    
}