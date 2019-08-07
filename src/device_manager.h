#ifndef __GJ_ATLAS_DEVICE_MANAGER_H__
#define __GJ_ATLAS_DEVICE_MANAGER_H__
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