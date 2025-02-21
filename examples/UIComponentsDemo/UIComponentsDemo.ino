#include <Arduino.h>    // Not strictly needed, but it's good on platformIO based envs.
#include <U8g2lib.h>

#include <pixelView.h>

// Macro to calculate lenght of an array, through   sizeof(array) / sizeof(array[0])
#define LEN(array) ((int)sizeof(array) / (int)sizeof((array)[0]))

// Joystick pins. Not necessary but change according to your pinout
//    More info in `sendInput`
//
#define JOY_X 8
#define JOY_Y 7
#define BTN 6

// The function that gets called to manage input. This function right
// now is setup for a joystick and returns the ACTION_* macros depending
// on what is happening
//
// This function WILL change based on your usecase. (eg. adapted to 5 button navigation)
// The function must return one of the following values from the enum ActionType:
//    - ActionType::DOWN
//    - ActionType::UP
//    - ActionType::LEFT
//    - ActionType::RIGHT
//
//    - ActionType::SEL
//    - ActionType::NONE       <- No action is currently happening
//
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

// The U8G2 constructor. THIS WILL CHANGE BASED ON YOUR USECASE
// The library only works for FULL BUFFER MODE. This might change
// in the future. I'll work on adding support for page buffer.
//
// Find your 128x64 display here: https://github.com/olikraus/u8g2/wiki/u8g2setupcpp
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

// The PixelView constructor. The only thing that will change is the doDelay parameter
// In this case it is adapted to native Arduino Code. Therefore delay is passed as the
// parameter.
//
// This will change, for example, on the ESP32 (where you are on FreeRTOS) use vTaskDelay.
// This is what it would look like:
// Note doDelay is a lambda function
//
// You can also specify a font parameter. by default a 6x12 font is used.
PixelView pixelview(&u8g2, sendInput, delay, u8g2_font_haxrcorp4089_tr);

PixelView::Keyboard kbd(pixelview);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Just a wrapper around gridMenu function with all the bitmaps.
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
  pixelview.gridMenu(icon_allArray, icon_allArray_LEN);
}
// End of gridMenu
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// The main setup function
void setup() {
  Serial.begin(115200); // Start Serial communication
  u8g2.begin();         // Init. the display

  // Initialize Pins for the Joystick
  // This will change based on your implementation of input
  //
  pinMode(JOY_X, INPUT);
  pinMode(JOY_Y, INPUT);
  pinMode(BTN, INPUT_PULLUP);
}

// ICONS
// You can skip past this section it is just a
// bunch of bitmaps.

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// A list of menu items
// In this case it is just every (maybe) UI component and their respective icons
//
PixelView::menuItem m[] = {{"Keyboard", bmp_keyboard}, {"Pagination", bmp_pagination}, {"Sub Menu", bmp_submenu},
                           {"Grid Menu", bmp_grid},    {"Radio Buttons", bmp_radio},   {"Check Boxes", bmp_checkbox},
                           {"List Browser", bmp_list}, {"Progress Bar", bmp_progress}, {"Dialog Boxes", bmp_warning}};
void loop() {

  // Do not let any stray input through
  while (sendInput() != ActionType::NONE)
    ;

  // Call the menu function for the UI elements declared above
  //
  // Returns the `menuItem` that was selected
  static int choice;
  choice = pixelview.menu(m, LEN(m), choice);

  // Flush away any stray input
  while (sendInput() != ActionType::NONE)
    ;

  if (choice == 0) { // KEYBOARD
    const char *types[] = {"Keyboard", "Numpad", "Back"};

    // Keep doing this till "Back" option is selected
    bool exit = false;
    while (!exit) {
      int choice = pixelview.subMenu("Choose Keyboard type", types, 3);

      if (choice == 0)
        String s = kbd.fullKeyboard(
            "Enter text", false, "def. value"); // A function that displays a keyboard, and returns the typed out String
                                                // Default values can be provided. In this case it is "def. value"

      else if (choice == 1)
        String s = kbd.numPad("11", false); // Like `fullKeyboard` but just has numbers, useful for stuff like getting
                                            // phone numbers or IP addresses

      // Exit the loop
      else if (choice == 2) exit = true;
    }

  } else if (choice == 1) { //  PAGINATION

    // Local functions for different "pages" in the paged navigation
    // Each function MUST return one of the following:
    //    - PAGER_CONTINUE
    //    - PAGER_EXIT
    //    - PAGER_TOGGLE_NAV
    //    - PAGER_DISABLE_NAV
    //    - PAGER_ENABLE_NAV
    //
    auto f1 = [](U8G2 *disp, PixelView::InputFuncType) {
      disp->setFont(u8g2_font_haxrcorp4089_tr);
      disp->drawStr(0, 10, "This is page One!");

      return PixelView::Pager::PagerActionType::CONTINUE;
    };

    auto f2 = [](U8G2 *disp, PixelView::InputFuncType) {
      disp->setFont(u8g2_font_haxrcorp4089_tr);
      disp->drawStr(0, 10, "This is page Two");

      return PixelView::Pager::PagerActionType::CONTINUE;
    };

    auto f3 = [](U8G2 *disp, PixelView::InputFuncType ip) {
      disp->setFont(u8g2_font_haxrcorp4089_tr);
      disp->drawStr(0, 10, "Press OK to exit");

      if (ip() == ActionType::SEL) {
        return PixelView::Pager::PagerActionType::EXIT;
      }

      return PixelView::Pager::PagerActionType::CONTINUE;
    };

    // Ugly way of defining an array of the functions we made above
    PixelView::Pager::PageFuncType functions[] = {f1, f2, f3};
    PixelView::Pager p(&pixelview, LEN(functions), functions, PixelView::Pager::IndicatorType::DOT);

    p.loop();

  } else if (choice == 2) { // SUBMENU

    const char *types[] = {"Option 1", "Option 2", "Back"};
    int choice = pixelview.subMenu("Submenu Demo", types, 3);

  } else if (choice == 3) { // GRID MENU
    gridMenu();

  } else if (choice == 4) { // RADIO BUTTONS

    const char *options[] = {"option", "Another option", "yeet", "yank n paste"};
    pixelview.radioSelect("Radio Button Demo", options, LEN(options));

  } else if (choice == 5) { // CHECK BOXES

    PixelView::checkBox c[] = {{"Option 1"}, {"press & hold ok to exit"}, {"Third choice"}, {"hhmmm"}, {"dark mode"}};

    pixelview.checkBoxes("Check box demo", c, LEN(c));

  } else if (choice == 6) {  // LIST BROWSER

    const char *options[] = {"a lot of text", "0xFF", "0x21", "www.google.com", "github.com"};

    pixelview.listBrowser("List Browser Demo", bmp_list, options, LEN(options), PixelView::ListType::NUMBER);
  } else if (choice == 7) {   // PROGRESS BAR

    int i = 0;
    while (sendInput() != ActionType::SEL) {
      pixelview.progressBar(i, "Progress Bar demo");
      i++;
      if (i > 100) {
        i = 0;
      }
      delay(30);
    }
  } else if (choice == 8) {  // DIALOG BOX

    bool exit = false;
    while (!exit) {

      while (sendInput() != ActionType::NONE)
        ;
      const char *types[] = {"Yes/No Dialog", "Ok Dialog", "Back"};
      int choice = pixelview.subMenu("Dialog Demo", types, LEN(types));

      if (choice == 0) pixelview.confirmYN("Test Confirm", true);
      else if (choice == 1) pixelview.showMessage("Test Message");
      else if (choice == 2) exit = true;
    }
  }
}

