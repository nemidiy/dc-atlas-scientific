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
    return Device::UNKNOWN;
}

std::string InfoResponse::get_version(){
    if(partitioned_data.size() != 3)
        return "None";
    return partitioned_data[2];
    
}