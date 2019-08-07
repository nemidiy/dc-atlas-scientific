#ifndef __GJ_ATLAS_INFO_RESPONSE_H__
#define __GJ_ATLAS_INFO_RESPONSE_H__
#include <Arduino.h>
#include <string>
#include <vector>
#include "device.h"
#include "device_response.h"

namespace dc {

namespace atlas {

struct InfoResponse: public DeviceResponse {

  InfoResponse(i2c_response_code code, const std::string& response_data);

  virtual ~InfoResponse(){};

  Device::device_type get_device_type();

  std::string get_version();

};

} // namespace atlas

} // namespace dc
#endif
