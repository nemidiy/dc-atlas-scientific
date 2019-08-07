#ifndef __GJ_ATLAS_DEVICE_COMMAND_H__
#define __GJ_ATLAS_DEVICE_COMMAND_H__
#include <Arduino.h>


namespace dc {

namespace atlas {

struct DeviceCommand {

    typedef enum {
      INFO   = 0,
      STATUS = 1,
      READ   = 2,
    } device_command;

};

} // namespace atlas

} // namespace dc

#endif
