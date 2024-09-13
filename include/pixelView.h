#pragma once
#include "actions.h"
#include <Arduino.h>
#include <U8g2lib.h>

using std::function;

#define PAGE_DOT_NAV 1
#define PAGE_NUM_NAV 2
#define PAGE_NUM_AND_ARROW_NAV 3
#define PAGE_ARROW_NAV 4

/* IDEAS:
 *  - Tabbed navigations
 *  - Sliders
 *  - Progress bar
 *  -
 *
 * */

/**
 * @class PixelView
 * @brief A simple GUI library for *128x64 OLEDs*
 *
 */
class PixelView {
private:
  /**
   * @brief Pointer to a U8G2 object
   */
  static U8G2 *u8g2;

  static function<int(void)> doInput;
  static function<void(int32_t)> doDelay;

  const uint8_t *font;

public:
  /**
   * @brief The constructor
   *
   * @param display is a pointer to an object of type U8G2
   * @param inputFunction A function that returns any ACTION
   * @param delayer A function that delays for n milliseconds
   *
   * @returns void
   */
  PixelView(U8G2 *display, function<int(void)> inputFunction, function<void(int)> delayer,
            const uint8_t font[] = u8g2_font_6x12_tr);

  /**
   * @brief  Renders text with word wrapping enabled.
   *
   * @param xloc The X location of the text
   * @param yloc The Y location of the text
   * @param text The text to be rendered
   *
   * @note Ensure that the font is loaded before calling this function.
   */
  void wordWrap(int xloc, int yloc, const char *text);

  /// @brief Shows a dialog box with two buttons: Yes and No
  ///
  /// @param message displayed on the screen before the buttons
  /// @param defaultOption Default choice. False -> No; True -> Yes
  ///
  /// @return The selected option     False -> No; True -> Yes
  bool confirmYN(const char *message = "Confirm?", bool defaultOption = false);

  /////////////////////////////////////////////////////////////////////////////////////////
  /// @brief Shows a dialog with one button: Yes
  ///
  /// @param message Displayed on the screen before the button
  void showMessage(const char *message);

  /**
   * @class Keyboard
   * @brief A (gboard-like) keyboard for OLEDs
   *
   *  +-----------------------------+
   *  |                             |
   *  |--|--|--|--|--|--|--|--|--|--|
   *  |q |w |e |r |t |y |u |i |o |p |
   *  |--|--|--|--|--|--|--|--|--|--|
   *  |--|--|--|--|--|--|--|--|--|--|
   *  |--|--|--|--|--|--|--|--|--|--|
   *  |--|--|--|--|--|--|--|--|--|--|
   *  |--|--|--|--|--|--|--|--|--|--|
   *  +-----------------------------+
   */
  class Keyboard {
  public:
    Keyboard(PixelView &pixelView); // Constructor

    /**
     * @brief Renders the keyboard on the display itself.
     *
     * @param message If this parameter is not empty, PixelView::showMessage is called with message.
     * @param isEmptyAllowed If set to true, the keyboard doesn't allow an empty input
     * @param defaultText Default text that is displayed on the keyboard. It must be cleared manually to
     * @return
     */
    String openKeyboard(const String &message = "", bool isEmptyAllowed = false, const String &defaultText = "");

  private:
    void renderKeyboard(int pX, int pY, const String &text);

    const char *letters[4][10] = {{"q", "w", "e", "r", "t", "y", "u", "i", "o", "p"},
                                  {"a", "s", "d", "f", "g", "h", "j", "k", "l", "<ques>"},
                                  {"<caps>", "z", "x", "c", "v", "b", "n", "m", "<caps>", "<rm>"},
                                  {"<sym1>", " ", " ", " ", " ", "<left>", "<right>", "<rev>", "<clr>", "<ok>"}};

    const char *capitalLetters[4][10] = {{"Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P"},
                                         {"A", "S", "D", "F", "G", "H", "J", "K", "L", "<ques>"},
                                         {"<caps>", "Z", "X", "C", "V", "B", "N", "M", "<caps>", "<rm>"},
                                         {"<sym1>", " ", " ", " ", " ", "<left>", "<right>", "<rev>", "<clr>", "<ok>"}};

    const char *symbols1[4][10] = {{"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"},
                                   {"!", "@", "#", "$", "%", "^", "&", "*", "(", ")"},
                                   {"-", "=", "_", "+", "/", ":", ";", "?", "'", "<rm>"},
                                   {"<let>", "<sym2>", ".", ",", " ", " ", "", "<rev>", "<clr>", "<ok>"}};

    const char *symbols2[4][10] = {{"<", ">", "\\", "|", "[", "]", "{", "}", " ", " "},
                                   {" ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                                   {" ", " ", " ", " ", " ", " ", " ", " ", " ", "<rm>"},
                                   {"<let>", "<sym1>", " ", " ", " ", " ", " ", " ", " ", " "}};

    const char *(*currentLayer)[10] = letters;

    bool caps;
    int insertIdx;
    PixelView &p;
  };

  /**
   * @class Pager
   * @brief Creates a paged view where the user can cycle between different "pages"
   *
   * +-------------------------------+    +-------------------------------+
   * |  Page 1                       |    |  Page 2                       |
   * |                               |    |                               |
   * |                               |    |                               |
   * |                               |    |                               |
   * |             ●○○○              |    |             ○●○○              |
   * +-------------------------------+    +-------------------------------+
   *
   */
  class Pager {
  private:
    std::function<void(U8G2, std::function<int(void)>)> *displayFunctions;
    int numFuncs;
    int index = 0;

  public:
    int indicator;
    /**
     * @constructor
     *
     * @param numFuncs The number of functions in the functions array
     * @param displayFunctions An array of functions that render pages.
     *                         Note that each function needs to return 0 by default
     *                         TODO: Add a thing to capture input
     */
    Pager(int numFuncs, std::function<void(U8G2, std::function<int(void)>)> *displayFunctions,
          const int indicatorType = PAGE_DOT_NAV);

    // void changeIndicatorType(const int indicatorType);

    /**
     * @brief Render the frame and manage input
     */
    void render();
  };

  struct menuItem {
    String name;
    const unsigned char *icon;
  };

  /**
   * @brief A menu system that shows 3 items at once. The center one is selected. Each option has a specific 16x16 Icon.
   *        Also a scroll handle is displayed on the far right
   *
   * @param items An array of type meuItems
   * @param numItems The number of items in `items`
   * @return the selected menuItem
   */
  menuItem menu(menuItem items[], unsigned int numItems);
  /**
   * @brief Similar to `menu` but does not have icons and also has a header
   *
   * @param header The top header above the menu
   * @param items An array of char*. Used as the items to populate the array
   * @param numItems The number of items
   * @return The selected option
   */
  const char *subMenu(const char *header, const char *items[], unsigned int numItems);
};
