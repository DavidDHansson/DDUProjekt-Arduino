#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// ----------------------- OLED ----------------------- 
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define SCREEN_ADDRESS 0x3C
// -
#define LOCK_DIM 19
static const unsigned char PROGMEM lock_bmp[] = {
0x01, 0xf0, 0x00, 0x07, 0xfc, 0x00, 0x0f, 0xfe, 0x00, 0x1f, 0xff, 0x00, 0x3f, 0x1f, 0x80, 0x3e, 
0x0f, 0x80, 0x7e, 0x0f, 0xc0, 0x7e, 0x0f, 0xc0, 0x7f, 0x1f, 0xc0, 0x7f, 0xff, 0xc0, 0x7f, 0xff, 
0xc0, 0x3f, 0xff, 0x80, 0x3f, 0xff, 0x80, 0x1f, 0xff, 0x00, 0x0f, 0xfe, 0x00, 0x03, 0xf8, 0x00, 
0x07, 0xfc, 0x00, 0x0f, 0xfe, 0x00, 0x0f, 0xfe, 0x00, 0x01, 0xf8, 0x00, 0x01, 0xf8, 0x00, 0x03, 
0xf8, 0x00, 0x01, 0xf8, 0x00, 0x00, 0x78, 0x00, 0x00, 0xf8, 0x00, 0x03, 0xf8, 0x00, 0x03, 0xf8, 
0x00, 0x00, 0xf8, 0x00, 0x00, 0x78, 0x00, 0x00, 0xf8, 0x00, 0x01, 0xf8, 0x00, 0x01, 0xf8, 0x00, 
0x01, 0xf8, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x70, 0x00, 0x00, 0x20, 0x00
};


// ----------------------- WiFi ----------------------- 
const char* ssid = "David";
const char* password = "lmaoboi123";


// ----------------------- PASS ----------------------- 
int laps = 30; // Sekunder for laps
int lap = laps - 1;
String code = "------";
bool shouldUpdateUI = true;
const char* host = "europe-west1-ddu-eksamensprojekt.cloudfunctions.net";
const String email = "fuckdig@gmail.com";


void setup() {
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.display();
  delay(2000);
  draw();

  // Connect to wifi
  Serial.begin(9600);
  
  WiFi.begin(ssid, password);

  // Log wifi process
  Serial.println("");
  Serial.print("(╯°□°)╯︵ ┻━");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("━");
  }
  Serial.print("┻");
  Serial.println("");
  Serial.print("Connnected til: ");
  Serial.println(ssid);
}

void loop() {
  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("Wifi gone");
    return; 
  }
  
  if(lap == laps) {
    changeCode();
    
    Serial.println("Updating code with server...");
    String url = "https://europe-west1-ddu-eksamensprojekt.cloudfunctions.net/updatecode?email=" + email + "&code=" + code;
    
    WiFiClientSecure client;
    const int httpPort = 443; // 443 = https

    client.setInsecure();

    // Check the host is open
    if (!client.connect(host, httpPort)) {
      Serial.println("connection failed");
      lap = 0;
      return;
    }
  
    Serial.println("With URL: " + url);

    // Make GET request
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");

    Serial.println("closing connection");
    
    lap = 0;
    
  } else {
    draw();
    delay(1000);
    lap++;
  }
}

void draw() {
  
  // Progress bar
  float p = float(lap)/float(laps) * 10; 
  float bar = float(p - 0) / float(10 - 0) * (82 - 0);
  display.drawRect(34, 40, bar, 2, SSD1306_WHITE);
  display.display();
  
  if(!shouldUpdateUI) { return; }
  
  display.clearDisplay();

  // Display progress bar border
  display.drawRect(34-1, 40-1, 82-1, 4, SSD1306_WHITE);
  display.display();

  // Display Text
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(32, 15);
  display.println("#" + code);
  display.display();

  // Draw border
  display.drawRect(0, 0, display.width(), display.height(), SSD1306_WHITE);
  display.display();

  // Draw Lock
  display.drawBitmap(
    6, 14,
    lock_bmp,
    LOCK_DIM, 36,
    1);
  display.display();

  shouldUpdateUI = false;
}

void changeCode() {
  char alfa[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  char numbers[] = "0987654321";

  code = "";
  for(int i = 0; i < 6; i++) {
    code += random(2) == 1 ? alfa[random(sizeof(alfa) - 1)] : numbers[random(sizeof(numbers) - 1)];
  }

  shouldUpdateUI = true;
}
