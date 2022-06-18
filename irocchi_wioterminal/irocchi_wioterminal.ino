//Guess the different colors game
//Wio Terminal
//Ver.1.0.0
//by 

#include <stdio.h>

#define FARM_MAJOR_VER (1)
#define FARM_MINOR_VER (0)
#define FARM_REVISION  (0)

#include <TFT_eSPI.h> // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

#include <JC_Button.h>
#define DEBOUNCE_MS 10
Button BtnA = Button(WIO_KEY_A, true, DEBOUNCE_MS);
Button BtnB = Button(WIO_KEY_B, true, DEBOUNCE_MS);
Button BtnC = Button(WIO_KEY_C, true, DEBOUNCE_MS);
Button BtnU = Button(WIO_5S_UP, true, DEBOUNCE_MS);
Button BtnD = Button(WIO_5S_DOWN, true, DEBOUNCE_MS);
Button BtnL = Button(WIO_5S_LEFT, true, DEBOUNCE_MS);
Button BtnR = Button(WIO_5S_RIGHT, true, DEBOUNCE_MS);
Button BtnP = Button(WIO_5S_PRESS, true, DEBOUNCE_MS);

void Button_update() {
  //Button update
  BtnA.read();
  BtnB.read();
  BtnC.read();

  BtnU.read();
  BtnD.read();
  BtnL.read();
  BtnR.read();
  BtnP.read();
}

uint8_t cursor_x = 0;
uint8_t cursor_y = 0;

uint8_t point_num = 0;

uint16_t offset_x = 39;
uint16_t offset_y = 39;

uint16_t count = 0;
uint16_t ok_score = 0;
uint16_t err_score = 0;

uint16_t color_15;
uint16_t color_1;


uint16_t BuildRGB565(uint8_t R, uint8_t G, uint8_t B) {
  return ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3);
};

//Score Display
void draw_score(uint16_t num_ok, uint16_t num_err) {

  char mes[24] = "";

  tft.fillRect(229, offset_y, 319 - 229, 239 - offset_y, TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_BLACK, TFT_CYAN);
  sprintf(mes, "%4d", num_ok);
  tft.drawString(mes, 230, 160, 1);

  tft.setCursor(230, 180);
  tft.setTextColor(TFT_BLACK, TFT_RED);
  sprintf(mes, "%4d", num_err);
  tft.print(mes);

  tft.setCursor(230, 200);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  sprintf(mes, "%3d%%", (uint8_t)(num_ok * 100 / count));
  tft.print(mes);

  tft.drawLine(230, offset_y, 230 , 239 , TFT_BLACK);

}

//Moving frame display
void draw_waku(uint8_t num, uint16_t color) {

  uint8_t yy = num / 4;
  uint8_t xx = num % 4;

  tft.drawLine(offset_x + (xx * 40), offset_y + (yy * 40), offset_x + (xx * 40) + 40 , offset_y + (yy * 40) , color);
  tft.drawLine(offset_x + (xx * 40), offset_y + (yy * 40), offset_x + (xx * 40) , offset_y + (yy * 40) + 40 , color);
  tft.drawLine(offset_x + (xx * 40), offset_y + (yy * 40) + 40, offset_x + (xx * 40) + 40 , offset_y + (yy * 40) + 40, color);
  tft.drawLine(offset_x + (xx * 40) + 40, offset_y + (yy * 40), offset_x + (xx * 40) + 40 , offset_y + (yy * 40) + 40 , color);
}

//(x,y) -> 0-15
uint8_t cursor_cal(uint8_t x_num, uint8_t y_num) {
  return x_num + (y_num * 4);
}


void draw4x4(uint8_t num, uint16_t color1, uint16_t color2) {
  int i, j;
  uint16_t color[16];

  for (i = 0; i < 16; i++) {
    if (i == num) {
      color[i] = color2;
    } else {
      color[i] = color1;
    }
  }

  tft.fillRect(offset_x, offset_y, 161, 161, TFT_BLACK);

  for (i = 0, j = 0; i < 4; i++, j++) {
    tft.fillRect(offset_x + 1 + (j * 40), offset_y + 1 , 39, 39, color[i]);
  }
  for (i = 4, j = 0; i < 8; i++, j++) {
    tft.fillRect(offset_x + 1  + (j * 40), offset_y + 1 + 40, 39, 39, color[i]);
  }
  for (i = 8, j = 0; i < 12; i++, j++) {
    tft.fillRect(offset_x + 1  + (j * 40), offset_y + 1 + 80, 39, 39, color[i]);
  }
  for (i = 12, j = 0; i < 16; i++, j++) {
    tft.fillRect(offset_x + 1  + (j * 40), offset_y + 1 + 120, 39, 39, color[i]);
  }
}



//Play tone
void playTone(int tone1, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone1 * 2) {
    digitalWrite(WIO_BUZZER, HIGH);
    delayMicroseconds(tone1);
    digitalWrite(WIO_BUZZER, LOW);
    delayMicroseconds(tone1);
  }
}


void create_game(void) {

  char mes[24] = "";

  point_num = random(0, 15);

//reference color
  uint8_t ran_r = random(10, 245);
  uint8_t ran_g = random(10, 245);
  uint8_t ran_b = random(10, 245);

//different color
  int8_t ran_r1;
  int8_t ran_g1 ;
  int8_t ran_b1;
  
  if (count < 10) {
    ran_r1 = random(5, 10);
    ran_g1 = random(5, 10);
    ran_b1 = random(5, 10);
  } else if (count < 20) {
    ran_r1 = random(0, 10);
    ran_g1 = random(5, 10);
    ran_b1 = random(0, 10);
  } else {
    ran_r1 = random(-10, 10);
    ran_g1 = random(-10, 10);
    ran_b1 = random(-10, 10);
  }

  if ((ran_r1 == 0 ) && (ran_g1 == 0 ) && (ran_b1 == 0 )) {
    ran_r1 = 1;
    ran_g1 = 1;
    ran_b1 = 1;
  }

  uint8_t ran_r11 = ran_r - ran_r1;
  uint8_t ran_g11 = ran_g - ran_g1;
  uint8_t ran_b11 = ran_b - ran_b1;

  color_15 = BuildRGB565(ran_r, ran_g, ran_b);
  color_1 = BuildRGB565(ran_r11, ran_g11, ran_b11);

  if (ran_r1 < 0) {
    ran_r1 = 0 - ran_r1;
  }
  if (ran_g1 < 0) {
    ran_g1 = 0 - ran_g1;
  }
  if (ran_b1 < 0) {
    ran_b1 = 0 - ran_b1;
  }

  uint8_t level = 31 - ran_r1 - ran_g1 - ran_b1;

  draw4x4(point_num, color_15, color_1);
  cursor_x = 0;
  cursor_y = 0;
  draw_waku(cursor_cal(cursor_x, cursor_y), TFT_WHITE);

  tft.fillRect(0, 0, 319, offset_y - 1, TFT_BLACK);

  tft.setTextSize(2);
  tft.setCursor(1, 1);
  tft.setTextColor(TFT_WHITE);
  count++;
  tft.print(count);

  tft.setCursor(100, 1);
  tft.print("Level:");
  tft.print(level);

  tft.fillRect(229, 10, 100, 100, TFT_BLACK);
  tft.fillRect(229, 10, 100, 100, TFT_BLACK);
}

void setup() {

  uint16_t i, j = 0;
  char mes[24] = "";

  Serial.begin(115200);

  pinMode(WIO_KEY_A, INPUT_PULLUP);
  pinMode(WIO_KEY_B, INPUT_PULLUP);
  pinMode(WIO_KEY_C, INPUT_PULLUP);

  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  pinMode(WIO_5S_LEFT, INPUT_PULLUP);
  pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);

  pinMode(WIO_BUZZER, OUTPUT);

  tft.init();
  tft.fillScreen(TFT_BLACK);

  tft.setRotation(3);
  tft.setCursor(0, 0);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);

  sprintf(mes, "Ver.%d.%d.%d",  FARM_MAJOR_VER, FARM_MINOR_VER, FARM_REVISION);
  tft.println(mes);
  tft.println("");

//title
  tft.setTextSize(8);
  tft.setCursor(10, 100);
  tft.setTextColor(TFT_RED);
  tft.print("I");
  tft.setTextColor(TFT_ORANGE);
  tft.print("R");
  tft.setTextColor(TFT_YELLOW);
  tft.print("O");
  tft.setTextColor(TFT_GREENYELLOW);
  tft.print("C");
  tft.setTextColor(TFT_CYAN);
  tft.print("C");
  tft.setTextColor(TFT_BLUE);
  tft.print("H");
  tft.setTextColor(TFT_PURPLE);
  tft.print("I");

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(0, 220);
  tft.println("Press the button to start");

  while (1) {
    if (digitalRead(WIO_5S_PRESS) == LOW) {
      delay(500);
      break;
    }
  }

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.print(count);

  cursor_x = 0;
  cursor_y = 0;

  randomSeed(millis());

  ok_score = 0;
  err_score = 0;

  draw_score(ok_score, err_score);

  create_game();

}

void loop() {

  if (BtnU.wasPressed()) {
    if (cursor_y != 0) {
      draw_waku(cursor_cal(cursor_x, cursor_y), TFT_BLACK);
      cursor_y--;
      draw_waku(cursor_cal(cursor_x, cursor_y), TFT_WHITE);
    }
  }

  if (BtnD.wasPressed()) {
    if (cursor_y != 3) {
      draw_waku(cursor_cal(cursor_x, cursor_y), TFT_BLACK);
      cursor_y++;
      draw_waku(cursor_cal(cursor_x, cursor_y), TFT_WHITE);
    }
  }

  if (BtnL.wasPressed()) {
    if (cursor_x != 0) {
      draw_waku(cursor_cal(cursor_x, cursor_y), TFT_BLACK);
      cursor_x--;
      draw_waku(cursor_cal(cursor_x, cursor_y), TFT_WHITE);
    }
  }


  if (BtnR.wasPressed()) {
    if (cursor_x != 3) {
      draw_waku(cursor_cal(cursor_x, cursor_y), TFT_BLACK);
      cursor_x++;
      draw_waku(cursor_cal(cursor_x, cursor_y), TFT_WHITE);
    }
  }

  if (BtnP.wasPressed()) {

    
    if (point_num == cursor_cal(cursor_x, cursor_y)) {
      ok_score++;
      playTone(500, 100);
    } else {

      err_score++;
      playTone(1915, 500);

      draw_waku(cursor_cal(cursor_x, cursor_y), TFT_BLACK);
      draw_waku(point_num, TFT_WHITE);
      delay(500);
      draw_waku(point_num, TFT_BLACK);
      delay(500);
      draw_waku(point_num, TFT_WHITE);
      delay(500);
      draw_waku(point_num, TFT_BLACK);
      delay(500);
      draw_waku(point_num, TFT_WHITE);

    }

    draw_score(ok_score, err_score);
    create_game();

  }

  Button_update() ;

}
