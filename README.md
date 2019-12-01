# Atlas Scientific Sensor Library
The library was built in order the interface with [Atlas-Scientific](https://www.atlas-scientific.com/) sensors through I2C using the Arduino Wire library.
Furthermore it was built using [platform.io](https://platformio.org/). This library is licensed under the Apache v2.0 license included in the repo.

# Disclaimer
* Basic c++ coding knowledge is assumed
* Platformio is installed, if not please refer to their documentation
* The instructions assume you are using some Linux flavor, if using Windows or CrApple some things may not apply and to be honest I really don't care ;)
* Atlas-Scientific is not related to this code in any way

# Design decisions
* Arduino framework compatible
* Plug n' play (or pray)
* Focuses on using the sensor shields in I2C mode
* DO, PH, ORP, TEMP and EC shields auto-discovery
* Implements a non-blocking loop to communicate with the sensors over the I2C bus

# Including the library in your platform.io project

First clone the the repo somewhere in your computer:

```
$ git clone https://github.com/nemiliani/dc-atlas-scientific.git
$ cd dc-atlas-scientific
$ git checkout develop
```

Create the platformio project somewhere else
```
$ cd ~
$ mkdir my_project
$ cd my_project
$ platformio init
```

Edit your platformio.ini file and include the library under lib_deps, the following config will work for a wemos d1 r2, you need to change it according to the platform and board used

```
[env:wemosd1]
platform = espressif8266
board = d1_mini
framework = arduino
upload_speed = 115200
lib_ldf_mode = deep
lib_deps =
   /path/to/dc-atlas-scientific/src
```
Replace "/path/to/dc-atlas-scientific/src" with the path where the lib was checked out.


# Using the library: setup
Make sure you set up your EZO shields in I2C mode and assign an address that is in range 90 through 120, otherwise you will need to modify the auto discovery code to scan whatever range falls outside.

```c++
#include <Wire.h>
#include <device.h>
#include <device_manager.h>

#define I2C_DO_ADDR 104

/*
Create the device manager. The manager is the main gateway to all your 
sensors. You will use it to loop and access the latests readings.
*/
dc::atlas::DeviceManager device_manager;
dc::atlas::DeviceManager* dm = &device_manager;
dc::atlas::DeviceManager* t = dc::atlas::DeviceManager::get_instance(dm);

void setup(){

  // since it's a WIP I tend to print a lot of crap
  // for debugging purposes
  Serial.begin(115200);
  
  //enable I2C.
  Wire.begin();

  //create the Device, set the type to Disolved Oxygen @ the 104 I2C addr.
  dev = new dc::atlas::Device(
        I2C_DO_ADDR, dc::atlas::Device::DO_SENSOR);
  //add to the device manager and give it a name.
  device_manager.add_device(dev, "do");

  // schedule the read command every 2 seconds.
  device_manager.schedule_command(
        // read command
        "R", 
        // name of the device to use
        "do", 
        // predefined callback that expects a double as a response from the device
        // and updates the device lats read value.
        dc::atlas::DeviceManager::read_double_callback, 
        // execute 5 seconds from now
        5000,
        // fetch the response 900ms after the command was sent out
        900,
        // re-execute every 2 seconds
        2000);
}
```

# Using the library: loop


```c++
void loop(){
  // call the non-blocking loop the get readings
  device_manager.loop();  
  // do whatever, ie : get all the latest readings :
  auto dm = dc::atlas::DeviceManager::get_instance();
  double disolved = dm->get_device_value_double("do")
  
  
} 
```
NOTE : depending on the EZO shield the response time varies from 600 to 900 ms. 

# Upload and look at the serial prints

```
$ platformio run -t upload
$ pio device monitor --port /dev/ttyUSB0 --baud 115200
```

# Internals

Internally the loop sends the read commands to the devices, storing the time and returning in order to not block, it will then track time to know when to request the value of the reading from the EZO shield. If interested in how this works check out the DeviceManager::send_reads and DeviceManager::fetch_responses calls.

# TODO

* Unit test
* Hooking up some CI tool to run unit tests when commiting
* Integrating other sensors ?
* Making it available through the Platformio's library manager