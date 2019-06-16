#include "read_response.h"
#include <cstdlib>
using namespace gj::atlas;

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
