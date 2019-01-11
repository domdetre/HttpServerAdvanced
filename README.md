# Advanced Wifi Http Server

- Provides API to gateway the HTTP request data to and from the serial
- Provides API to initialize, read and write digital and analog pins.

## Endpoints

- GET /serial  
  If there's data waiting on the serial, reads it and returns in the HTTP response

- POST /serial --data {data}  
  Sends the data to the serial and returns without waiting.

- GET /digital/{pinNumber}  
  If the pin is initialized as input, then will return the state of the pin.
  If the pin is initialized as output, then will return the stored state of the pin.

- POST /digital/{pinNumber}  
  If the pin is initialized as input, then nothing will happen.  
  If the pin is initialized as output, then set the state.  

- PUT /digital/{pinNumber} --data {output|input}  
  initializes the digital pin {pinNumber} to either as Output or Input pin.

- GET /analog/{pinNumber}  
  Reads the value from the analog pin {pinNumber}. The pin number can be either numeric or the usual format A#.  
  @see https://www.arduino.cc/reference/en/language/functions/analog-io/analogread/  

- PUT /analog/{pinNumber} --data {value}  
  This is an endpoint for analogWrite function. The pinNumber is the digital pin and the value is the duty cycle.  
  @see https://www.arduino.cc/reference/en/language/functions/analog-io/analogwrite/  

- PUT /dht11 --data {digitalPinNumber}  

- PUT /hc-sr04 --data "echo:{digitalPinNumber} trig:{digitalPinNumber}"  

- PUT /debug --data {true|false}  
  Turns on or off debugging on serial. Note that any attached devices will get the debug data.  

## TODOs

- check the stored pinmode and pinstate if output at getPinState and setPinState and initPin
- store pinstate at setPinState
- store pinmode at initpin
- get analog
- put analog
- put dht11
- put hc-sr04
- put debug

## Puzzles

- Nodemcu + eeporm ?

## Compile size results

- Wemos D1 R1  
  Sketch uses 263916 bytes (25%) of program storage space. Maximum is 1044464 bytes.  
  Global variables use 29184 bytes (35%) of dynamic memory, leaving 52736 bytes for local variables. Maximum is 81920 bytes.  
