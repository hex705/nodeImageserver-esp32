
// note this socket library! 
// WebSocketsClient : https://github.com/Links2004/arduinoWebSockets
// may need zip install 
// note the SMALL A -- it is not the official library 



// sd setup -- aug 11 - 2025 

// running on espcam board riding on wb backpack for programming

//board --> AI thinker ESP32-CAM
//serial port : /dev/cu.usbserial-1432730 Serial Port (USB)

// this node is the image source -- it sends fb frames to NODE server on my laptop
// you will need to adjust settings

// at TMU this may require netart routers??? 
// you need devices and laptop on same network -- and you need to know IP of laptop or device running node server



#include "WiFi.h"
#include "esp_camera.h"
//#include "base64.h"

//#include <ArduinoJson.h>
#include <WebSocketsClient.h> // check library above - zip install from github download 
//#include <SocketIOclient.h>

// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22


// Replace with your network credentials

const char* ssid = "someFruit"; //--> network / access point name.
const char* password = "sevenAppleBanana@705!"; //--> network // access point password.

const char* websockets_server_host = "192.168.2.200"; //--> sd laptop - running node
const uint16_t websockets_server_port = 3000; // node server on my laptop 

// Select the IP address according to your local network
IPAddress serverIP(192, 168, 2, 200); // my computer at home 
uint16_t  serverPort = 3000;

WebSocketsClient webSocket;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED: {
      Serial.printf("[WSc] Connected to url: %s\n", payload);
    }
      break;
    case WStype_TEXT:
      Serial.printf("[WSc] get text: %s\n", payload);
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
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

void setupCamera()
{

    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    
    config.frame_size = FRAMESIZE_VGA;// was vga // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 20; //was 10 try 15? and vga
    config.fb_count = 1;
  
    // Init Camera
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
      Serial.printf("Camera init failed with error 0x%x", err);
      return;
    }
  
  
}

void setup(){
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.print("coinnected to " );
  Serial.println(ssid);
  Serial.println("with ip ");
  Serial.println(WiFi.localIP());

  setupCamera();
  
  // server address, port and URL
  webSocket.begin(websockets_server_host, websockets_server_port, "/jpgstream_espCam"); // last element is a path is used in server 
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
  webSocket.enableHeartbeat(15000, 3000, 2); 
    
}


unsigned long messageTimestamp = 0;
void loop() {
    webSocket.loop();
    uint64_t now = millis();

    if(now - messageTimestamp > 300) { // triggers every 30 mS (100 frames) -- but in practice, sends about 10 frames per second
        messageTimestamp = now;

        camera_fb_t * fb = NULL;

        // Take Picture with Camera
        fb = esp_camera_fb_get();  
        if(!fb) {
          Serial.println("Camera capture failed");
          return;
        }
        
        webSocket.sendBIN(fb->buf,fb->len);
  
        Serial.println("Image sent");
        esp_camera_fb_return(fb); 
    }
}
