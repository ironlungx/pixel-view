#include <Arduino.h>
#include <U8g2lib.h>

#include <pixelView.h>

#define FORMAT_LITTLEFS_IF_FAILED true

#define JOY_Y 34
#define JOY_X 35
#define SEL 22

#define LEN(array) ((int)sizeof(array) / (int)sizeof((array)[0]))

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

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE,
                                        /* clock=*/33, /* data=*/25);
PixelView pi(&u8g2, sendInput, [](int ms) { vTaskDelay(pdMS_TO_TICKS(ms)); });

// U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

void f1(U8G2 u8g2, std::function<int(void)> doInput) {
  u8g2.setFont(u8g2_font_helvR08_tr);

  u8g2.drawStr(0, 12, "Hello World!");
}

void f2(U8G2 u8g2, std::function<int(void)> doInput) {
  u8g2.setFont(u8g2_font_helvR08_tr);
  u8g2.drawStr(0, 12, String(millis() / 1000).c_str());
}

void f3(U8G2 u8g2, std::function<int(void)> doInput);
void f4(U8G2 u8g2, std::function<int(void)> doInput);
std::function<void(U8G2, std::function<int(void)>)> functions[] = {f1, f2, f3, f4};
PixelView::Pager p(LEN(functions), functions, PAGE_DOT_NAV);
bool exitPager = false;

void f3(U8G2 u8g2, std::function<int(void)> doInput) {
  String buf = "Nav. Type:   ";
  switch (p.indicator) {
  case PAGE_NUM_NAV:
    buf += "Number";
    break;
  case PAGE_ARROW_NAV:
    buf += "Arrow";
    break;
  case PAGE_NUM_AND_ARROW_NAV:
    buf += "Arrow + Number";
    break;
  case PAGE_DOT_NAV:
    buf += "Dot";
    break;
  }

  u8g2.setFont(u8g2_font_haxrcorp4089_tr);
  u8g2.drawStr(0, 12, buf.c_str());
  u8g2.drawStr(0, 24, "Press OK to change");

  if (doInput() == ACTION_SEL) {
    const char *navTypes[] = {"Dot", "Number with arrow", "Number", "Arrow"};
    while ((doInput() == ACTION_SEL))
      ;

    // pi.menu(i, 3);
    const char *option = pi.radioSelect("Choose a nav. type", navTypes, 4);

    if (strcmp(option, navTypes[0]) == 0) p.indicator = PAGE_DOT_NAV;

    else if (strcmp(option, navTypes[1]) == 0) p.indicator = PAGE_NUM_AND_ARROW_NAV;

    else if (strcmp(option, navTypes[2]) == 0) p.indicator = PAGE_NUM_NAV;

    else if (strcmp(option, navTypes[3]) == 0) p.indicator = PAGE_ARROW_NAV;

    while ((doInput() == ACTION_SEL))
      ;
  }
}

void f4(U8G2 u8g2, std::function<int(void)> doInput) {

  u8g2.setFont(u8g2_font_helvR08_tr);
  u8g2.drawStr(0, 12, "Press SELECT to exit");

  if (doInput() == ACTION_SEL) {
    exitPager = true;

    while ((doInput() == ACTION_SEL))
      ;
  }
}

void gridMenu() {
  // 'Battery_16x16', 16x16px
  const unsigned char icon_Battery_16x16[] PROGMEM = {0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0xc0, 0x03, 0x20, 0x04, 0x20,
                                                      0x04, 0xa0, 0x05, 0x20, 0x04, 0xa0, 0x05, 0x20, 0x04, 0xa0, 0x05,
                                                      0x20, 0x04, 0xc0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  // 'choice_right_16x16', 16x16px
  const unsigned char icon_choice_right_16x16[] PROGMEM = {
      0xc0, 0x03, 0x30, 0x0c, 0x88, 0x11, 0x64, 0x26, 0x12, 0x48, 0x0a, 0x50, 0x09, 0x94, 0x25, 0xa2,
      0x45, 0xa1, 0x89, 0x90, 0x0a, 0x50, 0x12, 0x48, 0x64, 0x26, 0x88, 0x11, 0x30, 0x0c, 0xc0, 0x03};
  // 'file_edit_16x16', 16x16px
  const unsigned char icon_file_edit_16x16[] PROGMEM = {
      0x00, 0x10, 0x00, 0x28, 0x00, 0x44, 0x00, 0x8a, 0x00, 0x51, 0x80, 0x20, 0x40, 0x10, 0x20, 0x08,
      0x10, 0x04, 0x08, 0x02, 0x04, 0x01, 0x82, 0x00, 0x41, 0x00, 0x21, 0x00, 0x11, 0x00, 0x0f, 0x00};
  // 'file_movie_16x16', 16x16px
  const unsigned char icon_file_movie_16x16[] PROGMEM = {
      0x00, 0x38, 0x00, 0x73, 0x70, 0x66, 0xce, 0x0c, 0x9a, 0x01, 0x0f, 0x00, 0x3f, 0xb3, 0x9a, 0x99,
      0xce, 0xcc, 0x02, 0x80, 0x02, 0x80, 0xf2, 0x9f, 0x02, 0x80, 0xf2, 0x9f, 0x02, 0x80, 0xfe, 0xff};
  // 'file_print_16x16', 16x16px
  const unsigned char icon_file_print_16x16[] PROGMEM = {
      0xf8, 0x1f, 0x08, 0x10, 0x08, 0x10, 0x0c, 0x30, 0x0c, 0x30, 0xfe, 0x7f, 0x01, 0x80, 0x05, 0x80,
      0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0xfd, 0xbf, 0x0d, 0xb0, 0x0e, 0x70, 0x08, 0x10, 0xf8, 0x1f};
  // 'file_save_16x16', 16x16px
  const unsigned char icon_file_save_16x16[] PROGMEM = {
      0xfe, 0x3f, 0x09, 0x55, 0x09, 0x95, 0x09, 0x97, 0x09, 0x90, 0xf1, 0x8f, 0x01, 0x80, 0x01, 0x80,
      0x01, 0x80, 0xf9, 0x9f, 0x09, 0x90, 0xe9, 0x97, 0x09, 0x90, 0xeb, 0xd7, 0x09, 0x90, 0xfe, 0x7f};
  // 'file_save_flipped_16x16', 16x16px
  const unsigned char icon_file_save_flipped_16x16[] PROGMEM = {
      0xfe, 0x7f, 0x09, 0x90, 0xeb, 0xd7, 0x09, 0x90, 0xe9, 0x97, 0x09, 0x90, 0xf9, 0x9f, 0x01, 0x80,
      0x01, 0x80, 0x01, 0x80, 0xf1, 0x8f, 0x09, 0x90, 0xe9, 0x90, 0xa9, 0x90, 0xaa, 0x90, 0xfc, 0x7f};
  // 'flag_16x16', 16x16px
  const unsigned char icon_flag_16x16[] PROGMEM = {0x00, 0x07, 0xc1, 0x08, 0x33, 0x10, 0x0d, 0xe0, 0x01, 0x80, 0x01,
                                                   0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80,
                                                   0x01, 0x87, 0xc1, 0x88, 0x32, 0xf0, 0x0c, 0x00, 0x00, 0x00};
  // 'flower_16x16', 16x16px
  const unsigned char icon_flower_16x16[] PROGMEM = {0x00, 0x38, 0x00, 0x7c, 0x00, 0x4f, 0x0c, 0xef, 0x3e, 0xfe, 0x7f,
                                                     0x3e, 0x73, 0x3d, 0x3f, 0x01, 0x5e, 0xf2, 0x4c, 0xca, 0xb0, 0x44,
                                                     0xb0, 0x07, 0x08, 0x03, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02};

  // Array of all bitmaps for convenience. (Total bytes used to store images in
  // PROGMEM = 432)
  const int icon_allArray_LEN = 9;
  const unsigned char *icon_allArray[9] = {icon_Battery_16x16,           icon_choice_right_16x16, icon_file_edit_16x16,
                                           icon_file_movie_16x16,        icon_file_print_16x16,   icon_file_save_16x16,
                                           icon_file_save_flipped_16x16, icon_flag_16x16,         icon_flower_16x16};
  pi.gridMenu(icon_allArray, icon_allArray_LEN);
}

void setup() {
  Serial.begin(115200);
  u8g2.begin();
  pinMode(JOY_X, INPUT);
  pinMode(JOY_Y, INPUT);
  pinMode(SEL, INPUT_PULLUP);

  // pi.progressCircle();
  // while (true)
    //;
}

const unsigned char bmp_keyboard[] PROGMEM = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x7f, 0x02,
                                              0x80, 0xaa, 0xaa, 0x02, 0x80, 0x02, 0x80, 0xea, 0xab, 0x02, 0x80,
                                              0xfc, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const unsigned char bmp_pagination[] PROGMEM = {
    // 'pages, 16x16px
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x7f, 0x02, 0x40, 0xc2, 0x41, 0x42, 0x42, 0xc2, 0x41,
    0x42, 0x40, 0x42, 0x40, 0x02, 0x40, 0xaa, 0x55, 0x02, 0x40, 0xfe, 0x7f, 0x00, 0x00, 0x00, 0x00};

const unsigned char bmp_submenu[] PROGMEM = {
    // 'submenu' 16x16
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x1f, 0x00, 0x00, 0xf8, 0x1f,
    0x00, 0x00, 0xf8, 0x1f, 0x00, 0x00, 0xf8, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

const unsigned char bmp_grid[] PROGMEM = {
    // 'grid' 16x16
    0x00, 0x00, 0x7e, 0x7e, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x7e, 0x7e, 0x00, 0x00,
    0x00, 0x00, 0x7e, 0x7e, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x7e, 0x7e, 0x00, 0x00};

const unsigned char bmp_radio[] PROGMEM = {
    // 'radio' 16x16
    0xf0, 0x0f, 0x08, 0x10, 0xe4, 0x27, 0xf2, 0x4f, 0xf9, 0x9f, 0xfd, 0xbf, 0xfd, 0xbf, 0xfd, 0xbf,
    0xfd, 0xbf, 0xfd, 0xbf, 0xfd, 0xbf, 0xf9, 0x9f, 0xf2, 0x4f, 0xe4, 0x27, 0x08, 0x10, 0xf0, 0x0f};

const unsigned char bmp_checkbox[] PROGMEM = {
    // 'checkbox', 16x16px
    0x00, 0x00, 0xfe, 0x7f, 0x02, 0x40, 0xfa, 0x5f, 0xfa, 0x5f, 0xfa, 0x5f, 0xfa, 0x5f, 0xfa, 0x5f,
    0xfa, 0x5f, 0xfa, 0x5f, 0xfa, 0x5f, 0xfa, 0x5f, 0xfa, 0x5f, 0x02, 0x40, 0xfe, 0x7f, 0x00, 0x00};

const unsigned char bmp_list[] PROGMEM = {
    // 'list', 16x16px
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf4, 0x0f, 0x00, 0x00, 0x00, 0x00, 0xf4, 0x01, 0x00, 0x00,
    0x00, 0x00, 0xf4, 0x07, 0x00, 0x00, 0x00, 0x00, 0xf4, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

const unsigned char bmp_progress[] PROGMEM = {
    // 'progress', 16x16px
    0x00, 0x00, 0x38, 0x00, 0x7c, 0x30, 0x7c, 0x38, 0x7c, 0x1c, 0x38, 0x0e, 0x00, 0x07, 0x80, 0x03,
    0xc0, 0x01, 0xe0, 0x38, 0x70, 0x7c, 0x38, 0x7c, 0x1c, 0x7c, 0x0c, 0x38, 0x00, 0x00, 0x00, 0x00};

const unsigned char bmp_warning[] PROGMEM = {
    // 'warning' 16x16
    0x00, 0x00, 0x80, 0x01, 0x40, 0x02, 0x40, 0x02, 0x20, 0x04, 0x90, 0x09, 0x90, 0x09, 0x88, 0x11,
    0x88, 0x11, 0x84, 0x21, 0x02, 0x40, 0x82, 0x41, 0x81, 0x81, 0x01, 0x80, 0xfe, 0x7f, 0x00, 0x00};

PixelView::menuItem m[] = {{"Keyboard", bmp_keyboard}, {"Pagination", bmp_pagination}, {"Sub Menu", bmp_submenu},
                           {"Grid Menu", bmp_grid},    {"Radio Buttons", bmp_radio},   {"Check Boxes", bmp_checkbox},
                           {"List Browser", bmp_list}, {"Progress Bar", bmp_progress}, {"Dialog Boxes", bmp_warning}};
void loop() {

  while (sendInput() != ACTION_NONE)
    ;

  PixelView::menuItem choice = pi.menu(m, LEN(m));

  while (sendInput() != ACTION_NONE)
    ;

  if (choice.name == "Keyboard") {
    const char *types[] = {"Keyboard", "Numpad", "Back"};

    bool exit = false;
    while (!exit) {
      const char *c = pi.subMenu("Choose Keyboard type", types, 3);

      PixelView::Keyboard kbd(pi);

      if (strcmp(c, "Keyboard") == 0) String s = kbd.fullKeyboard("Enter text", false, "def. value");

      else if (strcmp(c, "Numpad") == 0) String s = kbd.numPad("11", false);

      else if (strcmp(c, "Back") == 0) exit = true;
    }

  } else if (choice.name == "Pagination") {

    while (!exitPager)
      p.render();

    exitPager = false;

  } else if (choice.name == "Sub Menu") {

    const char *types[] = {"Option 1", "Option 2", "Back"};
    const char *c = pi.subMenu("Submenu Demo", types, 3);

  } else if (choice.name == "Grid Menu") {
    gridMenu();

  } else if (choice.name == "Radio Buttons") {

    const char *options[] = {"option", "Another option", "yeet", "yank n paste"};
    pi.radioSelect("Radio Button Demo", options, LEN(options));

  } else if (choice.name == "Progress Bar") {

    int i = 0;
    while (sendInput() != ACTION_SEL) {
      pi.progressBar(i, "Progress Bar demo");
      i++;
      if (i > 100) {
        i = 0;
      }
      delay(30);
    }

  } else if (choice.name == "Check Boxes") {

    PixelView::checkBox c[] = {{"Option 1"}, {"press & hold ok to exit"}, {"Third choice"}, {"hhmmm"}, {"dark mode"}};

    pi.checkBoxes("Check box demo", c, LEN(c));

  } else if (strcmp(choice.name.c_str(), "List Browser") == 0) {

    const char *options[] = {"a lot of text", "0xFF", "0x21", "www.google.com", "github.com"};

    pi.listBrowser("List Browser Demo", bmp_list, options, LEN(options), LIST_BULLET_POINT, u8g2_font_haxrcorp4089_tr);

  } else if (strcmp(choice.name.c_str(), "Dialog Boxes") == 0) {

    bool exit = false;
    while (!exit) {

      while (sendInput() != ACTION_NONE)
        ;
      const char *types[] = {"Yes/No Dialog", "Ok Dialog", "Back"};
      const char *c = pi.subMenu("Dialog Demo", types, LEN(types));

      if (strcmp(c, "Yes/No Dialog") == 0) pi.confirmYN("Test Confirm", true);
      else if (strcmp(c, "Ok Dialog") == 0) pi.showMessage("Test Message");
      else if (strcmp(c, "Back") == 0) exit = true;
    }
  }
}
