**v01:**

*system*:  espcam to NODE server on laptop -- relay to p5js AND wrover with tft


**esp code** uses this  socket library:
// note this socket library! 
// WebSocketsClient : *https://github.com/Links2004/*arduinoWebSockets
// may need zip install 
// note the SMALL A -- it is not the official library 


**esp cam** is compiled as:

//board --> AI thinker ESP32-CAM
//serial port : /dev/cu.usbserial-1432730 Serial Port (USB)

**wrover** is compiled as :
esp32 wrover module (i think it needs a board extention)
and needs usb-c driver
/dev/cu.wchusbserial1410 Serial Port (USB)

** get link**

**set pins for tft** in the ***TFT_eSPI***  library 
file:  *User_Setup.h*



**to set up node server** - in terminal nav to main folder execute:
npm install



**server does not stop well** -- on OSX used activity monitor to stop processed

may also need terminal command:
sudo kill -9 *pid#*
PW

get pid from activity monitor
 
