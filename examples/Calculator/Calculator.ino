#include <Arduino.h>
#include <U8g2lib.h>
#include <pixelView.h>

#define JOY_X 8
#define JOY_Y 7
#define BTN 6

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
PixelView pv(&u8g2, sendInput, delay, u8g2_font_haxrcorp4089_tr); // Change delay to vTaskDelay(..) if using esp32 (optional, but needed if you are using
PixelView::Keyboard kyb(pv);

enum Operation { MULTIPLY = 0, DIVIDE = 1, ADD = 2, SUBTRACT = 3 };

void calculator(PixelView *pv, PixelView::Keyboard *kyb) {
  while (true) {
    int num1;
    int num2;
    int result;

    const char *options[] = {"Multiply", "Divide", "Add", "Subtract"};

    num1 = (kyb->numPad("Enter first number")).toInt();
    num2 = (kyb->numPad("Enter second number")).toInt();

    Operation op = (Operation)pv->subMenu("Which operation?", options, 4);

    switch (op) {
    case MULTIPLY: {
      result = num1 * num2;
    } break;
    case DIVIDE: {
      result = num1 / num2;
    } break;
    case ADD: {
      result = num1 + num2;
    } break;
    case SUBTRACT: {
      result = num1 - num2;
    } break;
    };

    pv->showMessage(String("Answer is: " + String(result)).c_str());
  }
}

void setup() {
  pinMode(JOY_X, INPUT);
  pinMode(JOY_Y, INPUT);
  pinMode(BTN, INPUT_PULLUP);

  u8g2.begin();

  calculator(&pv, &kyb);
}

void loop() {}
