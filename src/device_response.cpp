#include "device_response.h"
#include "info_response.h"
#include "read_response.h"
#include <string>
#include <vector>

using namespace dc::atlas;

DeviceResponse::DeviceResponse(
              i2c_response_code code, const std::string& response_data){
    this->code = code;
    this->raw_response_data = response_data;
}

void DeviceResponse::set_partitioned_data(
      const std::vector<std::string>& partitioned_data){
  this->partitioned_data = partitioned_data;
}

void DeviceResponse::split_response(
            const std::string& response_data, std::vector<std::string>& parts){
    std::string part;
    for(int i = 0; i < response_data.length(); ++i){
      if(response_data[i] != ',' && response_data[i] != 0){
          part += response_data[i];
      }else{
        parts.push_back(part);
        part = "";
      }
    }
    parts.push_back(part);
}

DeviceResponse* DeviceResponse::build_response(
                    DeviceCommand::device_command com,
                    i2c_response_code code,
                    const char* response_data,
                    int response_data_size){
  std::vector<std::string> parts;
  DeviceResponse* r;
  if(com == DeviceCommand::INFO){
      r = new InfoResponse(code, response_data);
  }else if(com == DeviceCommand::READ){
      r = new ReadResponse(code, response_data);
  }else{
      r = new DeviceResponse(code, response_data);
  }

  DeviceResponse::split_response(response_data, parts);
  r->set_partitioned_data(parts);

  return r;
}
