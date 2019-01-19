# Advanced Wifi Http Server

- Provides API to gateway the HTTP request data to and from the serial
- Provides API to initialize, read and write digital and analog pins.


## Endpoints


### /serial

- GET /serial  
  - If there's data waiting on the serial, reads it and returns in the HTTP response


- POST /serial --data {data}  
  - Sends the data to the serial and returns without waiting.


### /digital

- GET /digital/{pinNumber}  
  - If the pin is not initialized, will return error.
  - If the pin is initialized as input, will read the state of the pin.
  - If the pin is initialized as output, will return the stored state of the pin.
  - Only accepts numeric value for pinNumber and will be translated to the corresponding GPIO pin number
  - Supports up to 15 digital pins.


- POST /digital/{pinNumber}  --data {0|1|low|high}
  - If the pin is not initialized, will return error.
  - If the pin is initialized as input, will return error.  
  - If the pin is initialized as output, then sets the state.  


- PUT /digital/{pinNumber} --data {output|input}  
  - Initializes the digital pin {pinNumber} to either as Output or Input pin.


### /analog

- GET /analog/{pinNumber}  
  - Reads the value from the analog pin {pinNumber}. The pin number can be either numeric or the usual format A#.  
  @see https://www.arduino.cc/reference/en/language/functions/analog-io/analogread/  


- PUT /analog/{pinNumber} --data {value}  
  - This is an endpoint for analogWrite function. The pinNumber is the digital pin and the value is the duty cycle.  
  @see https://www.arduino.cc/reference/en/language/functions/analog-io/analogwrite/  


### Sensors

- PUT /dht11 --data {digitalPinNumber}  

- PUT /hc-sr04 --data "echo:{digitalPinNumber} trig:{digitalPinNumber}"  

- PUT /debug --data {true|false}  
  Turns on or off debugging on serial. Note that any attached devices will get the debug data.  


## TODOs

- get analog
- put analog
- put dht11
- put hc-sr04
- put debug
- software serial option for the output
- option to disable serial relay
- options to add custom endpoints
- option to have multiple relay client
- endpoint to get the available endpoints
- endpoint for getting help
- endpoint to get system informations


## Puzzles

- should we allow pins to be reinitialized?
  - Pin modes are stored in the eeprom, in case the device reboot and restores mode on reboot, meaning we should


## ESP8266


### Pins

- ESP8266WiFi.h has definitions for digital pin mappings from D0 to D15
- The LED_BUILTIN and D4 is the same. So the builtin led is wired to the digital 4 pin in inverse mode. LOW = on, HIGH = off.
- The digital pins are HIGH before initialization. So inverse mode is preferred.
- INPUT 0 OUTPUT 1 INPUT_PULLUP 2


## Compile size results

- Wemos D1 R1  
  Sketch uses 271280 bytes (25%) of program storage space. Maximum is 1044464 bytes.  
  Global variables use 30260 bytes (36%) of dynamic memory, leaving 51660 bytes for local variables. Maximum is 81920 bytes.  
