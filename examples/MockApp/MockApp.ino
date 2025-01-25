#include <Arduino.h>
#include <U8g2lib.h>
#include <pixelView.h>

#define JOY_X 8
#define JOY_Y 7
#define SEL 6

#define LEN(array) ((int)sizeof(array) / (int)sizeof((array)[0]))

static const unsigned char image_Lock_bits[] = {0x3c, 0x42, 0x42, 0xff, 0xff, 0xe7, 0xff, 0xff};
static const unsigned char image_Rpc_active_bits[] = {0x07, 0x35, 0x47, 0x40, 0x01, 0x71, 0x56, 0x70};
static const unsigned char image_Charging_lightning_mask_bits[] = {0x80, 0x00, 0x40, 0x00, 0x20, 0x00, 0x10,
                                                                   0x00, 0x78, 0x00, 0x3c, 0x00, 0x10, 0x00,
                                                                   0x08, 0x00, 0x04, 0x00, 0x02, 0x00};
static const unsigned char image_music_radio_streaming_bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x40, 0x00, 0x02, 0x80, 0x00, 0x12, 0x90, 0x00, 0x09,
    0x21, 0x01, 0xa5, 0x4b, 0x01, 0x95, 0x52, 0x01, 0xa5, 0x4b, 0x01, 0x09, 0x21, 0x01, 0x12, 0x90,
    0x00, 0x02, 0x80, 0x00, 0x04, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const unsigned char image_ButtonCenter_bits[] = {0x1c, 0x22, 0x5d, 0x5d, 0x5d, 0x22, 0x1c};
static const unsigned char image_car_bits[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0xe0, 0x0f, 0x00, 0x30, 0x11, 0x00, 0x18, 0x21, 0x00, 0xfe, 0xff, 0x03,
                                               0xef, 0xbf, 0x05, 0xd7, 0x5f, 0x07, 0xaa, 0xaf, 0x02, 0x10, 0x40, 0x00};

int sendInput() {
  int X = analogRead(JOY_X);
  int Y = analogRead(JOY_Y);

  // Assuming a center position of around 512 for both axes
  if (X < 10 && Y > 1750) {
    return ACTION_UP;
  } else if (X > 3900 && Y > 1750) {

    return ACTION_DOWN;
  } else if (X > 1750 && Y < 50) {

    return ACTION_LEFT;
  } else if (X > 1750 && Y > 3900) {

    return ACTION_RIGHT;
  }

  if (digitalRead(SEL) == LOW) return ACTION_SEL;

  return ACTION_NONE;
}


U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
PixelView pv(&u8g2, sendInput, [](int ms) { vTaskDelay(pdMS_TO_TICKS(ms)); }, u8g2_font_haxrcorp4089_tr);

void mockApp(PixelView *pv) {
  auto drawBar = [](U8G2 &disp) {
    disp.drawLine(0, 50, 127, 50);

    disp.setFont(u8g2_font_haxrcorp4089_tr);
    disp.drawStr(4, 61, "5:20");

    disp.drawXBM(117, 53, 9, 10, image_Charging_lightning_mask_bits);
    disp.drawXBM(106, 54, 7, 8, image_Rpc_active_bits);
  };

  auto homePage = [&drawBar](U8G2 disp, std::function<int(void)> input) {
    drawBar(disp);
    disp.drawStr(2, 11, "Hello there, this is a UI library ");
    disp.drawStr(2, 21, "that I've written from scratch");
    disp.drawStr(2, 31, "in C++. Everything you see is");
    disp.drawStr(2, 41, "made using this library");
    return PAGER_CONTINUE;
  };

  auto summary = [&drawBar](U8G2 disp, std::function<int(void)> input) {
    drawBar(disp);
    disp.drawBox(87, 31, 4, 18);
    disp.drawBox(82, 23, 4, 26);
    disp.drawBox(92, 25, 4, 24);
    disp.drawBox(97, 38, 4, 11);
    disp.drawBox(117, 31, 4, 18);
    disp.drawBox(122, 23, 4, 26);
    disp.drawBox(107, 38, 4, 11);
    disp.drawBox(102, 33, 4, 16);

    disp.drawLine(79, 0, 79, 50);
    disp.drawXBM(106, 1, 17, 16, image_music_radio_streaming_bits);
    disp.drawXBM(88, 5, 7, 7, image_ButtonCenter_bits);

    disp.setFont(u8g2_font_6x13_tr);
    disp.drawStr(15, 26, "Let Down");

    disp.setDrawColor(2);
    disp.drawRBox(13, 16, 51, 11, 0);
    disp.setDrawColor(1);

    disp.drawBox(112, 25, 4, 24);

    disp.setFont(u8g2_font_haxrcorp4089_tr);
    disp.drawStr(17, 36, "Radiohead");

    return PAGER_CONTINUE;
  };

  auto logs = [&](U8G2 disp, std::function<int(void)> input) {
    drawBar(disp);
    disp.setFont(u8g2_font_haxrcorp4089_tr);
    disp.drawStr(21, 27, "Press OK to view logs");

    if (input() == ACTION_SEL) {
      while (input() == ACTION_SEL) {
        vTaskDelay(50 / portTICK_PERIOD_MS);
      }
      String logs[] = {"Pushed to github at 4:22",
                       "Played radiohead songs :)",
                       "list brower",
                       "check boxes",
                       "LV -> moody lights",
                       "hello",
                       "booted up"};
      pv->listBrowser("Logs", NULL, logs, LEN(logs));
    }

    return PAGER_CONTINUE;
  };

  auto src = [&](U8G2 disp, std::function<int(void)> input) {
    drawBar(disp);
    disp.setFont(u8g2_font_haxrcorp4089_tr);
    disp.drawStr(2, 11, "All the source code is on");
    disp.drawStr(2, 21, "github");

    
    static int x;

    x += (input() == ACTION_SEL) ? 3 : 1;

    if (x > 132) x = -21;

    disp.setDrawColor(2);
    disp.drawXBM(x, 33, 19, 16, image_car_bits);

    disp.setFont(u8g2_font_helvR08_tr);
    disp.drawStr(33, 37, "Try pressing OK");

    disp.setDrawColor(1);

    return PAGER_CONTINUE;
  };

  auto exit = [&](U8G2 disp, std::function<int(void)> input) {
    drawBar(disp);
    disp.setFont(u8g2_font_haxrcorp4089_tr);
    disp.drawStr(29, 27, "Press OK to exit");

    if (input() == ACTION_SEL) {
      while (input() == ACTION_SEL) {
        vTaskDelay(50 / portTICK_PERIOD_MS);
      }

      return PAGER_EXIT;
    }

    return PAGER_CONTINUE;
  };

  // Ugly way of defining an array of the functions we made above
  std::function<int(U8G2, std::function<int(void)>)> functions[] = {homePage, summary, logs, src, exit};
  PixelView::Pager p(pv, LEN(functions), functions, PAGE_DOT_NAV);

  p.loop();
}


void setup() {
    pinMode(JOY_X, INPUT);
  pinMode(JOY_Y, INPUT);
  pinMode(SEL, INPUT_PULLUP);

  u8g2.begin();
  mockApp(&pv);
}

void loop() {
  pv.showMessage("You've exited from the paged navigation, this is the main loop");
}
