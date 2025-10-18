#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <HardwareSerial.h>
#include <TinyGPSPlus.h>

// OLED setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// GPS
TinyGPSPlus gps;
HardwareSerial SerialGPS(2);  // UART2: RX=16, TX=17

void setup() {
  Serial.begin(9600);
  SerialGPS.begin(9600, SERIAL_8N1, 16, 17);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED init failed");
    while (1);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Booting...");
  display.display();
  delay(1000);
}

void loop() {
  // Feed GPS data into parser
  while (SerialGPS.available()) {
    gps.encode(SerialGPS.read());
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);

  // Always show satellite count and UTC time if possible
  display.print("Sats: ");
  if (gps.satellites.isValid()) {
    display.println(gps.satellites.value());
  } else {
    display.println("--");
  }

  if (gps.time.isValid()) {
    display.setTextSize(2);
    display.printf("%02d:%02d",gps.time.hour(), gps.time.minute());
    display.setTextSize(1);
    display.printf("\n          %02d\n",gps.time.second());
  } else {
    display.setTextSize(2);
    display.print("--:--:--");
    display.setTextSize(1);
    display.printf("\n          --\n");
  }

  display.setTextSize(1);
  // Now show either searching or position
  if (gps.location.isValid()) {
    display.print("Speed: ");
    display.print(gps.speed.kmph());

  } else {
    display.println("Searching...");
  }

  display.display();

  // Optional: print to serial too
  if (gps.location.isUpdated()) {
    Serial.printf("Lat: %.6f  Lon: %.6f  Sats: %d  Time: %02d:%02d:%02d\n",
                  gps.location.lat(),
                  gps.location.lng(),
                  gps.satellites.value(),
                  gps.time.hour(),
                  gps.time.minute(),
                  gps.time.second());
  }

  delay(500);
}
