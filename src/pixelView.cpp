#include "pixelView.h"
#include <Arduino.h>
#include <U8g2lib.h>
#include <cstdio>

U8G2 *PixelView::u8g2 = nullptr;
function<int(void)> PixelView::doInput = nullptr;
function<void(int32_t)> PixelView::doDelay = nullptr;

// Implement the constructor
PixelView::PixelView(U8G2 *display, function<int(void)> inputFunction, function<void(int)> delayer,
                     const uint8_t font[])
    : font(font) {
  u8g2 = display;
  doInput = inputFunction;
  doDelay = delayer;
}

void PixelView::wordWrap(int xloc, int yloc, const char *text, bool maintainX) {
  int dspwidth = this->u8g2->getDisplayWidth(); // display width in pixels
  int strwidth = 0;                             // string width in pixels
  char glyph[2];
  int orignalX = xloc;
  glyph[1] = 0;
  for (const char *ptr = text, *lastblank = NULL; *ptr; ++ptr) {
    while (xloc == 0 && (*text == ' ' || *text == '\n'))
      if (ptr == text++)
        ++ptr; // skip blanks and newlines at the left edge

    glyph[0] = *ptr;
    strwidth += this->u8g2->getUTF8Width(glyph); // accumulate the pixel width

    if (*ptr == ' ')
      lastblank = ptr; // remember where the last blank was
    else
      ++strwidth; // non-blanks will be separated by one additional pixel

    if (*ptr == '\n' ||               // if we found a newline character,
        xloc + strwidth > dspwidth) { // or if we ran past the right edge of the display
      int starting_xloc = xloc;
      // print to just before the last blank, or to just before where we got to
      while (text < (lastblank ? lastblank : ptr)) {
        glyph[0] = *text++;
        xloc += this->u8g2->drawUTF8(xloc, yloc, glyph);
      }
      strwidth -= xloc - starting_xloc;       // account for what we printed
      yloc += this->u8g2->getMaxCharHeight(); // advance to the next line
      xloc = maintainX ? orignalX : 0;
      lastblank = NULL;
    }
  }
  while (*text) { // print any characters left over
    glyph[0] = *text++;
    xloc += this->u8g2->drawStr(xloc, yloc, glyph);
  }
}

bool PixelView::confirmYN(const char *message, bool defaultOption) {
  while (this->doInput() != ACTION_NONE)
    ;

  while (true) {
    auto render = [defaultOption, message, this]() {
      u8g2->clearBuffer();

      u8g2->setFont(u8g2_font_6x12_tr);
      this->wordWrap(2, 12, message);

      if (defaultOption) {
        u8g2->drawButtonUTF8(42, 50, U8G2_BTN_INV | U8G2_BTN_SHADOW2 | U8G2_BTN_HCENTER | U8G2_BTN_BW1, 0, 2, 2, "Yes");

        u8g2->drawButtonUTF8(84, 50, U8G2_BTN_SHADOW2 | U8G2_BTN_HCENTER | U8G2_BTN_BW1, 0, 2, 2, "No");
      } else {
        u8g2->drawButtonUTF8(42, 50, U8G2_BTN_SHADOW2 | U8G2_BTN_HCENTER | U8G2_BTN_BW1, 0, 2, 2, "Yes");

        u8g2->drawButtonUTF8(84, 50, U8G2_BTN_INV | U8G2_BTN_SHADOW2 | U8G2_BTN_HCENTER | U8G2_BTN_BW1, 0, 2, 2, "No");
      }
      u8g2->sendBuffer();
    };
    auto render2 = [defaultOption, message, this]() {
      u8g2->clearBuffer();

      u8g2->setFont(u8g2_font_6x12_tr);
      this->wordWrap(2, 12, message);
      if (defaultOption) {
        u8g2->drawButtonUTF8(45, 53, U8G2_BTN_INV | U8G2_BTN_HCENTER | U8G2_BTN_BW1, 0, 2, 2, "Yes");

        u8g2->drawButtonUTF8(84, 50, U8G2_BTN_SHADOW2 | U8G2_BTN_HCENTER | U8G2_BTN_BW1, 0, 2, 2, "No");
      } else {
        u8g2->drawButtonUTF8(42, 50, U8G2_BTN_SHADOW2 | U8G2_BTN_HCENTER | U8G2_BTN_BW1, 0, 2, 2, "Yes");

        u8g2->drawButtonUTF8(87, 53, U8G2_BTN_INV | U8G2_BTN_HCENTER | U8G2_BTN_BW1, 0, 2, 2, "No");
      }
      u8g2->sendBuffer();
    };
    render();

    int action = doInput();

    if (action == ACTION_NONE) {
      this->doDelay(150);
      continue;
    }

    if (action != ACTION_SEL) {
      defaultOption = !defaultOption;

      while (doInput() != ACTION_NONE)
        ;

    } else {
      render2();
      while (doInput() != ACTION_NONE)
        ;
      render();
      this->doDelay(150);
      return defaultOption;
    }
    this->doDelay(50);
  }
}

void PixelView::showMessage(const char *message) {

  while (doInput() != ACTION_NONE)
    ;

  u8g2->clearBuffer();

  u8g2->setFont(u8g2_font_6x12_tr);
  this->wordWrap(2, 12, message);
  u8g2->drawButtonUTF8(58, 56, U8G2_BTN_INV | U8G2_BTN_SHADOW2 | U8G2_BTN_HCENTER | U8G2_BTN_BW1, 0, 2, 2, "Okay");

  u8g2->sendBuffer();
  while (doInput() != ACTION_SEL)
    ;

  u8g2->clearBuffer();
  u8g2->setFont(u8g2_font_6x12_tr);
  this->wordWrap(2, 12, message);
  u8g2->drawButtonUTF8(61, 59, U8G2_BTN_INV | U8G2_BTN_HCENTER | U8G2_BTN_BW1, 0, 2, 2, "Okay");

  u8g2->sendBuffer();

  this->doDelay(150);
  while (doInput() != ACTION_NONE)
    ;

  u8g2->clearBuffer();
  u8g2->setFont(u8g2_font_6x12_tr);
  this->wordWrap(2, 12, message);
  u8g2->drawButtonUTF8(58, 56, U8G2_BTN_INV | U8G2_BTN_SHADOW2 | U8G2_BTN_HCENTER | U8G2_BTN_BW1, 0, 2, 2, "Okay");
  u8g2->sendBuffer();
  this->doDelay(50);
}

PixelView::Keyboard::Keyboard(PixelView &pixelView) : caps(false), insertIdx(0), p(pixelView), currentLayer(letters) {}

void PixelView::Keyboard::renderKeyboard(int pX, int pY, const String &text) {
  u8g2->clearBuffer();

  // Draw grid lines
  for (int i = 9; i <= 51; i += 14) {
    u8g2->drawLine(0, i, 128, i);
  }
  for (int i = 15; i <= 120; i += 12) {
    u8g2->drawLine(i, 9, i, 64);
  }

  // Render keys
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 10; j++) {
      int x = j * 12 + 7;
      int y = i * 14 + 18 + 2;

      u8g2->setFont(u8g2_font_6x13_me);

      if (strcmp(currentLayer[i][j], "<caps>") == 0) {
        if (caps || (j == pX && i == pY)) {
          u8g2->setFont(u8g2_font_6x12_t_symbols);
          u8g2->drawButtonUTF8(x, y, U8G2_BTN_INV, 0, 1, 1, "⇑");
        } else {
          u8g2->setFont(u8g2_font_6x12_t_symbols);
          u8g2->drawUTF8(x, y, "⇑");
        }
        continue;
      }

      if (strcmp(currentLayer[i][j], "<rm>") == 0) {
        if (j == pX && i == pY) {
          u8g2->setFont(u8g2_font_6x12_t_symbols);
          u8g2->drawButtonUTF8(x, y, U8G2_BTN_INV, 0, 1, 1, "⇐");
        } else {
          u8g2->setFont(u8g2_font_6x12_t_symbols);
          u8g2->drawUTF8(x, y, "⇐");
        }
        continue;
      }

      if (strcmp(currentLayer[i][j], "<sym1>") == 0) {
        if (j == pX && i == pY) {
          u8g2->setFont(u8g2_font_6x12_t_symbols);
          u8g2->drawButtonUTF8(x, y, U8G2_BTN_INV, 0, 1, 1, "#");
        } else {
          u8g2->setFont(u8g2_font_6x12_t_symbols);
          u8g2->drawUTF8(x, y, "#");
        }
        continue;
      }

      if (strcmp(currentLayer[i][j], "<sym2>") == 0) {
        if (j == pX && i == pY) {
          u8g2->setFont(u8g2_font_6x12_t_symbols);
          u8g2->drawButtonUTF8(x, y, U8G2_BTN_INV, 0, 1, 1, "¬");
        } else {
          u8g2->setFont(u8g2_font_6x12_t_symbols);
          u8g2->drawUTF8(x, y, "¬");
        }
        continue;
      }

      if (strcmp(currentLayer[i][j], "<let>") == 0) {
        if (j == pX && i == pY) {
          u8g2->setFont(u8g2_font_6x12_tr);
          u8g2->drawButtonUTF8(x, y, U8G2_BTN_INV, 0, 1, 1, "A");
        } else {
          u8g2->setFont(u8g2_font_6x12_t_symbols);
          u8g2->drawUTF8(x, y, "A");
        }
        continue;
      }
      if (strcmp(currentLayer[i][j], "<ques>") == 0) {
        if (j == pX && i == pY) {
          u8g2->setFont(u8g2_font_6x12_tr);
          u8g2->drawButtonUTF8(x, y, U8G2_BTN_INV, 0, 1, 1, "?");
        } else {
          u8g2->setFont(u8g2_font_6x12_t_symbols);
          u8g2->drawUTF8(x, y, "?");
        }
        continue;
      }
      if (strcmp(currentLayer[i][j], "<ok>") == 0) {
        if (j == pX && i == pY) {
          u8g2->setFont(u8g2_font_6x12_t_symbols);
          u8g2->drawButtonUTF8(x, y, U8G2_BTN_INV, 0, 1, 1, "✓");
        } else {
          u8g2->setFont(u8g2_font_6x12_t_symbols);
          u8g2->drawUTF8(x, y, "✓");
        }
        continue;
      }
      if (strcmp(currentLayer[i][j], "<clr>") == 0) {
        if (j == pX && i == pY) {
          u8g2->setFont(u8g2_font_6x12_t_symbols);
          u8g2->drawButtonUTF8(x, y, U8G2_BTN_INV, 0, 1, 1, "✕");
        } else {
          u8g2->setFont(u8g2_font_6x12_t_symbols);
          u8g2->drawUTF8(x, y, "✕");
        }
        continue;
      }
      if (strcmp(currentLayer[i][j], "<rev>") == 0) {
        if (j == pX && i == pY) {
          u8g2->setFont(u8g2_font_6x12_t_symbols);
          u8g2->drawButtonUTF8(x, y, U8G2_BTN_INV, 0, 1, 1, "©");
        } else {
          u8g2->setFont(u8g2_font_6x12_t_symbols);
          u8g2->drawUTF8(x, y, "©");
        }
        continue;
      }
      if (strcmp(currentLayer[i][j], "<left>") == 0) {
        if (j == pX && i == pY) {
          u8g2->setFont(u8g2_font_6x12_t_symbols);
          u8g2->drawButtonUTF8(x, y, U8G2_BTN_INV, 0, 1, 1, "←");
        } else {
          u8g2->setFont(u8g2_font_6x12_t_symbols);
          u8g2->drawUTF8(x, y, "←");
        }
        continue;
      }
      if (strcmp(currentLayer[i][j], "<right>") == 0) {
        if (j == pX && i == pY) {
          u8g2->setFont(u8g2_font_6x12_t_symbols);
          u8g2->drawButtonUTF8(x, y, U8G2_BTN_INV, 0, 1, 1, "→");
        } else {
          u8g2->setFont(u8g2_font_6x12_t_symbols);
          u8g2->drawUTF8(x, y, "→");
        }
        continue;
      }

      // Draw character
      if (j == pX && i == pY)
        u8g2->drawButtonUTF8(x, y, U8G2_BTN_INV, 0, 1, 1, currentLayer[i][j]);
      else
        u8g2->drawStr(x, y, currentLayer[i][j]);
    }
  }

  String displayText = text;
  if (text.length() > 19) {
    displayText = displayText.substring(text.length() - 19, text.length());
  }

  u8g2->setFont(u8g2_font_6x12_me);
  u8g2->drawStr(2, 7, displayText.c_str());

  int cursorX = (u8g2->getMaxCharWidth() * displayText.length()) + 2;
  u8g2->drawVLine(cursorX, 0, 8);

  u8g2->sendBuffer();
}

String PixelView::Keyboard::numPad(const char *defaultText, bool isEmptyAllowed) {
  short indexX = 0;
  short indexY = 0;
  String text = defaultText;

  /*
   * x = 12i+4
   * y = 12i+7
   */

  while (true) {
    u8g2->clearBuffer();
    u8g2->setFontMode(1);
    u8g2->setBitmapMode(1);
    u8g2->setFont(u8g2_font_profont12_tf);
    for (int i = 0; i <= 3; i++) {
      for (int j = 0; j <= 2; j++) {
        u8g2->drawUTF8((j + 1) * 12 - 4, (i + 1) * 12 + 7, numpad[i][j]);
      }
    }

    u8g2->drawRFrame(4, 8, 37, 50, 0);
    u8g2->setFont(u8g2_font_haxrcorp4089_tr);
    this->p.wordWrap(50, 21, text.c_str(), true);

    u8g2->setDrawColor(2);
    u8g2->drawBox((indexX + 1) * 12 - 4 - 2, ((indexY + 1) * 12 + 7 - 9), 9, 10);
    u8g2->sendBuffer();

    int action = doInput();
    if (action == ACTION_UP) {
      indexY = max(0, indexY - 1);
      while (doInput() != ACTION_NONE)
        ;
    }
    if (action == ACTION_DOWN) {
      int newY = min(3, indexY + 1);
      if (strcmp(numpad[newY][indexX], " ") > 0)
        indexY = newY;

      while (doInput() != ACTION_NONE)
        ;
    }
    if (action == ACTION_LEFT) {
      // indexX = max(0, indexX - 1);
      int newX = max(0, indexX - 1);

      if (strcmp(numpad[indexY][newX], " ") > 0)
        indexX = newX;

      while (doInput() != ACTION_NONE)
        ;
    }
    if (action == ACTION_RIGHT) {
      indexX = min(2, indexX + 1);
      while (doInput() != ACTION_NONE)
        ;
    }
    if (action == ACTION_SEL) {
      if (strcmp(numpad[indexY][indexX], "\u0087") == 0) {
        if ((text.length() <= 0) && (isEmptyAllowed == false)) {
          p.showMessage("Error: Cannot be empty");
          goto skip_append;
        } else {
          break;
        }
      }
      if (strcmp(numpad[indexY][indexX], "<") == 0) {
        text.remove(text.length() - 1);
      } else {
        text += numpad[indexY][indexX];
      }
    }
  skip_append:
    Serial.println(text);
    while (doInput() != ACTION_NONE) {
      doDelay(50);
    }
    doDelay(50);
  }
  return text;
}

String PixelView::Keyboard::fullKeyboard(const String &message, bool isEmptyAllowed, const String &defaultText) {
  while (doInput() != ACTION_NONE)
    ;
  if (message.length() != 0)
    p.showMessage(message.c_str());

  String text = defaultText;
  insertIdx = text.length();
  int pointerX = 0;
  int pointerY = 0;

  int action;

  bool exit = false;
  while (!exit) {
    renderKeyboard(pointerX, pointerY, text);

    action = doInput();
    switch (action) {
    case ACTION_LEFT: {
      pointerX = max(0, pointerX - 1);
      break;
    }
    case ACTION_RIGHT: {
      pointerX = min(9, pointerX + 1);
      break;
    }
    case ACTION_DOWN: {
      pointerY = min(3, pointerY + 1);
      break;
    }
    case ACTION_UP: {
      pointerY = max(0, pointerY - 1);
      break;
    }
    default:
      break;
    }

    if (action == ACTION_SEL) {
      while (doInput() != ACTION_NONE)
        ;
      if (strcmp(currentLayer[pointerY][pointerX], "<caps>") == 0) {
        if (!caps)
          currentLayer = capitalLetters;
        else
          currentLayer = letters;

        caps = !caps;
        goto skip_append;
      }

      if (strcmp(currentLayer[pointerY][pointerX], "<sym1>") == 0) {
        currentLayer = symbols1;
        goto skip_append;
      }

      if (strcmp(currentLayer[pointerY][pointerX], "<sym2>") == 0) {
        currentLayer = symbols2;
        goto skip_append;
      }

      if (strcmp(currentLayer[pointerY][pointerX], "<let>") == 0) {
        currentLayer = letters;
        goto skip_append;
      }

      if (strcmp(currentLayer[pointerY][pointerX], "<rm>") == 0) {
        if (insertIdx > 0) {
          text.remove(--insertIdx, 1);
        }
        goto skip_append;
      }

      if (strcmp(currentLayer[pointerY][pointerX], "<left>") == 0) {
        if (insertIdx > 0)
          insertIdx--;
        goto skip_append;
      }

      if (strcmp(currentLayer[pointerY][pointerX], "<right>") == 0) {
        if (insertIdx < text.length())
          insertIdx++;
        goto skip_append;
      }

      if (strcmp(currentLayer[pointerY][pointerX], "<ok>") == 0) {
        if (isEmptyAllowed == false && text.length() == 0)
          p.showMessage("ERROR:\n Text cannot be empty");
        else
          exit = true;
        goto skip_append;
      }

      if (strcmp(currentLayer[pointerY][pointerX], "<rev>") == 0) {
        // Preview the current text
        u8g2->clearBuffer();
        u8g2->setFont(u8g2_font_6x12_tr);
        p.wordWrap(2, 7, text.length() == 0 ? "No text input" : text.c_str());
        u8g2->sendBuffer();

        while (doInput() != ACTION_SEL)
          ;
        while (doInput() != ACTION_NONE)
          ;

        goto skip_append;
      }

      if (strcmp(currentLayer[pointerY][pointerX], "<clr>") == 0) {
        if (p.confirmYN("Clear text?")) {
          text = "";
          insertIdx = 0;
        }
        goto skip_append;
      }

      if (strcmp(currentLayer[pointerY][pointerX], "<ques>") == 0) {
        if (message.length() != 0)
          p.showMessage(message.c_str());
        goto skip_append;
      }

      // Handle character insertion at insertIdx
      if (caps) {
        String x = String(currentLayer[pointerY][pointerX]);
        x.toUpperCase();
        text = text.substring(0, insertIdx) + x + text.substring(insertIdx);
      } else {
        text = text.substring(0, insertIdx) + currentLayer[pointerY][pointerX] + text.substring(insertIdx);
      }

      insertIdx++; // Move cursor right after insertion
    }
  skip_append:

    doDelay(100);
  }

  return text;
}

PixelView::Pager::Pager(int numFuncs, std::function<void(U8G2, std::function<int(void)>)> *displayFunctions,
                        const int indicatorType) {
  this->numFuncs = numFuncs;
  this->displayFunctions = displayFunctions;
  this->indicator = indicatorType;
}

void PixelView::Pager::render() {
  u8g2->clearBuffer();
  displayFunctions[index](*u8g2, doInput);

  switch (this->indicator) {
  case PAGE_DOT_NAV: {
    String str;
    for (int i = 0; i < numFuncs; i++)
      str += ((i == index) ? "●" : "○");

    int centerX = (u8g2->getDisplayWidth() - (u8g2->getUTF8Width(str.c_str()))) / 2;
    u8g2->setFont(u8g2_font_unifont_t_75);
    u8g2->drawUTF8(centerX, 64, str.c_str());
    break;
  }
  case PAGE_NUM_NAV: {
    char buf[64];
    sprintf(buf, "%d of %d", index + 1, numFuncs);
    int centerX = (u8g2->getDisplayWidth() - (u8g2->getUTF8Width(buf))) / 2;

    u8g2->setFont(u8g2_font_6x12_tr);
    u8g2->drawStr(centerX, 64, buf);
    sprintf(buf, "< %d of %d >", index + 1, numFuncs);
    break;
  };
  case PAGE_NUM_AND_ARROW_NAV: {
    char buf[64];
    sprintf(buf, "< %d of %d >", index + 1, numFuncs);
    int centerX = (u8g2->getDisplayWidth() - (u8g2->getUTF8Width(buf))) / 2;

    u8g2->setFont(u8g2_font_6x12_tr);
    u8g2->drawStr(centerX, 64, buf);
    break;
  };
  case PAGE_ARROW_NAV: {
    const char *buf = "<      >";
    int centerX = (u8g2->getDisplayWidth() - (u8g2->getUTF8Width(buf))) / 2;
    u8g2->drawStr(centerX, 64, buf);
    break;
  }
  };

  u8g2->sendBuffer();

  int input = doInput();

  if ((input == ACTION_LEFT) || (input == ACTION_UP)) {
    index--;
    while (doInput() != ACTION_NONE)
      vTaskDelay(pdMS_TO_TICKS(100));
  }

  if ((input == ACTION_RIGHT) || (input == ACTION_DOWN)) {
    index++;
    while (doInput() != ACTION_NONE)
      vTaskDelay(pdMS_TO_TICKS(100));
  }

  if (index < 0) {
    index = numFuncs - 1; // Wrap around to the last element
  } else if (index >= numFuncs) {
    index = 0; // Wrap around to the first element
  }
}

static const unsigned char bitmap_sel_outline[] U8X8_PROGMEM = {
    0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0x04, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x0c, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x0c, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x02, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x02, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x0c, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x0c, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x02,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x02, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x0c, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c,
    0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0xf8, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03};

static const unsigned char bitmap_scrollbar_background_full[] U8X8_PROGMEM = {
    0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40,
    0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40,
    0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40,
    0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x00};

PixelView::menuItem PixelView::menu(menuItem items[], unsigned int numItems) {
  int itemSelected = 0;
  int prevItem;
  int nextItem;

  while (true) {
    int input = doInput();

    if (input == ACTION_UP) {

      itemSelected--;
      if (itemSelected < 0)
        itemSelected = numItems - 1;
      while (doInput() != ACTION_NONE) {
        doDelay(70);
      }
    }

    if (input == ACTION_DOWN) {
      itemSelected++;
      if (itemSelected >= numItems)
        itemSelected = 0;
      while (doInput() != ACTION_NONE) {
        doDelay(70);
      }
    }

    if (input == ACTION_SEL) {
      return items[itemSelected];
      while (doInput() != ACTION_NONE) {
        doDelay(70);
      }
    }

    prevItem = itemSelected - 1;
    if (prevItem < 0)
      prevItem = numItems - 1;

    nextItem = itemSelected + 1;
    if (nextItem >= numItems)
      nextItem = 0;

    u8g2->clearBuffer();
    u8g2->setFontMode(1);
    u8g2->setBitmapMode(1);

    u8g2->drawXBMP(4, 2, 16, 16, items[prevItem].icon);
    u8g2->drawXBMP(4, 24, 16, 16, items[itemSelected].icon);
    u8g2->drawXBMP(4, 46, 16, 16, items[nextItem].icon);

    int maxWidth = 90;

    u8g2->setFont(u8g2_font_helvR08_tr);

    u8g2->drawStr(24, 15, items[prevItem].name.c_str());

    u8g2->drawXBMP(0, 22, 128, 21, bitmap_sel_outline);

    // Draw selected item with a bold font
    u8g2->setFont(u8g2_font_helvB08_tr);
    u8g2->drawStr(25, 37, items[itemSelected].name.c_str());

    // Draw next item
    u8g2->setFont(u8g2_font_helvR08_tr);

    u8g2->drawStr(25, 59, items[nextItem].name.c_str());

    u8g2->setDrawColor(1);
    u8g2->drawXBMP(120, 0, 8, 64, bitmap_scrollbar_background_full);
    u8g2->drawRBox(125, 64 / numItems * itemSelected, 3, 64 / numItems, 1);

    u8g2->sendBuffer();
  }
}

const char *PixelView::subMenu(const char *header, const char *items[], unsigned int numItems) {
  int itemSelected = 0;
  int prevItem;
  int nextItem;

  while (true) {

    int input = doInput();
    if (input == ACTION_UP) {

      itemSelected--;
      if (itemSelected < 0)
        itemSelected = numItems - 1;
      while (doInput() != ACTION_NONE) {
        doDelay(70);
      }
    }

    if (input == ACTION_DOWN) {
      itemSelected++;
      if (itemSelected >= numItems)
        itemSelected = 0;
      while (doInput() != ACTION_NONE) {
        doDelay(70);
      }
    }

    if (input == ACTION_SEL) {
      return items[itemSelected];
      while (doInput() != ACTION_NONE) {
        doDelay(70);
      }
    }
    prevItem = itemSelected - 1;
    if (prevItem < 0)
      prevItem = numItems - 1;

    nextItem = itemSelected + 1;
    if (nextItem >= numItems)
      nextItem = 0;

    u8g2->clearBuffer();
    u8g2->setFontMode(1);
    // u8g2->setBitmapMode(1);
    u8g2->setDrawColor(1);

    u8g2->drawXBMP(120, 0, 8, 64, bitmap_scrollbar_background_full);

    int scrollbarH = max(1, (int)(64 / numItems));
    int scrollbarY;
    if ((64 / numItems * itemSelected) < 0)
      scrollbarY = 1;
    else
      scrollbarY = 64 / numItems * itemSelected;

    u8g2->drawRBox(125, scrollbarY, 3, scrollbarH, 1);

    u8g2->setFont(u8g2_font_helvB08_tr);
    u8g2->drawStr(1, 11, header);

    u8g2->setFont(u8g2_font_helvR08_tr);
    u8g2->drawStr(8, 28, items[prevItem]);
    u8g2->drawStr(8, 44, items[itemSelected]);
    u8g2->drawStr(8, 60, items[nextItem]);

    u8g2->setDrawColor(2);
    u8g2->drawRBox(2, 33, 121, 15, 1);
    u8g2->sendBuffer();
    u8g2->setDrawColor(1);

    doDelay(50);
  }
}
void PixelView::listBrowser(const char *header, const unsigned char iconBitmap[], const char *items[],
                            unsigned int numItems, int displayType, const uint8_t font[]) {

  unsigned int offset = 0; // Offset for scrolling
  int displayHeight = u8g2->getDisplayHeight();

  u8g2->setFont(font);
  int fontHeight = u8g2->getMaxCharHeight();

  // Reserve space for the header and recalculate visible items
  int headerHeight = u8g2->getMaxCharHeight();   // Assuming header takes up one line
  int listHeight = displayHeight - headerHeight; // List area excluding the header
  int visibleItems = listHeight / fontHeight;    // How many list items fit below the header

  // Adjust visibleItems if there are fewer items than what can fit on the screen
  if (visibleItems > numItems) {
    visibleItems = numItems;
  }

  do {
    u8g2->clearBuffer(); // Clear the screen buffer
    u8g2->setFont(u8g2_font_helvB08_tr);

    // Draw the header at the top
    int headerWidth = u8g2->getUTF8Width(header);

    int headerX;
    if (iconBitmap != NULL)
      headerX = (u8g2->getDisplayWidth() - (u8g2->getUTF8Width(header))) / 2;
    else
      headerX = ((u8g2->getDisplayWidth() - (u8g2->getUTF8Width(header))) / 2) - 6;

    u8g2->drawStr(headerX + 2, headerHeight,
                  header); // Draw header at the top

    u8g2->setDrawColor(2);
    u8g2->drawRBox(headerX, 2, headerWidth + 4, headerHeight + 0, 0); // Draw background for header
    u8g2->setDrawColor(1);
    if (iconBitmap != NULL)
      u8g2->drawXBMP(headerX - 16 - 4, 0, 16, 16, iconBitmap);

    u8g2->setFont(font);

    // Scroll handle height and position calculation
    int handleHeight = (64 * visibleItems) / numItems;
    int handlePosition = ((64 * offset) / numItems);

    // Draw scrollbar

    // u8g2->drawRBox(123, 17, 3, 4, 1);
    u8g2->drawXBMP(120, 0, 8, 64, bitmap_scrollbar_background_full);
    u8g2->drawRBox(125, handlePosition, 3, handleHeight, 1);

    // Display list items below the header
    for (int i = 0; i < visibleItems; i++) {
      int itemIndex = i + offset; // Adjust for scrolling

      if (itemIndex >= numItems) {
        break; // Prevent out-of-bound access when at the last item
      }

      char buf[128];
      switch (displayType) {
      case LIST_NONE: {
        strcpy(buf, items[itemIndex]);
        break;
      }
      case LIST_BULLET_POINT: {
        sprintf(buf, "-° %s", items[itemIndex]);
        break;
      }
      case LIST_NUMBER: {
        sprintf(buf, "%d. %s", itemIndex + 1, items[itemIndex]);
      }
      }

      // Render list items below the header (start from headerHeight)
      u8g2->drawStr(5, headerHeight + (i + 1) * u8g2->getMaxCharHeight(), buf); // Display each item
    }

    u8g2->sendBuffer(); // Send buffer to the display

    // Handle input actions
    int action = doInput();

    if (action == ACTION_UP) {
      if (offset > 0) {
        offset--;
      }
    }
    if (action == ACTION_DOWN) {
      if ((numItems > visibleItems) && offset < (numItems - visibleItems)) {
        offset++;
      }
    }
    if (action == ACTION_SEL) {
      while (doInput() == ACTION_SEL)
        ; // Wait until selection is released
      break;
    }

    while (doInput() != ACTION_NONE)
      ; // Wait for no input
  } while (true);
}
