#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Keypad.h>
#include <Metro.h>
#include "Projectile.h"

// declarare display
#define TFT_CS 10
#define TFT_RST 9
#define TFT_DC 8
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

//declarare butoane utilizate de pe keypad
const byte ROWS = 2;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'}
};
//declarare pini pentru keypad
byte rowPins[ROWS] = {5, 6};
byte colPins[COLS] = {3, 4, 2};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

//vector de gloante
const short maxBullets = 50;
Projectile bullets[maxBullets];
short bulletCounter = 0;


//temporizare
Metro shootT = Metro(1000);
Metro projectileMoveT = Metro(100);
boolean shootState = false;
boolean projectileMoveState = false;


//////////////////////////////////////////////////AFISAJ VARIABILE///////////////////////////////////////
// constrante
const byte projectileSize = 2;
const byte moveDistance = 5;


// variabile
int16_t xNow = 64;
int16_t yNow = 150;
int16_t state = 0;//0 = idle, 4= left, 6= right

//start game
byte start = 0;


void setup() {
  // put your setup code here, to run once:
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST7735_BLACK);
  Serial.begin(9600);
  drawAircraft(xNow, yNow, ST7735_WHITE);

  //state = idle(== jumping)
  state = 0;
}

void loop() {

  if (start == 1) {
    if (shootT.check()) {
      shootState = !shootState;
      shoot();
    }
    if (projectileMoveT.check()) {
      projectileMoveState = !projectileMoveState;
      projectileMove();
    }
  }
  char key = keypad.getKey();
  if (key) {
    Serial.println(key);

    switch (key) {
      case '4': moveLeft(); break;
      case '6': moveRight(); break;
      default: Serial.println("DK which button you pressed");
    }

  }
  //  Serial.println(state);
}

//////////////////////////////////////////////////AFISAJ TFT///////////////////////////////////////

void moveLeft() {
  Serial.println("move left");
  drawAircraft(xNow, yNow, ST7735_BLACK);
  if (xNow - moveDistance < 0) {
    xNow = 128;
  }
  xNow -= moveDistance;
  drawAircraft(xNow, yNow, ST7735_WHITE);

}

void moveRight() {
  Serial.println("move right");
  drawAircraft(xNow, yNow, ST7735_BLACK);
  if (xNow + moveDistance > 128) {
    xNow = 0;
  }
  xNow += moveDistance;
  drawAircraft(xNow, yNow, ST7735_WHITE);

}

void drawAircraft(uint16_t x, uint16_t y, uint16_t color) {

  tft.drawPixel(x - 1, y - 4, color);
  tft.drawPixel(x + 1, y - 4, color);

  tft.drawPixel(x - 1, y - 3, color);
  tft.drawPixel(x, y - 3, color);
  tft.drawPixel(x + 1, y - 3, color);

  tft.drawPixel(x - 4, y - 2, color);
  tft.drawPixel(x - 3, y - 2, color);
  tft.drawPixel(x - 2, y - 2, color);
  tft.drawPixel(x - 1, y - 2, color);
  tft.drawPixel(x, y - 2, color);
  tft.drawPixel(x + 1, y - 2, color);
  tft.drawPixel(x + 2, y - 2, color);
  tft.drawPixel(x + 3, y - 2, color);
  tft.drawPixel(x + 4, y - 2, color);

  tft.drawPixel(x - 3, y - 1, color);
  tft.drawPixel(x - 2, y - 1, color);
  tft.drawPixel(x - 1, y - 1, color);
  tft.drawPixel(x, y - 1, color);
  tft.drawPixel(x + 1, y - 1, color);
  tft.drawPixel(x + 2, y - 1, color);
  tft.drawPixel(x + 3, y - 1, color);

  tft.drawPixel(x - 2, y, color);
  tft.drawPixel(x - 1, y, color);
  tft.drawPixel(x, y, color);
  tft.drawPixel(x + 1, y, color);
  tft.drawPixel(x + 2, y, color);

  tft.drawPixel(x - 1, y + 1, color);
  tft.drawPixel(x, y + 1, color);
  tft.drawPixel(x + 1, y + 1, color);

  tft.drawPixel(x, y + 2, color);
  tft.drawPixel(x , y + 3, color);
  tft.drawPixel(x, y + 4, color);


}

///////////////////////////////////////////////////////GLOANTE/////////////////////////////////////
void drawProjectile(int16_t x, int16_t y, uint16_t color) {
  tft.drawCircle(x, y, projectileSize, color);
  tft.fillCircle(x, y, projectileSize, color);
}

int validMove(Projectile bullet) {
  if (bullet.y - moveDistance > 0) // este inca in mapa
    return 0;
  if (bullet.y - moveDistance < 0) // a iesit din mapa
    return 1;
}

void shoot() {
  Projectile bullet = Projectile(bulletCounter, xNow, yNow - moveDistance - 3);
  Add(bullet);
}

void projectileMove() {
  for (int i = 0; i < bulletCounter; i++)
  {
    if (validMove(bullets[i]) == 0) {
      drawProjectile(bullets[i].x, bullets[i].y, ST7735_BLACK);
      bullets[i].y -= moveDistance;
      drawProjectile(bullets[i].x, bullets[i].y, ST7735_WHITE);
    }
    if (validMove(bullets[i]) == 1) {
      drawProjectile(bullets[i].x, bullets[i].y, ST7735_BLACK);
      Remove(bullets[i]);
      i--;
    }
  }
}

//////////////////////////////////////////////////////vector gloante///////////////////////////////////////
void Remove(Projectile bullet) {

  for (int i = bullet.id; i < maxBullets - 1; i++) {
    bullets[i] = bullets[i + 1];
    bullets[i].id = i;
  }
  bulletCounter--;
}
void Add(Projectile newBullet) {
  bullets[bulletCounter++] = newBullet;
}
