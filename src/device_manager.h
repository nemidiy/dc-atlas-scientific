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

#ifndef __DC_ATLAS_DEVICE_MANAGER_H__
#define __DC_ATLAS_DEVICE_MANAGER_H__
#include <map>
#include <device.h>

namespace dc {

namespace atlas {

struct DeviceManager{

    DeviceManager(unsigned long int loop_millis=1000);

    virtual ~DeviceManager();

    static
    DeviceManager* get_instance(DeviceManager* i = NULL);

    void add_device(
            Device::device_type t,
            Device* dev);

    double get_device_value(Device::device_type t);

    void loop();

    int auto_discovery();

    std::map<dc::atlas::Device::device_type, dc::atlas::Device*>
    get_all_devs(){
        return all_devs;
    }

    void track_read(dc::atlas::Device::device_type t, dc::atlas::Device::read_function f);

private:

    void send_reads();

    void fetch_responses();

    std::map<
    dc::atlas::Device::device_type,dc::atlas::Device*> all_devs;
    std::map<
    dc::atlas::Device::device_type, dc::atlas::Device::read_function> reads;

    unsigned long int loop_millis;
    unsigned long int last_run;

    static DeviceManager* instance;
};

} // namespace atlas

} // namespace dc

#endif