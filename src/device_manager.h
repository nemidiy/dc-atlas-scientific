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

    /**
    Function type definition for the response callback
    @param dev is the device
    @param code is the I2C response code
    @param buffer is the response data
    @param size of the response buffer
    */
    typedef std::function<
        void* ( Device* dev,
                DeviceResponse::i2c_response_code code,
                char* buffer,
                unsigned int buffer_size)
        > response_callback;

    //constructore
    DeviceManager();

    //destructor
    virtual ~DeviceManager();

    /**
    Singleton
    */
    static DeviceManager* get_instance(DeviceManager* i = NULL);

    /**
    Get the last value we read from a device

    @param name is the device name
    @return double is the value that was read for the last time
    */
    double get_device_value_double(const std::string& dev_name) const;

    /**
    Main loop of the device manager
    */
    void loop();

    /**
    Auto discovery of devices in the I2C bus. This function scans from addres
    90 to 120 looking for devices using the info (I) command. As a result, all
    devices found will be added to the device manager instance
    */
    int auto_discovery();

    /**
    Adds a device to the device manager

    @param dev is a pointer to the device to be added
    @param name: is the name for the device, if set to "" the device will get named
                 as <device_type>_<random_id>
    @return string the name assigned to the device
    */
    std::string add_device(dc::atlas::Device* dev, const std::string& name="");

    /**
    Schedule a command to be sent out on the I2C buffer

    @param command is the command to be sent out
    @param device_name is the name of the device
    @param callback is the callback function to be executed when the response arrives
    @param exec_millis is the relative milliseconds to execute the command
    @param response_millis is the time we need to wait before picking the response from the I2C bus
    @param auto_interval if != 0 then auto re-schedule the command in autointerval millisecs
    */
    void schedule_command(
            const std::string& command,
            const std::string& device_name,
            response_callback& callback,
            unsigned long int exec_millis,
            unsigned long int response_millis,
            unsigned long int auto_interval=0);

    /**
    Returns a device by name

    @param name is the device name
    @return the device, null if not found
    */
    dc::atlas::Device* get_device(const std::string& name) const;

    // generic null callback
    static response_callback null_callback;

    /*
    predefined callback to read a double and set it as the
    last value of a device
    */
    static response_callback read_double_callback;

private:

    /**
    send all the command that are in the scheduling queue
    */
    void send_scheduled();

    /**
    fetch responses from sent commands
    */
    void fetch_responses();

    // devices
    std::map<
    std::string,dc::atlas::Device*> devices;

    /**
    Definition of the scheduling and response queue togheteher with the structure
    */
    typedef struct {
        std::string command;
        std::string device_name;
        response_callback callback;
        unsigned long int response_time_millis;
        unsigned long int fetch_time_millis;
        unsigned long int exec_time_millis;
        unsigned long int auto_interval;
    } schedule;
    std::vector<schedule> scheduling_queue;
    std::vector<schedule> response_queue;

    static DeviceManager* instance;
};

} // namespace atlas

} // namespace dc

#endif