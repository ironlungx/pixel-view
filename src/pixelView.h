#pragma once

#include "actions.h"
#include <Arduino.h>
#include <U8g2lib.h>

// Pager Navigation types
#define PAGE_DOT_NAV 1
#define PAGE_NUM_NAV 2
#define PAGE_NUM_AND_ARROW_NAV 3
#define PAGE_ARROW_NAV 4
#define PAGE_NONE_NAV 5

/* IDEAS:
 *    *None* as of now
 *
 *
 * */

/**
 * @class PixelView
 * @brief A simple GUI library for *128x64 OLEDs*
 *
 */
class PixelView {
public:
  typedef std::function<ActionType(void)> InputFuncType;

private:
  /**
   * @brief Pointer to a U8G2 object
   */
  void search(const char *items[], unsigned int numItems, const char *query, const char *result[],
              unsigned int *resultCount, unsigned int resultIndices[], bool caseSensitive = true);

  static U8G2 *u8g2;

  static InputFuncType doInput;
  static std::function<void(int32_t)> doDelay;

  const uint8_t *font;

public:
  /**
   * @brief The constructor
   *
   * @param display is a pointer to an object of type U8G2
   * @param inputFunction A function that returns any ActionType
   * @param delayer A function that delays for n milliseconds
   *
   * @returns void
   */
  PixelView(U8G2 *display, std::function<ActionType(void)> inputFunction, std::function<void(int)> delayer,
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
  void wordWrap(int xloc, int yloc, const char *text, bool maintainX = false);

  /**
   * @breif Renders text with a rounded box behind it. Takes into account Font size and stuff
   *
   * @param x The X location of text
   * @param y The Y location of text
   * @param text The text to be rendered
   * @param font The font :)
   */
  void accentText(int x, int y, const char *text, const uint8_t font[]);

  /**
   * @brief Shows a dialog box with two buttons: Yes and No
   * @param message displayed on the screen before the buttons
   * @param defaultOption Default choice. False -> No; True -> Yes

   * @return The selected option     False -> No; True -> Yes
   */
  bool confirmYN(const char *message = "Confirm?", bool defaultOption = false);

  /**
   * @brief Shows a dialog with one button: Yes
   * @param message Displayed on the screen before the button
   */
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
    String fullKeyboard(const String &message = "", bool isEmptyAllowed = false, const String &defaultText = "");
    String numPad(const String message = "", bool isEmptyAllowed = false, const char *defaultText = "");

  private:
    void renderKeyboard(int pX, int pY, const String &text);

    const char *numpad[4][3] = {{"1", "2", "3"}, {"4", "5", "6"}, {"7", "8", "9"}, {"\u0087", "0", "<"}};

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
   * +-------------------------------+    +-------------------------------+
   * |  Page 1                       |    |  Page 2                       |
   * |                               |    |                               |
   * |                               |    |                               |
   * |                               |    |                               |
   * |            <  >               |    |             < >               |
   * +-------------------------------+    +-------------------------------+
   *
   * +-------------------------------+    +-------------------------------+
   * |  Page 1                       |    |  Page 2                       |
   * |                               |    |                               |
   * |                               |    |                               |
   * |                               |    |                               |
   * |          < 1 of 2 >           |    |          < 2 of 2>            |
   * +-------------------------------+    +-------------------------------+
   */
  class Pager {
  public:
    enum class IndicatorType { DOT, NUM, NUM_ARROW, ARROW, NONE };
    enum class PagerActionType { EXIT, CONTINUE, DISABLE_NAV, ENABLE_NAV, TOGGLE_NAV };

    typedef std::function<PagerActionType(U8G2 *, InputFuncType)> PageFuncType;

  private:
    PageFuncType *displayFunctions;
    int numFuncs;
    int index = 0;
    PixelView *px;

  public:
    IndicatorType indicator;
    /**
     * @constructor
     *
     * @param numFuncs The number of functions in the functions array
     * @param displayFunctions An array of functions that render pages.
     *                         Note that each function needs to return PAGER_CONTINUE by default
     *                         Each function can return PAGER_* macros to execute actions
     *
     */

    Pager(PixelView *px, int numFuncs, PageFuncType *pages, const IndicatorType indicatorType = IndicatorType::DOT);

    /**
     * @brief Render the current page and manage input
     * @returns the ActionType returned by the currently rendering page
     */
    PagerActionType render();

    /**
     * @brief Loops till functions return PAGER_EXIT
     * @param
     * */
    void loop(int delay = 20);
  };

  /**
   * @class menuItem
   * @brief Contains a name and a 16x16 icon
   *
   */
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
   * @return the selected menuItem's index
   */
  int menu(menuItem items[], unsigned int numItems, int index = 0);
  /**
   * @brief Similar to `menu` but does not have icons and also has a header
   *
   * @param header The top header above the menu
   * @param items An array of char*. Used as the items to populate the array
   * @param numItems The number of items
   * @return The selected option
   */
  int subMenu(const char *header, const char *items[], unsigned int numItems, int index = 0);
  int subMenu(const char *header, const String items[], unsigned int numItems, int index = 0);

  int searchList(const char *header, const char *items[], unsigned int numItems, bool caseSensitive = true);
  int searchList(const char *header, const String items[], unsigned int numItems, bool caseSensitive = true);

  /**
   * @brief A grid of icons, you select one of them
   *
   * @param icons an array of bitmaps
   * @param numItems number of bitmaps
   * @return
   */
  int gridMenu(const unsigned char *icons[], int numItems);

  /**
   * @brief Radio buttons!!!
   *
   * @param header String to display with a highlight behind it
   * @param items An array of c-strs to show
   * @param numItems Number of items in `items`
   *
   * @return the selected item
   */
  const char *radioSelect(const char *header, const char *items[], const unsigned int numItems);

  struct checkBox {
    const char *name;
    bool isChecked;
  };

  /**
   * @brief Check Boxes
   *
   * @param header String to display with highlight behind it
   * @param items An array of `checkBox`
   * @param numItems Number of items in the providied array
   */
  void checkBoxes(const char *header, checkBox items[], const unsigned int numItems);

  /**
   * @brief Shows a list of items that you can scroll through
   *
   * @param header The header/title that remains static and doesnt scroll
   * @param iconBitmap A 16x16 icon, pass NULL to use no icon
   * @param items The list of items to be displayed
   * @param numItems Number of items in the list
   * @param displayType Can be Bullet points, numbers or nothing
   * @param font the font to use
   */

  enum class ListType { NONE, BULLET, NUMBER };

  void listBrowser(const char *header, const unsigned char iconBitmap[], const char *items[], unsigned int numItems,
                   ListType displayType = ListType::NUMBER);

  /**
   * @brief Shows a list of items that you can scroll through
   *
   * @param header The header/title that remains static and doesnt scroll
   * @param iconBitmap A 16x16 icon, pass NULL to use no icon
   * @param items The list of items to be displayed
   * @param numItems Number of items in the list
   * @param displayType Can be Bullet points, numbers or nothing
   * @param font the font to use
   */
  void listBrowser(const char *header, const unsigned char iconBitmap[], const String items[], unsigned int numItems,
                   ListType displayType = ListType::NUMBER);

  void progressBar(int progress, const char *header, const unsigned char *bitmap[] = NULL);

  void progressCircle(int frame);
};
