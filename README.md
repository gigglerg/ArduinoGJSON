# GJSON

**GJSON** library is a light weight collection of C++ classes written for Ardunio but is portable and can be used in bare metal environments and works great with [Free Real Time Operating System (FRTOS)](https://www.freertos.org/).

Use GJSON as part of your IoT project as encoder and decoder for simplified JSON object with string parsing and building.  The small collection of classes include everything required to communicate 
JSON between NodeJS, Python or PHP as a data source (backend) using whatever REST API you wish.  These classes allow quick and easy development with some examples of various use cases, created with enough consideration to make them expandable; 
plus you can include them into your favourite RTOS.

There are lots of JSON parsing options that you may consider, class used, parsing method, inheritance, objects or just remain with strings, source and destination of parsing (I/O), validating brackets, etc..  The examples don't cover every possible coding 
option but should be enough to get you started on your journey.

[License: GPL2](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html)

I can't force you, but if your work is part of a commerical project or product, please consider a proportional donation to [The Oxford Foodbank](http://oxfordfoodbank.org/support-us/make-a-donation/)


## Using the library

For now as this is a library, Doxygen documentation is available as follows:

* [All variant documentation](https://gigglerg.github.io/ArduinoGJSON/release_100/avr8_html/index.htm)

To rebuild simply use make from within the "./docs" folder.  Use "make clean" to tidy.  Doxygen 1.8.15 is required without DOT.


## Requirements

* Supports
  * Arduino ([AVR8 Mega single flash bank](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf)) devices
    * [Arduino Uno/Nano](https://store.arduino.cc/arduino-uno-rev3)

  * Arduino ([Due AT91SAM](https://store.arduino.cc/due))
    
  * [STM32 Cortex M series](https://www.st.com/en/microcontrollers-microprocessors/stm32-32-bit-arm-cortex-mcus.html)

  * Bare metal MCu development, Arduino isn't required
  
  * Definable use of dynamic memory allocation

  * Custom I/O instances for data to come from and go to other media

* Restrictions

  * Duplicate keys arn't checked

  * Does not fully support JSON string escapes


## Library Installation

GJSON is available for installation in the [Arduino Library Manager].  Alternatively it can be downloaded from GitHub using the following:

1. Download the ZIP file (pick a release branch) from https://github.com/gigglerg/ArduinoGJSON
2. Rename the downloaded file to `GJSON.zip`
3. From the Arduino IDE: Sketch -> Include Library... -> Add .ZIP Library...
4. Restart the Arduino IDE to see "GJSON" library with examples

See the comments in the example sketches for coding use details.


### Include library

```cpp
// Include all headers
#include "gjson.h"

using namespace gjson;
```

The above includes everything but if you want to be specific, look at the source and include individual headers as desired.  Look at the few examples for ideas and further information.

The main include has default macros which you can overide by defining them before including.  These include but not limited to:

* Macros
  * GJSON_PRINT - Output control, mainly for examples
  * GJSON_PRINTLINE
  * GJSON_PRINTSTR
  * GJSON_PACKING - Library packing, default 1.  For ARM use 4
  * GJSON_NEW_OBJECT - RTOS intergration helpers for dynamic memory allocation.  new and delete are used if not overriden
  * GJSON_RELEASE_OBJECT
  * GJSON_DEFAULT_EXPONENTIAL_NOTATION - Choose to except JSON literals in expoential notation, due to limited resource targets, you may want to set as false

The parser was designed to be subclassed, create your own to suite your specific requirements or use one of the few included with library source.   You can choose to work with and remain with 
JSON strings or use in-memory objects to manipulate your JSON and at anytime convert your objects backing into a JSON string.  If using objects, some builder helpers exists to allow attribute/property 
creation, including those that create new maps or arrays.

You can control where your JSON strings come from or get written to by an I/O instance which you inherit from; implementing storage to suit your project.  Shipped with library are Null I/O, 
User String I/O and Buffered I/O.  These cover pretty much the standard use cases but if you have shared memory or RTOS requirements, customise.


## TODOs

* 1).
Apart from refactoring to include std lib when available for Arduino or perhaps Arduino streams.
* 2).
Support JSON string escapes


## Gotchas

None yet.


## Contributing

Contributions to the code are welcome using the normal Github pull request workflow.

Please remember the small memory footprint of most micro controllers like Arduino, so try and be efficient when adding new features.


### Code Style

When making contributions, please follow [Google's code style](https://google.github.io/styleguide/cppguide.html) where possible to keep the codebase consistent.

* Indent *4 spaces*, No hard tabs
* EOL Unix (Library code)
* The rest is online!
