// does not work on huzzah
// may work on wrover board --- testing to see 



// moding the original example into one thate can listen to node

// this needs to be converted from AP to station -- and tehn make a client connection to node
// identifying as a borwser?? it may need specific eps server ? 


// https://www.youtube.com/watch?v=3d7-p7xEOu0


/*

esp32 huzzah with TFT screen attached

the esp32 is a server that the camera can connect to
// huzzah is in AP (router) mode)

TFT has a config file [User_Setup.h] in the library fold where you can set pins

physical connections:

Vcc  :: USB (5V)
GN - GND
CS to D15 (chip select)
RESET -- RESET ( in config set to -1)
DC - D17
MOSI - 18 ( mosi ) 
SCK - 5 (sck)
LED - 3V
MISO - 19 ( miso )

*/


// iteration 1 going to try arduino websockets .. but not sure ??? 
// we know the algorythm here can catch the image and present it.


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 03_ESP32-CAM_to_ESP32_--_Server_-_ESP32_TFT_LCD_ILI9341
//----------------------------------------Including Libraries.
#include <SPI.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
 //#include <ArduinoWebsockets.h>
 #include <WebSocketsClient.h>

#include <WiFi.h>
//----------------------------------------

//----------------------------------------Defines the TFT LCD size and font size.
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240
#define FONT_SIZE 2
//----------------------------------------

// this was AP mode -- removing 

//----------------------------------------Access Point Declaration and Configuration.
// const char* ssid = "ESP32CAM_to_ESP32"; //--> access point name.
// const char* password = "myesp32server"; //--> access point password.

// // Use this IP address (local_ip) in the ESP32-CAM (client) program code.
// // Use it in the "websockets_server_host" variable.
// IPAddress local_ip(192,168,1,1);

// IPAddress gateway(192,168,1,1);
// IPAddress subnet(255,255,255,0);
//----------------------------------------

// here is station mode ocnnection to NODE on laptop
// Replace with your network credentials

const char* ssid = "someFruit"; //--> access point name.
const char* password = "sevenAppleBanana@705!"; //--> access point password.

const char* websockets_server_host = "192.168.2.200"; //--> sd laptop at home 
const uint16_t websockets_server_port = 3000; // node server on my laptop 

unsigned long start = 0; 
unsigned long interval = 50;

int centerX, centerY;



WebSocketsClient webSocket;

TFT_eSPI tft = TFT_eSPI();



//________________________________________________________________________________ tft_output()
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
  // Stop further decoding as image is running off bottom of screen.
  if ( y >= tft.height() ) return 0;

  // This function will clip the image block rendering automatically at the TFT boundaries.
  tft.pushImage(x, y, w, h, bitmap); //trying /2 to scale image <-- nope

  // This might work instead if you adapt the sketch to use the Adafruit_GFX library.
  // tft.drawRGBBitmap(x, y, bitmap, w, h);
  
  // Return 1 to decode next block.
  return 1;
}
//________________________________________________________________________________ 

//void websockets::webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

  //WebSocket &ws, const WebSocket::DataType dataType,
             //       const char *message, uint16_t length)
// hmm -- need to restructur this ...
// https://arduinogetstarted.com/tutorials/arduino-websocket

//  //void websockets::webSocketEvent(WebsocketsClient& c, WebsocketsEvent type, WSInterfaceString payload){
//  void webSocketEvent([&] (WebSocket &ws, const WebSocket::DataType dataType,
//                     const char *payload, uint16_t length) {

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  Serial.print("socket event ");
  Serial.println(type);
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED: {
      Serial.printf("[WSc] Connected to url: %s\n", payload);
             webSocket.sendTXT("Hello Server");
    }
      break;
    case WStype_TEXT:
      Serial.printf("[WSc] get text: %s\n", payload);
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      Serial.println("this is likely image data call a function? ");
      getImage(payload, length);

//     uint32_t t = millis();

//     // Get the width and height in pixels of the jpeg if you wish.
//     uint16_t w = 0, h = 0;
//     TJpgDec.getJpgSize(&w, &h, payload, length);
//  //    TJpgDec.getJpgSize(&w, &h, (const uint8_t*)msg.c_str(), msg.length());
//     Serial.print("Width = "); Serial.print(w); Serial.print(", height = "); Serial.println(h);

//     // Draw the image, top left at 0,0.
//     TJpgDec.drawJpg(0, 0, payload, length);
//     //   TJpgDec.drawJpg(0, 0, (const uint8_t*)msg.c_str(), msg.length());
  
//     // How much time did rendering take (ESP8266 80MHz 271ms, 160MHz 157ms, ESP32 SPI 120ms, 8bit parallel 105ms.
//     t = millis() - t;
//     Serial.print(t); Serial.println(" ms");
      break;

    case WStype_PING:
        // pong will be send automatically
        Serial.printf("[WSc] get ping\n");
        break;
    case WStype_PONG:
        // answer to a ping we send
        Serial.printf("[WSc] get pong\n");
        break;
    }


}


//________________________________________________________________________________ VOID SETUP()
void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial.println();
  delay(3000);


  // again we are turning this into a client not a server
  // ----------------------------------------Create ESP32 asSTATION to NODE server
   // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // now to connect the socket ... 
    // server address, port and URL
    // not the URL is webClient -- like a browser, not like a camera 
    
//  // webSocket.begin(websockets_server_host, websockets_server_port, "/jpgstream_webClient"); // last element is a path is used in server 
//   webSocket.begin(websockets_server_host, websockets_server_port, "/jpgstream_tftClient");  
//   webSocket.onEvent(webSocketEvent);
//   webSocket.setReconnectInterval(5000);
//   webSocket.enableHeartbeat(15000, 3000, 2); 

// set up the screen 
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLUE);

  // Set X and Y coordinates for center of display.
  centerX = SCREEN_WIDTH / 2;
  centerY = SCREEN_HEIGHT / 2;

  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.drawCentreString("Waiting for images", centerX, centerY - 15, FONT_SIZE);
  tft.drawCentreString("from NODE server", centerX, centerY + 5, FONT_SIZE);
  Serial.println();
  Serial.println("Waiting for images from node server .");
  
  // We need to swap the colour bytes (endianess).
  tft.setSwapBytes(true); 

  // The jpeg image can be scaled by a factor of 1, 2, 4, or 8.
  TJpgDec.setJpgScale(1);

  // The decoder must be given the exact name of the rendering function above.
  TJpgDec.setCallback(tft_output);

   // webSocket.begin(websockets_server_host, websockets_server_port, "/jpgstream_webClient"); // last element is a path is used in server 
  webSocket.begin(websockets_server_host, websockets_server_port, "/jpgstream_tftClient");  
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(1000);
  webSocket.enableHeartbeat(15000, 3000, 2); 


}
//________________________________________________________________________________ 



//________________________________________________________________________________ VOID LOOP()
void loop() {
  // put your main code here, to run repeatedly:
   webSocket.loop();
  

    uint64_t now = millis();
    if(now -start> interval){
     //  webSocket.onEvent(webSocketEvent);
       start += interval;
    }
  
  // if(server.poll()){
  //   client = server.accept();
  // }

//NEED TO LINK THE CODE BELOW TO A SOCKET EVENT
  

  // if(client.available()){
  //   client.poll();

  //   WebsocketsMessage msg = client.readBlocking();

  //   uint32_t t = millis();

  //   // Get the width and height in pixels of the jpeg if you wish.
  //   uint16_t w = 0, h = 0;
  //   TJpgDec.getJpgSize(&w, &h, (const uint8_t*)msg.c_str(), msg.length());
  //   Serial.print("Width = "); Serial.print(w); Serial.print(", height = "); Serial.println(h);

  //   // Draw the image, top left at 0,0.
  //   TJpgDec.drawJpg(0, 0, (const uint8_t*)msg.c_str(), msg.length());
  
  //   // How much time did rendering take (ESP8266 80MHz 271ms, 160MHz 157ms, ESP32 SPI 120ms, 8bit parallel 105ms.
  //   t = millis() - t;
  //   Serial.print(t); Serial.println(" ms");
  // } 
}
//________________________________________________________________________________ 
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void getImage(uint8_t * payload, size_t length){
    uint32_t t = millis();

    // Get the width and height in pixels of the jpeg if you wish.
    uint16_t w = 0, h = 0;
    TJpgDec.getJpgSize(&w, &h, payload, length);
 //    TJpgDec.getJpgSize(&w, &h, (const uint8_t*)msg.c_str(), msg.length());
    Serial.print("Width = "); Serial.print(w); Serial.print(", height = "); Serial.println(h);

    // Draw the image, top left at 0,0.
    TJpgDec.drawJpg(0, 0, payload, length);
    //   TJpgDec.drawJpg(0, 0, (const uint8_t*)msg.c_str(), msg.length());
  
    // How much time did rendering take (ESP8266 80MHz 271ms, 160MHz 157ms, ESP32 SPI 120ms, 8bit parallel 105ms.
    t = millis() - t;
    Serial.print(t); Serial.println(" ms");

}
