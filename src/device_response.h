#ifndef __GJ_ATLAS_DEVICE_RESPONSE_H__
#define __GJ_ATLAS_DEVICE_RESPONSE_H__
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
