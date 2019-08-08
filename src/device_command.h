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
