#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Keypad.h>
#include <Metro.h> // librarie temporizare
#include "Point.h" //librarie pentru tipul punct scrisa de mine

/////////////////////////////////////////////////////////////// HARDWARE ///////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////// SOFTWARE ///////////////////////////////////////////////////////////////
//vector de gloante
const byte maxBullet = 50;
Point bullet[maxBullet];
byte bulletCounter = 0;

// vector de inamici
const byte maxEnemy = 24;
Point enemy[maxEnemy];
byte enemyCounter = 0;
Point enemyBullet[maxBullet];
byte enemyBulletCounter;

//temporizare
Metro shootT = Metro(1000);
Metro projectileMoveT = Metro(100);
Metro enemyShootT = Metro(750);
Metro enemyProjectileMoveT = Metro(75);
//boolean shootState = false;
//boolean projectileMoveState = false;

/////////////////////////////////////////////////////////////// VARIABILE AFISAJ ///////////////////////////////////////////////////////////////
// constrante
const byte projectileSize = 1;
const byte moveDistance = 5;

// variabile
int16_t xNow = 64;
int16_t yNow = 150;
byte currentLevel = 1;

//start game
boolean start = false;
boolean nextLevel = false;
boolean gameOver = false;


void setup() {

  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST7735_BLACK);
  Serial.begin(9600);
  drawAircraft(xNow, yNow, ST7735_WHITE);
  spawnEnemies(currentLevel);

}

void loop() {


  if (gameOver == false) {
    if (shootT.check()) {
      //shootState = !shootState;
      shoot();
    }
    if (projectileMoveT.check()) {
      //projectileMoveState = !projectileMoveState;
      projectileMove();
    }
    if (enemyShootT.check()) {
      enemyShoot();
    }
    if (enemyProjectileMoveT.check()) {
      enemyProjectileMove();
    }
  }
  char key = keypad.getKey();
  if (key) {
    Serial.println(key);

    switch (key) {
      case '4': moveLeft(); break;
      case '6': moveRight(); break;
      case '1': start = !start; break;
      default: Serial.println("DK which button you pressed");
    }
  }

}

/////////////////////////////////////////////////////////////// AFISAJ TFT ///////////////////////////////////////////////////////////////
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

void drawProjectile(int16_t x, int16_t y, uint16_t color) {
  tft.drawCircle(x, y, projectileSize, color);
  tft.fillCircle(x, y, projectileSize, color);
}

void drawEnemies() {
  int posX = 6;
  int posY = 6;

  for (int i = 0 ; i < enemyCounter; i++) {
    if (enemy[i].id == 0)
    {
      tft.drawRect(posX + i * 20, posY, 16, 5, ST7735_BLACK);
      tft.fillRect(posX + i * 20, posY, 16, 5, ST7735_BLACK);
      enemy[i].id = -1;
    }
  }
}

void GAMEOVER(){
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(10, 50);
  tft.setTextSize(2);
  tft.setTextColor(ST7735_CYAN);
  tft.setTextWrap(true);
  tft.print("GAME OVER");
  drawAircraft(xNow, yNow, ST7735_CYAN);
}

/////////////////////////////////////////////////////////////// GLOANTE FUNCTIONALITATI ///////////////////////////////////////////////////////////////
int validMove(Point bullet) {
  if (bullet.y - moveDistance > 0) { // este inca in mapa
    for (int i = 0; i < enemyCounter; i++) {
      if (enemy[i].id == 1) { // daca inca este desenat inamicul
        if (bullet.y - moveDistance >= enemy[i].y && ( bullet.y - moveDistance <= enemy[i].y + 5) && bullet.x >= enemy[i].x && ( bullet.x <= enemy[i].x + 16)) {
          enemy[i].id = 0;
          return 2; //s-a eliminat un inamic
        }
      }
    }
    return 0; // miscare valida inca in mapa
  }
  if (bullet.y - moveDistance < 0) // a iesit din mapa
    return 1;
}

void shoot() {
  Point bullet = Point(bulletCounter, xNow, yNow - moveDistance - 3);
  Add(bullet, 0);
}

void projectileMove() {
  for (short i = 0; i < bulletCounter; i++)
  {
    int state = validMove(bullet[i]);
    if (state == 0) {
      drawProjectile(bullet[i].x, bullet[i].y, ST7735_BLACK);
      bullet[i].y -= moveDistance;
      drawProjectile(bullet[i].x, bullet[i].y, ST7735_WHITE);
    }
    if (state == 1) {
      drawProjectile(bullet[i].x, bullet[i].y, ST7735_BLACK);
      Remove(bullet[i], 0);
      i--;
    }
    if (state == 2) {
      drawProjectile(bullet[i].x, bullet[i].y, ST7735_BLACK);
      Remove(bullet[i], 0);
      i--;
      drawEnemies();
    }
  }
}

/////////////////////////////////////////////////////////////// INAMICI FUNCTIONALITATE ///////////////////////////////////////////////////////////////
void spawnEnemies(int level) {
  int posX = 6;
  int posY = 6;
  if (level == 1)
  {
    for (int i = 0 ; i < 6; i++) {
      Point newEnemy = Point(1, posX + i * 20, posY);
      Add(newEnemy, 1);
      tft.drawRect(newEnemy.x, newEnemy.y, 16, 5, ST7735_WHITE);
      tft.fillRect(newEnemy.x, newEnemy.y, 16, 5, ST7735_WHITE);
    }
  }
}

int enemyValidMove(Point bullet) {
  if (bullet.y + moveDistance < 160) { // este inca in mapa
    short bulletY = bullet.y + moveDistance;
    
    
    if (bulletY >= yNow - moveDistance && bulletY <= yNow + moveDistance  && bullet.x >= xNow - moveDistance && bullet.x <= xNow + moveDistance) {
      Serial.println("GAME OVER!");
      return 2; //GAME OVER
    }
    return 0; // miscare valida inca in mapa
  }
  if (bullet.y + moveDistance >= 160) // a iesit din mapa
    return 1;
}

void enemyShoot() {
  for (int i = 0; i < maxEnemy; i++) {
    if (enemy[i].id == 1) {
      byte chance = 1 + (rand() % 10);
      if (chance % 2 == 0) {
        chance = 1 + (rand() % 16);
        Point bullet = Point(enemyBulletCounter, enemy[i].x + chance, enemy[i].y + moveDistance); /////HARDCODED
        Add(bullet, 2);
      }
    }
  }
}

void enemyProjectileMove() {
  for (short i = 0; i < enemyBulletCounter; i++)
  {
    int state = enemyValidMove(enemyBullet[i]);
    if (state == 0) {
      drawProjectile(enemyBullet[i].x, enemyBullet[i].y, ST7735_BLACK);
      enemyBullet[i].y += moveDistance;
      drawProjectile(enemyBullet[i].x, enemyBullet[i].y, ST7735_WHITE);
    }
    if (state == 1) {
      drawProjectile(enemyBullet[i].x, enemyBullet[i].y, ST7735_BLACK);
      Remove(enemyBullet[i], 1);
      i--;
    }
    if (state == 2) {
      gameOver=true;
      GAMEOVER();
      //      drawProjectile(bullet[i].x, bullet[i].y, ST7735_BLACK);
//      Remove(enemyBullet[i], 1);
//      i--;
      
    }
  }
}



/////////////////////////////////////////////////////////////// OPERATII VECTOR GLOANTE ///////////////////////////////////////////////////////////////
void Remove(Point mybullet, byte vector) {

  if (vector == 0) {
    for (short i = mybullet.id; i < maxBullet - 1; i++) {
      bullet[i] = bullet[i + 1];
      bullet[i].id = i;
    }
    bulletCounter--;
  }
  else {
    if (vector == 1) {
      for (short i = mybullet.id; i < maxBullet - 1; i++) {
        enemyBullet[i] = enemyBullet[i + 1];
        enemyBullet[i].id = i;
      }
      enemyBulletCounter--;
    }
  }

}
/*** functie care adauga un punct intr-un anumit vector in functie de flag
   point= punctul de adaugat
   flag= factorul de decizie: 0=bullet/1=enemy/2=enemyBullet
*/
void Add(Point point, short flag) {
  if (flag == 0) {
    if (bulletCounter < maxBullet) {
      bullet[bulletCounter++] = point;
    }
  }
  else {
    if (flag == 1)
    {
      if (enemyCounter < maxEnemy) {
        enemy[enemyCounter++] = point;
      }

    }
    else {
      if (enemyBulletCounter < maxBullet) {
        //Serial.println(enemyBulletCounter);
        enemyBullet[enemyBulletCounter++] = point;
      }
    }
  }

}
