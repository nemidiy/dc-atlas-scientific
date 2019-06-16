#include "info_response.h"

using namespace gj::atlas;

InfoResponse::InfoResponse(
        i2c_response_code code, const std::string& response_data):
            DeviceResponse(code, response_data){

}

Device::device_type InfoResponse::get_device_type(){
    if(partitioned_data.size() != 3)
        return Device::UNKNOWN;

    if(partitioned_data[1] == "RTD")
        return Device::TEMP_SENSOR;
    return Device::UNKNOWN;
}

std::string InfoResponse::get_version(){
    if(partitioned_data.size() != 3)
        return "None";
    return partitioned_data[2];
    
}