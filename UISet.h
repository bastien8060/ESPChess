//colors
#define darkgrey 0x2965
#define ligthgrey 0x6B4C
#define background_color 0x4207
#define splashscreen_bg 0x3165

#define chessboard_white 0xEF7A
#define chessboard_black 0x74AA

//chessboard functions
#include "chessboard.h"

bool wifi_enabled = false;

bool in_keyboard = false;

uint16_t tx, ty;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 150;

bool button_newgame = false;

String wifi_choose_list(int n) {

  String ssid = "0";
  return ssid;

}

void keyboard_redraw(char* alpha) { //redraw keyboard keys when needed.
  tft.fillRect(0, 160, 800, 340, background_color);
  char digits[] = "1234567890";

  //80 is the keys size in width/height.

  for (int i = 5; i < 730; i += 80) {
    tft.fillRoundRect(i, 80, 70, 70, 15, darkgrey); //draw keyboard's digits row
  }
  for (int i = 5; i < 730; i += 80) {
    tft.fillRoundRect(i, 160, 70, 70, 15, darkgrey); //draw keyboard's [Q-P] row
  }
  for (int i = 45; i < 730; i += 80) {
    tft.fillRoundRect(i, 240, 70, 70, 15, darkgrey); //draw keyboard's [A-L] row
  }
  for (int i = 5; i < 730; i += 80) {
    tft.fillRoundRect(i, 320, 70, 70, 15, darkgrey); //draw keyboard's [Z-M] row
  }
  tft.fillRoundRect(170, 400, 400, 70, 15, darkgrey); //spacebar

  for (int i = 0; i < 10; i++) { //write the labels for all 10digits /first row/digits row
    char c = digits[i];
    int row;
    row = 90;
    int coor = 25 + 80 * i;
    tft.textMode();
    tft.textTransparent(darkgrey);
    tft.textColor(RA8875_WHITE, darkgrey);
    tft.textSetCursor(coor, row);
    tft.textEnlarge(2);
    tft.print(c);
    tft.graphicsMode();

  }

  for (int i = 0; i < 10; i++) { //write the letters for first 10keys/second row
    char c = alpha[i];
    int row;
    row = 170;
    int coor = 25 + 80 * i;
    tft.textMode();
    tft.textTransparent(darkgrey);
    tft.textColor(RA8875_WHITE, darkgrey);
    tft.textSetCursor(coor, row);
    tft.textEnlarge(2);
    tft.print(c);
    tft.graphicsMode();

  }

  for (int i = 10; i < 19; i++) { //write the letters for third row
    char c = alpha[i];
    int row = 250;
    int coor = 80 * i - 10 * 80 + 70;
    tft.textMode();
    tft.textTransparent(darkgrey);
    tft.textColor(RA8875_WHITE, darkgrey);
    tft.textSetCursor(coor, row);
    tft.textEnlarge(2);
    tft.print(c);
    tft.graphicsMode();
  }

  for (int i = 19; alpha[i]; i++) { //write the letters for fourth row
    char c = alpha[i];
    int row = 330;
    int coor = 80 * i - 19 * 80 + 30;
    tft.textMode();
    tft.textTransparent(darkgrey);
    tft.textColor(RA8875_WHITE, darkgrey);
    tft.textSetCursor(coor, row);
    tft.textEnlarge(2);
    tft.print(c);
    tft.graphicsMode();
  }
}


String keyboard(String user_msg) {
  char lower_alpha[] = "qwertyuiopasdfghjkl^zxcvbnm<=";
  char upper_alpha[] = "QWERTYUIOPASDFGHJKL^ZXCVBNM<=";
  char digits[] = "1234567890";
  bool caps = false;

  in_keyboard = true;
  tft.fillScreen(background_color);
  tft.fillRoundRect(50, 10, 700, 50, 15, RA8875_WHITE);
  tft.textMode();
  tft.textTransparent(RA8875_WHITE);
  tft.textColor(RA8875_BLACK, RA8875_WHITE);
  tft.textSetCursor(55, 20);
  tft.textEnlarge(1);
  tft.print(user_msg); //write system message to user, for what to type.
  tft.graphicsMode();

  char alpha[] = "qwertyuiopasdfghjkl^zxcvbnm<="; //define keyboards keys in the right order.

  keyboard_redraw(alpha); //draw keys on keyboard

  bool entered = false; //bool to check if enter key was pressed
  char character; //store current character for every press
  String inputBuffer = ""; //store total user input.

  while (!entered) {
    if (tft.touched()) { //easy!
      if ((millis() - lastDebounceTime) > debounceDelay) {

        lastDebounceTime = millis();

        tsPoint_t calibrated = getCalibratedTouch();

        /* Draw a single pixel at the calibrated point */
        tx = calibrated.x;
        ty = calibrated.y;


        tx = 800 - tx;
        ty = 480 - ty;

        character = '`';
        if (ty > 90 && ty < 170) {
          character = digits[(tx - 25) / 80];
        } else if (ty > 170 && ty < 250) {
          if (caps) {
            character = upper_alpha[(tx - 25) / 80];
          } else {
            character = alpha[(tx - 25) / 80];
          }
        } else if (ty > 250 && ty < 330) {
          if (caps) {
            character = upper_alpha[(tx - 70 + 10 * 80) / 80];
          } else {
            character = alpha[(tx - 70 + 10 * 80) / 80];
          }
        } else if (ty > 330 && ty < 410) {
          if (caps) {
            character = upper_alpha[(tx - 30 + 19 * 80) / 80];
          } else {
            character = alpha[(tx - 30 + 19 * 80) / 80];
          }
        } else if (ty > 410) {
          character = ' ';
        }

        Serial.printf("Button Pressed: %c \n", character);
        if (character == '<') {
          inputBuffer.remove(inputBuffer.length() - 1);
        } else if (character == '`') {

        } else if (character == '=') {
          entered = true;
        } else if (character == '^') {
          if (!caps) {
            caps = true;
            keyboard_redraw(upper_alpha);

          } else {
            caps = false;
            keyboard_redraw(lower_alpha);
          }
        }
        else {
          inputBuffer += character;
        }
        String towrite = inputBuffer + '|';
        tft.fillRoundRect(50, 10, 700, 50, 15, RA8875_WHITE);
        tft.textMode();
        tft.textTransparent(RA8875_WHITE);
        tft.textColor(RA8875_BLACK, RA8875_WHITE);
        tft.textSetCursor(55, 20);
        tft.textEnlarge(1);
        tft.print(towrite);
        tft.graphicsMode();

      }

    }
  }
  in_keyboard = false;
  return inputBuffer;
}


void chess_homescreen() { //chess homescreen UI.
  tft.textEnlarge(0.5);
  screen = 0;
  tft.fillRect(0, 0, 800, 500, background_color); //bg
  tft.fillRect(0, 0, 40, 500, darkgrey); //left sidebar

  tft.fillRect(60, 30, 130, 130, darkgrey); //button 1
  tft.fillRect(200, 30, 130, 130, darkgrey); //button 2
  tft.fillRect(340, 30, 130, 130, darkgrey); //button 3

  tft.fillRect(490, 30, 290, 420, darkgrey); //menu profile
  tft.fillRect(510, 50, 80, 80, RA8875_WHITE); //Profile Pic

  tft.fillRect(0, 440, 40, 40, RA8875_YELLOW); //color button bottom

  if (wifi_enabled) {
    tft.fillCircle(tft.width() - 10, 10, 5, RA8875_GREEN);
  } else {
    tft.fillCircle(tft.width() - 10, 10, 5, RA8875_RED);
  }
  bmpDraw(newgame_bmp, 105, 65);
  bmpDraw(computer_bmp, 240, 65);
  bmpDraw(puzzles_bmp, 375, 65);

  tft.textMode();
  tft.textTransparent(darkgrey);
  tft.textColor(RA8875_WHITE, darkgrey);
  tft.textSetCursor(630, 50);
  tft.print("Louis2014 - IE");

  tft.textSetCursor(510, 150);
  tft.print("Games");
  tft.textSetCursor(740, 150);
  tft.print("570");

  tft.textSetCursor(510, 175);
  tft.print("Puzzles");
  tft.textSetCursor(740, 175);
  tft.print("250");

  tft.textSetCursor(510, 225);
  tft.print("Bullet");
  tft.textSetCursor(740, 225);
  tft.print("526");

  tft.textSetCursor(510, 250);
  tft.print("Rapid");
  tft.textSetCursor(740, 250);
  tft.print("526");

  tft.textSetCursor(510, 275);
  tft.print("Puzzles");
  tft.textSetCursor(740, 275);
  tft.print("1023");

  tft.textSetCursor(510, 300);
  tft.print("Blitz");
  tft.textSetCursor(740, 300);
  tft.print("767");

  tft.textSetCursor(510, 325);
  tft.print("Puzzle Rush");
  tft.textSetCursor(740, 325);
  tft.print("19");



  tft.textTransparent(darkgrey);
  tft.textColor(RA8875_WHITE, background_color);
  tft.textSetCursor(540, 0);
  tft.print("Chess.com - Play Chess Online");

  tft.graphicsMode();
}

void splashscreen() { //boot splashscreen
  tft.fillRect(0, 0, 800, 500, RA8875_WHITE);
  bmpDrawPath("/boot.bmp", 300, 200);
  delay(1000);
}

void interface() { //load homescreen.
  chess_homescreen();
}
