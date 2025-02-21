#include <Arduino.h>
#include <U8g2lib.h>
#include <WiFi.h>
#include <pixelView.h>

// Joystick pins. Not necessary but change according to your pinout
//    More info in `sendInput`
//
#define JOY_X 8
#define JOY_Y 7
#define BTN 6

const unsigned char bmpWiFi_4Bars[] PROGMEM = {0x00, 0x00, 0x00, 0x00, 0xc0, 0x03, 0xf8, 0x1f, 0xfe, 0x7f, 0xff,
                                               0xff, 0xfe, 0x7f, 0xfc, 0x3f, 0xf8, 0x1f, 0xf0, 0x0f, 0xe0, 0x07,
                                               0xc0, 0x03, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const unsigned char bmpWiFi_3Bars[] PROGMEM = {0x00, 0x00, 0x00, 0x00, 0xc0, 0x03, 0xf8, 0x1f, 0x0e, 0x70, 0x03,
                                               0xc0, 0x06, 0x60, 0xec, 0x37, 0xf8, 0x1f, 0xf0, 0x0f, 0xe0, 0x07,
                                               0xc0, 0x03, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const unsigned char bmpWiFi_2Bars[] PROGMEM = {0x00, 0x00, 0x00, 0x00, 0xc0, 0x03, 0xf8, 0x1f, 0x0e, 0x70, 0x03,
                                               0xc0, 0x06, 0x60, 0x0c, 0x30, 0xd8, 0x1b, 0xf0, 0x0f, 0xe0, 0x07,
                                               0xc0, 0x03, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const unsigned char bmpWiFi_1Bars[] PROGMEM = {0x00, 0x00, 0x00, 0x00, 0xc0, 0x03, 0xf8, 0x1f, 0x0e, 0x70, 0x03,
                                               0xc0, 0x06, 0x60, 0x0c, 0x30, 0x18, 0x18, 0xb0, 0x0d, 0xe0, 0x07,
                                               0xc0, 0x03, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const unsigned char bmpWiFi_0Bars[] PROGMEM = {0x00, 0x00, 0x00, 0x00, 0xc0, 0x03, 0xf8, 0x1f, 0x0e, 0x70, 0x03,
                                               0xc0, 0x06, 0x60, 0x0c, 0x30, 0x18, 0x18, 0x30, 0x0c, 0x60, 0x06,
                                               0xc0, 0x03, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const unsigned char bmpArrowBack[] PROGMEM = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x40,
                                              0x00, 0x20, 0x00, 0xf0, 0x1f, 0xf0, 0x1f, 0x20, 0x00, 0x40, 0x00,
                                              0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const unsigned char bmpLock[] PROGMEM = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0xfc, 0x00, 0x86,
                                         0x01, 0x3b, 0xff, 0x69, 0xc0, 0x69, 0xc0, 0x3b, 0xef, 0x86, 0x69,
                                         0xfc, 0x78, 0x30, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

ActionType sendInput() {
  int X = analogRead(JOY_X);
  int Y = analogRead(JOY_Y);

  if (X < 10 && Y > 1750) {
    return ActionType::UP;
  } else if (X > 3900 && Y > 1750) {
    return ActionType::DOWN;
  } else if (X > 1750 && Y < 50) {
    return ActionType::LEFT;
  } else if (X > 1750 && Y > 3900) {
    return ActionType::RIGHT;
  }

  if (digitalRead(BTN) == LOW) return ActionType::SEL;

  return ActionType::NONE;
}
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
PixelView pixelview(&u8g2, sendInput, delay, u8g2_font_haxrcorp4089_tr);
PixelView::Keyboard keyboard(pixelview);

void wifiScanner(PixelView *pv, PixelView::Keyboard *kyb, U8G2 *u8g2) {
  WiFi.mode(WIFI_STA);

  // There's a bug with the WiFi library on the ESP32 where Async scan errors out... if it is fixed we can use progressCircle() while waiting for the scan to finish :)

  u8g2->clearBuffer();
  u8g2->setFont(u8g2_font_haxrcorp4089_tr);
  u8g2->drawStr(0, 10, "Scanning Wi-Fi networks...");
  u8g2->sendBuffer();

  const int n = WiFi.scanNetworks();

  if (n == 0) {
    Serial.println("No networks found");
    return;
  }

  // PixelView::menuItem *items = (PixelView::menuItem *)calloc(n + 2, sizeof(PixelView::menuItem));
  PixelView::menuItem *items = new PixelView::menuItem[n + 2]();

  for (int i = 0; i < n; i++) {
    items[i].name = WiFi.SSID(i);
    int rssi = WiFi.RSSI(i);
    if (rssi > -70) items[i].icon = bmpWiFi_4Bars;
    else if (rssi > -80) items[i].icon = bmpWiFi_3Bars;
    else if (rssi > -90) items[i].icon = bmpWiFi_2Bars;
    else items[i].icon = bmpWiFi_1Bars;
  }

  items[n] = {.name = "Exit", .icon = bmpArrowBack};
  items[n + 1] = {.name = "Add hidden", .icon = bmpLock};

  const int choice = pv->menu(items, n + 2);

  String ssid;
  String psk;

  if (choice == n) return;
  else if (choice == n + 1) ssid = kyb->fullKeyboard("Enter the SSID");
  else ssid = items[choice].name;

  psk = kyb->fullKeyboard("Enter the password for\n " + ssid);
#define TIMEOUT 60000

  WiFi.begin(ssid, psk);

  int beginMS = millis();
  byte b = 0;
  while (WiFi.status() != WL_CONNECTED && millis() - beginMS < TIMEOUT) {
    pv->progressCircle(b);
    b++;
    vTaskDelay(70 / portTICK_PERIOD_MS);
  }

  if (WiFi.status() == WL_CONNECTED) pv->showMessage(("Connected successfully to: " + ssid).c_str());
  else pv->showMessage("Failed to connect");
}

void setup() {
  u8g2.begin();

  // Initialize Pins for the Joystick
  // This will change based on your implementation of input
  //
  pinMode(JOY_X, INPUT);
  pinMode(JOY_Y, INPUT);
  pinMode(BTN, INPUT_PULLUP);

  pixelview.showMessage("Setup finished, starting Wi-Fi scanner");
  wifiScanner(&pixelview, &keyboard, &u8g2);
}
void loop() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_haxrcorp4089_tr);
  u8g2.drawStr(0, 10, ("Connected to: " + WiFi.SSID()).c_str());
  u8g2.drawStr(0, 21, String(millis()).c_str());
  u8g2.sendBuffer();
}
