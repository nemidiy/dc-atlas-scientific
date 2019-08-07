#ifndef __GJ_ATLAS_READ_RESPONSE_H__
#define __GJ_ATLAS_READ_RESPONSE_H__
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
