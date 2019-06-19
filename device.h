#ifndef __GJ_ATLAS_DEVICE_H__
#define __GJ_ATLAS_DEVICE_H__
#include <map>
#include <string>
#include <vector>
#include <Arduino.h>

#include "device_response.h"

#define DEFAULT_I2C_ADDRESS_PH 99
#define DEFAULT_I2C_ADDRESS_EC 100
#define DEFAULT_I2C_ADDRESS_TEMP 101
#define DEFAULT_I2C_ADDRESS_ORP 102
#define DEFAULT_I2C_ADDRESS_PUMP 103
#define DEFAULT_I2C_ADDRESS_FLOW 104
#define DEFAULT_I2C_ADDRESS_DO 105

#define I2C_ADDRESS_SCAN_START 90
#define I2C_ADDRESS_SCAN_FINISH 120

namespace gj {

namespace atlas {

struct Device{

    typedef enum {
      PH_SENSOR   = 0,
      EC_SENSOR   = 1,
      TEMP_SENSOR = 2,
      ORP_SENSOR  = 3,
      DO_SENSOR   = 4,
      FLOW_SENSOR = 5,
      PUMP        = 6,
      UNKNOWN     = 7,
    } device_type;

    //constructor
    Device(int i2c_addr, device_type t);

    //copy constructor
    Device(const Device& d);

    //destructor
    ~Device();

    device_type get_device_type(){
      return dev_type;
    }

    double read();

    static void send_i2c_command(
        int address,
        const char* cmd);

    static DeviceResponse::i2c_response_code read_i2c_response(
        int address,
        char* buffer,
        int buffer_size);


  private:

    //i2c address for the device
    int i2c_addr;

    //device_type
    device_type dev_type;
};

} // namespace atlas

} // namespace gj

#endif
