# Advanced Wifi Http Server

A Library for ESP8266 devices (and Arduinos in the future) that provides an advanced REST API to control the modules. The main goal is to have a library which can be easily flashed on any ESP8266 devie and then those can be controlled without writing any extra or specialized code.
The reason it is not using MQTT is that this way those IoT modules can be used standalone without need to setup broker server. With this approach all you need is a terminal and curl to control your device (after flashing iwth the library).

- Provides API to gateway the HTTP request data to and from the serial
- Provides API to initialize, read and write digital and analog pins.

An example ino file can be found at `docs/examples/arduino.ino`

---

## Endpoints

---

### /serial

#### `GET /serial`
If there's data waiting on the serial, reads it and returns in the HTTP response

##### Examples
`curl -i -X GET http://92c1c372.domdetre.com/serial`


#### `POST /serial --data {data}`
Sends the data to the serial and returns without waiting.

##### Examples
`curl -i -X POST --data something http://92c1c372.domdetre.com/serial`

---

### /digital


####  `PUT /digital/{pinNumber} --data (output|input|input_pullup)`
Initializes the digital pin {pinNumber} to either as Output or Input pin.

##### Examples
`curl -i -X PUT --data output http://92c1c372.domdetre.com/digital/1`

##### Notes
- Must be called before POSTing to any or GETting from any digital endpoint


#### `GET /digital/{pinNumber}`

##### Examples
`curl -i -X GET http://92c1c372.domdetre.com/digital/1`

##### Notes
  - If the pin is not initialized, will return error.
  - If the pin is initialized as input, will read the state of the pin.
  - If the pin is initialized as output, will return the stored state of the pin.
  - Only accepts numeric value for pinNumber and will be translated to the corresponding GPIO pin number
  - Supports up to 15 digital pins.


#### `POST /digital/{pinNumber}  --data (0|1|low|high)`
Sets the output of the digital pin.

##### Examples
`curl -i -X POST --data high http://92c1c372.domdetre.com/digital/1`

##### Notes
  - If the pin is not initialized, will return error. You need to PUT the digital endpoint to a mode.
  - If the pin is initialized as input, will return error.  
  - If the pin is initialized as output, then sets the state.  


####  `DELETE /digital/{pinNumber}`
Reverts the effect of PUTting the pin into a mode. Unsets the pin mode to allow it to be reinitialized.

##### Examples
`curl -i -X DELETE http://92c1c372.domdetre.com/digital/1`

##### Notes

---

## ESP8266


### Pins

- ESP8266WiFi.h has definitions for digital pin mappings from D0 to D15
- The LED_BUILTIN and D4 is the same. So the builtin led is wired to the digital 4 pin in inverse mode. LOW = on, HIGH = off.
- The digital pins are HIGH before initialization. So inverse mode is preferred.
- INPUT 0 OUTPUT 1 INPUT_PULLUP 2


| Digital | GPIO |          |             |
|:-------:|:----:|:--------:|:-----------:|
|   D0    |  16  |   User   |    Wake     |
|   D1    |  5   |          |             |
|   D2    |  4   |          |             |
|   D3    |  0   |  Flash   |             |
|   D4    |  2   |   TXD1   | LED_BUILTIN |
|         |      |   3V3    |             |
|         |      |   GND    |             |
|   D5    |  14  |  HSCLK   |             |
|   D6    |  12  |  HMISO   |             |
|   D7    |  13  |  HMOSI   |    RXD2     |
|   D8    |  15  |   HCS    |    TXD2     |
|         |  3   |    RX    |             |
|         |  1   |    TX    |             |
|         |      |   GND    |             |
|         |      |   3V3    |             |
|         |      |          |             |
|   A0    |      |   TOUT   |             |
|         |      | Reserved |             |
|         |      | Reserved |             |
|         |  10  |   SDD3   |             |
|         |  9   |   SDD2   |             |
|         |      |   SDD1   |             |
|         |      |  SDCMD   |             |
|         |      |   SDD0   |             |
|         |      |  SDCLK   |             |
|         |      |   GND    |             |
|         |      |   3v3    |             |
|         |      |  Enable  |             |
|         |      |  Reset   |             |
|         |      |   GND    |             |
|         |      |   VIN    |             |


## Compile size results

- Wemos/Lolin D1 R2 & mini  
  Sketch uses 275620 bytes (26%) of program storage space. Maximum is 1044464 bytes.
  Global variables use 30936 bytes (37%) of dynamic memory, leaving 50984 bytes for local variables. Maximum is 81920 bytes. 
