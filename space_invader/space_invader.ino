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
// stari
enum stateT {idle, nextLevel, level, gameOver, victory};
boolean inUse=false;
stateT state = idle;

//culori
struct RGB {
  short r, g, b;
};
RGB bgColor, enemyColor, enemyBulletColor, aircraftColor, aircraftBulletColor;

//vector de gloante
const byte maxBullet = 50;
Point bullet[maxBullet];
byte bulletCounter = 0;

// vector de inamici
const byte maxEnemy = 30;
Point enemy[maxEnemy];
byte enemyCounter = 0;
byte enemyAlive;
Point enemyBullet[maxBullet];
byte enemyBulletCounter = 0;

//temporizare
Metro shootT = Metro(1000);
Metro projectileMoveT = Metro(100);
Metro enemyShootT = Metro(750);
Metro enemyProjectileMoveT = Metro(75);
Metro enemyBodyMoveT = Metro(750);

/////////////////////////////////////////////////////////////// VARIABILE AFISAJ ///////////////////////////////////////////////////////////////
// constrante
const byte projectileSize = 1;
const byte moveDistance = 5;
const byte enemyMoveDistance = 2;
const byte enemyHeight = 5;
const byte enemyWidth = 6;

// variabile
int16_t xNow = 64;
int16_t yNow = 150;
byte currentLevel = 1;

//start game
boolean start = false;
//boolean nextLevel = false;
//boolean gameOver = false;


void setup() {
  Serial.begin(9600);
  tft.initR(INITR_BLACKTAB);

  //  tft.fillScreen(ST7735_BLACK);
  //
  //  //delay(2000);
  //  drawAircraft(xNow, yNow, ST7735_WHITE);
  //  spawnEnemies(currentLevel);



}

void loop() {
  
  char key = keypad.getKey();
  
  if (key) {
    switch (key) {
      case '4': moveLeft(); break;
      case '6': moveRight(); break;
      case '1': start = !start; break;
      default: Serial.println("DK which button you pressed");
    }
  }
  
  switch (state) {
    case idle:
      if(inUse == false){
        startMenu();
        inUse=!inUse; 
      }
      break;
    case nextLevel:
      nextLevelAnimation();
      state = level;
      break;
    case level:
      spawnEnemies(currentLevel);
      break;
    case gameOver:
      GAMEOVER();
      state = idle;
      break;
    case victory:
      VICTORY();
      state = idle;
      break;
    default: break;
  }

  //  if (shootT.check()) {
  //    shoot();
  //  }
  //  if (projectileMoveT.check()) {
  //    projectileMove();
  //  }
  //  if (enemyShootT.check()) {
  //    enemyShoot();
  //  }
  //  if (enemyProjectileMoveT.check()) {
  //    enemyProjectileMove();
  //  }
  //  if (enemyBodyMoveT.check()) {
  //    enemyBodyMove();
  //  }
}

/////////////////////////////////////////////////////////////// AFISAJ TFT ///////////////////////////////////////////////////////////////
void startMenu() {
  tft.fillScreen(tft.color565(38, 188, 201));
  tft.setCursor(10, 20);
  tft.setTextSize(2);
  tft.setTextColor(tft.color565(12, 7, 166));
  tft.setTextWrap(true);
  tft.print("  SPACE    INVADER");

  tft.setCursor(10,80);
  tft.setTextSize(1);
  tft.print("To start the game        press '1' ");

  tft.setCursor(10,140);
  tft.print("by Cojocaru Eugeniu");
}

void nextLevelAnimation() {

}

void GAMEOVER() {
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(10, 50);
  tft.setTextSize(2);
  tft.setTextColor(ST7735_CYAN);
  tft.setTextWrap(true);
  tft.print("GAME OVER");
  drawAircraft(xNow, yNow, ST7735_CYAN);
}

void VICTORY() {

}
void selectColor(byte level) {
  switch (level) {
    case 1:
      bgColor.r = 255;
      bgColor.g = 255;
      bgColor.b = 255;

      enemyColor.r = 0;
      enemyColor.g = 0;
      enemyColor.b = 0;

      enemyBulletColor.r = 247;
      enemyBulletColor.r = 32;
      enemyBulletColor.r = 12;

      aircraftColor.r = 0;
      aircraftColor.r = 0;
      aircraftColor.r = 0;

      aircraftBulletColor.r = 0;
      aircraftBulletColor.r = 21;
      aircraftBulletColor.r = 255;

    //    case 2:
    //    case 3:
    //    case 4:
    //    case 5:
    default: break;
  }
}
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
void drawEnemy(int16_t x, int16_t y, int16_t width, int16_t height, uint16_t color) {
  tft.drawRect(x, y, width, height, color);
  tft.fillRect(x, y, width, height, color);
}

void drawEnemies() {
  for (int i = 0 ; i < enemyCounter; i++) {
    if (enemy[i].id == 0) {
      tft.drawRect(enemy[i].x, enemy[i].y, enemyWidth, enemyHeight, ST7735_BLACK);
      tft.fillRect(enemy[i].x, enemy[i].y, enemyWidth, enemyHeight, ST7735_BLACK);
      enemy[i].id = -1;
    }
  }
}



/////////////////////////////////////////////////////////////// GLOANTE PLAYER FUNCTIONALITATI ///////////////////////////////////////////////////////////////
int validMove(Point bullet) {
  if (bullet.y > 0) { // este inca in mapa
    for (int i = 0; i < enemyCounter; i++) {
      if (enemy[i].id != 0 && enemy[i].id != -1) { // daca inca este desenat inamicul
        if (bullet.y >= enemy[i].y && ( bullet.y <= enemy[i].y + enemyHeight) && bullet.x + 1 >= enemy[i].x && ( bullet.x - 1 <= enemy[i].x + enemyWidth)) {
          Serial.println("am ajuns!");
          enemy[i].id = 0;
          return 2; //s-a eliminat un inamic
        }
      }
    }
    return 0; // miscare valida inca in mapa
  }
  if (bullet.y < 0) // a iesit din mapa
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
      drawProjectile(bullet[i].x, bullet[i].y, ST7735_GREEN);
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
  byte numberEnemy = level * 6;
  byte xPosition = 0;
  byte id = 0;
  for (int i = 0 ; i < numberEnemy; i++) {
    int chance = 1 + rand() % 10;
    if (chance <= 5) {
      id = 10;
      xPosition = 0;
    }
    else {
      xPosition = 10;
      id = 15;
    }
    Point newEnemy = Point(id, posX + xPosition + (i % 6) * 20, posY + (i / 6) * (enemyHeight + 4));
    Add(newEnemy, 1);
    tft.drawRect(newEnemy.x, newEnemy.y, enemyWidth, enemyHeight, ST7735_WHITE);
    tft.fillRect(newEnemy.x, newEnemy.y, enemyWidth, enemyHeight, ST7735_WHITE);
  }
}

int enemyBulletValidMove(Point bullet) {
  if (bullet.y < 160) { // este inca in mapa
    short bulletY = bullet.y + moveDistance;
    if (bullet.y >= yNow - moveDistance && bulletY <= yNow + moveDistance  && bullet.x >= xNow - moveDistance && bullet.x <= xNow + moveDistance) {
      Serial.println("GAME OVER!");
      return 2; //GAME OVER
    }
    return 0; // miscare valida inca in mapa
  }
  if (bullet.y >= 160) // a iesit din mapa
    return 1;
}

void enemyShoot() {
  for (int i = 0; i < maxEnemy; i++) {
    if (enemy[i].id != 0 && enemy[i].id != -1 ) {
      byte chance = 1 + (rand() % 10); // sansa 50/50 de a trage un glont
      if (chance % 2 == 0) {
        chance = 1 + (rand() % enemyWidth); // pozitia de unde sa porneasca glontul
        Point bullet = Point(enemyBulletCounter, enemy[i].x + chance, enemy[i].y + moveDistance); /////HARDCODED
        Add(bullet, 2);
      }
    }
  }
}

void enemyProjectileMove() {
  for (short i = 0; i < enemyBulletCounter; i++)
  {
    int state = enemyBulletValidMove(enemyBullet[i]);
    if (state == 0) {
      drawProjectile(enemyBullet[i].x, enemyBullet[i].y, ST7735_BLACK);
      enemyBullet[i].y += moveDistance;
      drawProjectile(enemyBullet[i].x, enemyBullet[i].y, ST7735_RED);
    }
    if (state == 1) {
      drawProjectile(enemyBullet[i].x, enemyBullet[i].y, ST7735_BLACK);
      Remove(enemyBullet[i], 1);
      i--;
    }
    if (state == 2) {
      //gameOver = true;
      Remove(enemyBullet[i], 1);
      i--;
      //GAMEOVER();
    }
  }
}
int enemyValidMove(Point enemyBody) {
  if (enemyBody.id >= 10 && enemyBody.id < 15)
    return 0;
  if (enemyBody.id >= 15 && enemyBody.id < 20)
    return 1;
  if ( enemyBody.id == 20) {
    enemyBody.id = 10;
    return 2;
  }
}
void enemyBodyMove() {
  for (int i = 0; i < enemyCounter; i++) {
    if (enemy[i].id != 0 && enemy[i].id != -1) {
      int state = enemyValidMove(enemy[i]);
      switch (state) {
        case 0:
          drawEnemy(enemy[i].x, enemy[i].y, enemyWidth, enemyHeight, ST7735_BLACK);
          enemy[i].x += enemyMoveDistance;
          drawEnemy(enemy[i].x, enemy[i].y, enemyWidth, enemyHeight, ST7735_WHITE);
          enemy[i].id++;
          break;
        case 1:
          drawEnemy(enemy[i].x, enemy[i].y, enemyWidth, enemyHeight, ST7735_BLACK);
          enemy[i].x -= enemyMoveDistance;
          drawEnemy(enemy[i].x, enemy[i].y, enemyWidth, enemyHeight, ST7735_WHITE);
          enemy[i].id++;
          break;
        case 2:
          drawEnemy(enemy[i].x, enemy[i].y, enemyWidth, enemyHeight, ST7735_BLACK);
          enemy[i].x += enemyMoveDistance;
          drawEnemy(enemy[i].x, enemy[i].y, enemyWidth, enemyHeight, ST7735_WHITE);
          enemy[i].id = 11;
          break;
      }
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
        enemyBullet[enemyBulletCounter++] = point;
      }
    }
  }
}
/////////////////////////////////////////////////////////////// RESET ///////////////////////////////////////////////////////////////
void RESET() {
  enemyCounter = 0;
  enemyBulletCounter = 0;
  bulletCounter = 0;

  tft.fillScreen(ST7735_BLACK);




}
