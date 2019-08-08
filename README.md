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

  /*
  auto discovery this will trigger auto discovery from port 90
  through port 120. This call will print out on the serial port
  the ports scanned and what it found.
  */
  device_manager.auto_discovery();
  
  for (auto& kv : device_manager.get_all_devs()) {
    /*
    Iterate over the devices found. Where :
    kv.first : the device type
    kv.second : the device instance
    */
  }
}
```

Executing the auto-discovery will print out on the serial port the following :
```
sending command to port 90
	* response code is DeviceResponse::NO_DATA
---------------------------------------------

...

---------------------------------------------
sending command to port 98
	* response code is DeviceResponse::NO_DATA
---------------------------------------------
sending command to port 99
	* response code is DeviceResponse::NO_DATA
---------------------------------------------
sending command to port 100
	* response code is DeviceResponse::SUCCESS
	* response data is [?I,RTD,2.10]
	* device_type 2
	* firmware version 2.10
---------------------------------------------
sending command to port 101
	* response code is DeviceResponse::SUCCESS
	* response data is [?I,pH,2.12]
	* device_type 0
	* firmware version 2.12
---------------------------------------------
sending command to port 102
	* response code is DeviceResponse::SUCCESS
	* response data is [?I,ORP,2.10]
	* device_type 3
	* firmware version 2.10
---------------------------------------------
sending command to port 103
	* response code is DeviceResponse::SUCCESS
	* response data is [?I,EC,2.13]
	* device_type 1
	* firmware version 2.13
---------------------------------------------
sending command to port 104
	* response code is DeviceResponse::SUCCESS
	* response data is [?I,DO,2.13]
	* device_type 4
	* firmware version 2.13
---------------------------------------------
sending command to port 105
	* response code is DeviceResponse::NO_DATA
---------------------------------------------
sending command to port 106
	* response code is DeviceResponse::NO_DATA

... all the way to port 120
```
Here the lib detected :
* The RTD EZO in port 100
* The PH EZO in port 101
* The ORP EZO in port 102
* The EC EZO in port 103
* The DO EZO in port 104

# Using the library: loop

Once all the devices were found you need to loop over them in order to get the readings

```c++
void loop(){
  // call the non-blocking loop the get readings
  device_manager.loop();  
  // do whatever, ie : get all the latest readings :
  double temperature = dc::atlas::DeviceManager::get_instance() \
                    ->get_device_value(dc::atlas::Device::TEMP_SENSOR);
  double ph = dc::atlas::DeviceManager::get_instance() \
                    ->get_device_value(dc::atlas::Device::PH_SENSOR);
  double disolved = dc::atlas::DeviceManager::get_instance() \
                    ->get_device_value(dc::atlas::Device::DO_SENSOR);
  double orp = dc::atlas::DeviceManager::get_instance() \
                    ->get_device_value(dc::atlas::Device::ORP_SENSOR);
  double ec = dc::atlas::DeviceManager::get_instance() \
                    ->get_device_value(dc::atlas::Device::EC_SENSOR);
  
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

The Device class implements a read method that sends the read command over the I2C bus and returns a lambda function (thank u 11 std) that has the code and context that needs to be executed after the time needed by the EZO shield has gone by. All of this is orchestrated by the DeviceManager loop using the send_reads and fetch_responses calls.


# TODO

* Unit test
* Hooking up some CI tool to run unit tests when commiting
* Integrating other sensors ?
* Making it available through the Platformio's library manager
* Serial support
