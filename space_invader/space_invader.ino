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
enum stateT {idle, nextLevel, play, gameOver, victory};
boolean inUse = false;
stateT state = idle;

//vector de gloante
const byte maxBullet = 50;
Point bullet[maxBullet];
byte bulletCounter = 0;

// vector de inamici
const byte maxEnemy = 30;
Point enemy[maxEnemy];
byte enemyCounter = 0;
byte enemyAlive = 0;
Point enemyBullet[maxBullet];
byte enemyBulletCounter = 0;

//temporizare
Metro shootT = Metro(1000);
Metro projectileMoveT = Metro(100);
Metro enemyShootT = Metro(1500);
Metro enemyProjectileMoveT = Metro(100);
Metro enemyBodyMoveT = Metro(750);

/////////////////////////////////////////////////////////////// VARIABILE AFISAJ ///////////////////////////////////////////////////////////////
// constrante
const byte projectileSize = 1;
const byte moveDistance = 5;
const byte enemyMoveDistance = 2;
const byte enemyHeight = 5;
const byte enemyWidth = 6;

// variabile
byte xNow = 64;
byte yNow = 150;
byte currentLevel = 1;

void setup() {
  Serial.begin(9600);
  tft.initR(INITR_BLACKTAB);
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    switch (key) {
      case '4': if (state == play) moveLeft(); break;
      case '6': if (state == play) moveRight(); break;
      case '1': if (state == idle) {
          state = nextLevel;
          inUse = false;
        } break;
      default: Serial.println("DK which button you pressed");
    }
  }


  switch (state) {
    case idle:
      if (inUse == false) {
        startMenu();
        inUse = true;
      }
      break;

    case nextLevel:
      nextLevelAnimation(currentLevel);
      state = play;
      if (currentLevel > 1)
        inUse = false;

      break;

    case play:
      if (inUse == false) {
        Serial.println(currentLevel);
        inUse = true;
        tft.fillScreen(ST7735_WHITE);
        spawnEnemies(currentLevel);
        xNow = 64;
        yNow = 150;
        drawAircraft(xNow, yNow, ST7735_BLACK, 0);
      }
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

  if (shootT.check() && state == play) {
    shoot();
  }
  if (projectileMoveT.check() && state == play) {
    projectileMove();
  }
  if (enemyShootT.check() && state == play) {
    enemyShoot();
  }
  if (enemyProjectileMoveT.check() && state == play) {
    enemyProjectileMove();
  }
  if (enemyBodyMoveT.check() && state == play) {
    enemyBodyMove();
  }

}

/////////////////////////////////////////////////////////////// AFISAJ TFT ///////////////////////////////////////////////////////////////
void startMenu() {
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(10, 20);
  tft.setTextSize(2);
  tft.setTextColor(ST7735_YELLOW);
  tft.setTextWrap(true);
  tft.print("  SPACE    INVADER");

  tft.setCursor(10, 80);
  tft.setTextSize(1);
  tft.print("To start the game        press '1' ");

  tft.setCursor(10, 140);
  tft.print("by Cojocaru Eugeniu");
}

void nextLevelAnimation(byte level) {
  tft.fillScreen(ST7735_WHITE);
  tft.setCursor(0, 20);
  tft.setTextSize(2);
  tft.setTextColor(ST7735_BLUE);
  tft.setTextWrap(true);
  tft.print("   LEVEL       ");
  tft.print(level);
  yNow = 150;
  while (yNow > 0)
  {
    drawAircraft(xNow, yNow, ST7735_YELLOW, 1);
    yNow -= 2;
    drawAircraft(xNow, yNow, ST7735_BLACK, 0);
    delay(30);
  }
  drawAircraft(xNow, yNow, ST7735_YELLOW, 1);
}

void GAMEOVER() {
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(10, 50);
  tft.setTextSize(2);
  tft.setTextColor(ST7735_YELLOW);
  tft.setTextWrap(true);
  tft.print("GAME OVER");
  tft.setCursor(10, 90);
  tft.setTextSize(1);
  tft.print("Press '1' to retry");
  drawAircraft(xNow, yNow, ST7735_WHITE, 0);
  RESET(1);
}

void VICTORY() {
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(10, 50);
  tft.setTextSize(2);
  tft.setTextColor(ST7735_YELLOW);
  tft.setTextWrap(true);
  tft.print(" VICTORY!");
  tft.setCursor(10, 90);
  tft.setTextSize(1);
  tft.print("  To play again          press '1'");
  drawAircraft(xNow, yNow, ST7735_WHITE, 0);
  RESET(1);
}

void moveLeft() {
  drawAircraft(xNow, yNow, ST7735_WHITE, 1);
  if (xNow - moveDistance < 0) {
    xNow = 128;
  }
  xNow -= moveDistance;
  drawAircraft(xNow, yNow, ST7735_BLACK, 0);
}

void moveRight() {
  drawAircraft(xNow, yNow, ST7735_WHITE, 1);
  if (xNow + moveDistance > 128) {
    xNow = 0;
  }
  xNow += moveDistance;
  drawAircraft(xNow, yNow, ST7735_BLACK, 0);
}

void drawAircraft(byte x, byte y, uint16_t color, byte flag) {
  uint16_t color1 = ST7735_CYAN;
  uint16_t color2 = ST7735_YELLOW;
  if (flag == 1) {
    color2 = color;
    color1 = color;
  }
  tft.drawPixel(x, y - 8, color1); //yNow-8
  tft.drawPixel(x, y - 7, color2); //yNow-7
  tft.drawPixel(x, y - 6, color); //yNow-6
  tft.drawPixel(x - 1, y - 5, color);
  tft.drawPixel(x, y - 5, color); //yNow-5
  tft.drawPixel(x + 1, y - 5, color);
  tft.drawPixel(x - 1, y - 4, color);
  tft.drawPixel(x, y - 4, color); //yNow-4
  tft.drawPixel(x + 1, y - 4, color);
  tft.drawPixel(x - 4, y - 3, color1);
  tft.drawPixel(x - 1, y - 3, color);
  tft.drawPixel(x, y - 3, color); //yNow -3
  tft.drawPixel(x + 1, y - 3, color);
  tft.drawPixel(x + 4, y - 3, color1);
  tft.drawPixel(x - 4, y - 2, color2);
  tft.drawPixel(x - 1, y - 2, color);
  tft.drawPixel(x, y - 2, color); //yNow -2
  tft.drawPixel(x + 1, y - 2, color);
  tft.drawPixel(x + 4, y - 2, color2);
  tft.drawPixel(x - 4, y - 1, color);
  tft.drawPixel(x - 2, y - 1, color);
  tft.drawPixel(x - 1, y - 1, color);
  tft.drawPixel(x, y - 1, color); //yNow-1
  tft.drawPixel(x + 1, y - 1, color);
  tft.drawPixel(x + 2, y - 1, color);
  tft.drawPixel(x + 4, y - 1, color);
  tft.drawPixel(x - 7, y, color2);
  tft.drawPixel(x - 4, y, color);
  tft.drawPixel(x - 3, y, color2);
  tft.drawPixel(x - 2, y, color);
  tft.drawPixel(x - 1, y, color);
  tft.drawPixel(x, y, color2); //yNow
  tft.drawPixel(x + 1, y, color);
  tft.drawPixel(x + 2, y, color);
  tft.drawPixel(x + 3, y, color2);
  tft.drawPixel(x + 4, y, color);
  tft.drawPixel(x + 7, y, color2);
  tft.drawPixel(x - 7, y + 1, color2);
  tft.drawPixel(x - 4, y + 1, color1);
  tft.drawPixel(x - 3, y + 1, color);
  tft.drawPixel(x - 2, y + 1, color);
  tft.drawPixel(x - 1, y + 1, color1);
  tft.drawPixel(x, y + 1, color1); //yNow+1
  tft.drawPixel(x + 1, y + 1, color1);
  tft.drawPixel(x + 2, y + 1, color);
  tft.drawPixel(x + 3, y + 1, color);
  tft.drawPixel(x + 4, y + 1, color1);
  tft.drawPixel(x + 7, y + 1, color2);
  tft.drawPixel(x - 7, y + 2, color);
  tft.drawPixel(x - 4, y + 2, color);
  tft.drawPixel(x - 3, y + 2, color);
  tft.drawPixel(x - 2, y + 2, color);
  tft.drawPixel(x - 1, y + 2, color1);
  tft.drawPixel(x, y + 2, color); //yNow+2
  tft.drawPixel(x + 1, y + 2, color1);
  tft.drawPixel(x + 2, y + 2, color);
  tft.drawPixel(x + 3, y + 2, color);
  tft.drawPixel(x + 4, y + 2, color);
  tft.drawPixel(x + 7, y + 2, color);
  tft.drawPixel(x - 7, y + 3, color);
  tft.drawPixel(x - 5, y + 3, color);
  tft.drawPixel(x - 4, y + 3, color);
  tft.drawPixel(x - 3, y + 3, color);
  tft.drawPixel(x - 2, y + 3, color);
  tft.drawPixel(x - 1, y + 3, color);
  tft.drawPixel(x, y + 3, color); //yNow+3
  tft.drawPixel(x + 1, y + 3, color);
  tft.drawPixel(x + 2, y + 3, color);
  tft.drawPixel(x + 3, y + 3, color);
  tft.drawPixel(x + 4, y + 3, color);
  tft.drawPixel(x + 5, y + 3, color);
  tft.drawPixel(x + 7, y + 3, color);
  tft.drawPixel(x - 7, y + 4, color);
  tft.drawPixel(x - 6, y + 4, color);
  tft.drawPixel(x - 5, y + 4, color);
  tft.drawPixel(x - 4, y + 4, color);
  tft.drawPixel(x - 3, y + 4, color);
  tft.drawPixel(x - 2, y + 4, color2);
  tft.drawPixel(x - 1, y + 4, color);
  tft.drawPixel(x, y + 4, color); //yNow+4
  tft.drawPixel(x + 1, y + 4, color);
  tft.drawPixel(x + 2, y + 4, color2);
  tft.drawPixel(x + 3, y + 4, color);
  tft.drawPixel(x + 4, y + 4, color);
  tft.drawPixel(x + 5, y + 4, color);
  tft.drawPixel(x + 6, y + 4, color);
  tft.drawPixel(x + 7, y + 4, color);
  tft.drawPixel(x - 7, y + 5, color);
  tft.drawPixel(x - 6, y + 5, color);
  tft.drawPixel(x - 5, y + 5, color);
  tft.drawPixel(x - 3, y + 5, color2);
  tft.drawPixel(x - 2, y + 5, color2);
  tft.drawPixel(x - 1, y + 5, color);
  tft.drawPixel(x, y + 5, color); //yNow+5
  tft.drawPixel(x + 1, y + 5, color);
  tft.drawPixel(x + 2, y + 5, color2);
  tft.drawPixel(x + 3, y + 5, color2);
  tft.drawPixel(x + 5, y + 5, color);
  tft.drawPixel(x + 6, y + 5, color);
  tft.drawPixel(x + 7, y + 5, color);
  tft.drawPixel(x - 7, y + 6, color);
  tft.drawPixel(x - 6, y + 6, color);
  tft.drawPixel(x - 3, y + 6, color2);
  tft.drawPixel(x - 2, y + 6, color1);
  tft.drawPixel(x, y + 6, color); //yNow+6
  tft.drawPixel(x + 2, y + 6, color1);
  tft.drawPixel(x + 3, y + 6, color2);
  tft.drawPixel(x + 6, y + 6, color);
  tft.drawPixel(x + 7, y + 6, color);
  tft.drawPixel(x - 7, y + 7, color);
  tft.drawPixel(x - 3, y + 7, color1);
  tft.drawPixel(x, y + 7, color); //yNow+7
  tft.drawPixel(x + 3, y + 7, color1);
  tft.drawPixel(x + 7, y + 7, color);
}

void drawProjectile(byte x, byte y, uint16_t color) {
  tft.drawCircle(x, y, projectileSize, color);
  tft.fillCircle(x, y, projectileSize, color);
}

void drawEnemy(byte x, byte y, byte width, byte height, uint16_t color) {
  tft.drawRect(x, y, width, height, color);
  tft.fillRect(x, y, width, height, color);
}

void drawEnemies() {
  for (byte i = 0 ; i < enemyCounter; i++) {
    if (enemy[i].id == 0) {
      tft.drawRect(enemy[i].x, enemy[i].y, enemyWidth, enemyHeight, ST7735_WHITE);
      tft.fillRect(enemy[i].x, enemy[i].y, enemyWidth, enemyHeight, ST7735_WHITE);
      enemy[i].id = -1;
    }
  }
}

/////////////////////////////////////////////////////////////// GLOANTE PLAYER FUNCTIONALITATI ///////////////////////////////////////////////////////////////
int validMove(Point bullet) {
  if (bullet.y > 0) { // este inca in mapa
    for (byte i = 0; i < enemyCounter; i++) {
      if (enemy[i].id != 0 && enemy[i].id != -1) { // daca inca este desenat inamicul
        if (bullet.y >= enemy[i].y && ( bullet.y <= enemy[i].y + enemyHeight) && bullet.x + 1 >= enemy[i].x && ( bullet.x - 1 <= enemy[i].x + enemyWidth)) {
          enemy[i].id = 0;
          enemyAlive--;
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
  Point bullet = Point(bulletCounter, xNow, yNow - 10);
  Add(bullet, 0);
}

void projectileMove() {
  for (byte i = 0; i < bulletCounter; i++)
  {
    byte stateP = validMove(bullet[i]);
    if (stateP == 0) {
      drawProjectile(bullet[i].x, bullet[i].y, ST7735_WHITE);
      bullet[i].y -= moveDistance;
      drawProjectile(bullet[i].x, bullet[i].y, ST7735_BLUE);
    }
    if (stateP == 1) {
      drawProjectile(bullet[i].x, bullet[i].y, ST7735_WHITE);
      Remove(bullet[i], 0);
      i--;
    }
    if (stateP == 2) {
      drawProjectile(bullet[i].x, bullet[i].y, ST7735_WHITE);
      Remove(bullet[i], 0);
      i--;
      drawEnemies();
      if (enemyAlive == 0)
      {
        if (currentLevel == 5) {
          RESET(1);
          state = victory;
        }
        else {
          RESET(0);
          currentLevel++;
          state = nextLevel;
        }
      }
    }
  }
}

/////////////////////////////////////////////////////////////// INAMICI FUNCTIONALITATE ///////////////////////////////////////////////////////////////
void spawnEnemies(byte level) {
  byte posX = 6;
  byte posY = 6;
  byte numberEnemy = level * 6;
  enemyAlive = numberEnemy;
  byte xPosition = 0;
  byte id = 0; // imi spune daca inamicul se spawneaza mai in stanga sau mia in dreapta
  for (byte i = 0 ; i < numberEnemy; i++) {
    byte chance = 1 + rand() % 10;
    if (chance <= 5) { //////////////////////////////////////////////////////////////////////////////// poti da id-ul sa fie random ca sa se spawneze si mai diferit
      id = 10;
      xPosition = 0;
    }
    else {
      xPosition = 10;
      id = 15;
    }
    Point newEnemy = Point(id, posX + xPosition + (i % 6) * 20, posY + (i / 6) * (enemyHeight + 4));
    Add(newEnemy, 1);
    tft.drawRect(newEnemy.x, newEnemy.y, enemyWidth, enemyHeight, ST7735_BLACK);
    tft.fillRect(newEnemy.x, newEnemy.y, enemyWidth, enemyHeight, ST7735_BLACK);
  }
}

int enemyBulletValidMove(Point bullet) {
  if (bullet.y < 160) { // este inca in mapa
    if (bullet.y == yNow - 8 || bullet.y == yNow - 7 || bullet.y == yNow - 6) {
      if (bullet.x == xNow || bullet.x - 1 == xNow || bullet.x + 1 == xNow) {
        Serial.println("GAME OVER!");
        return 2;
      }
    }
    if (bullet.y == yNow - 5 || bullet.y == yNow - 4) {
      if (bullet.x - 1 >= xNow - 1 && bullet.x + 1 <= xNow + 1) {
        Serial.println("GAME OVER!");
        return 2;
      }
    }
    if (bullet.y == yNow - 3 || bullet.y == yNow - 2 || bullet.y == yNow - 1) {
      if (bullet.x - 1 >= xNow - 4 && bullet.x + 1 <= xNow + 4) {
        Serial.println("GAME OVER!");
        return 2;
      }
    }

    if (bullet.y >= yNow && bullet.y <= yNow + 7) {
      if (bullet.x - 1 >= xNow - 7 && bullet.x + 1 <= xNow + 7) {
        Serial.println("GAME OVER!");
        return 2;
      }
    }
    return 0; // miscare valida inca in mapa
  }
  if (bullet.y >= 160) // a iesit din mapa
    return 1;
}

void enemyShoot() {
  for (byte i = 0; i < maxEnemy; i++) {
    if (enemy[i].id != 0 && enemy[i].id != -1 ) {
      byte chance = 1 + (rand() % 10); // sansa 50/50 de a trage un glont
      if (chance % 2 == 0) {
        chance = 1 + (rand() % enemyWidth); // pozitia de unde sa porneasca glontul
        Point bullet = Point(enemyBulletCounter, enemy[i].x + chance, enemy[i].y + moveDistance + 1); /////HARDCODED
        Add(bullet, 2);
      }
    }
  }
}

void enemyProjectileMove() {
  for (byte i = 0; i < enemyBulletCounter; i++)
  {
    byte stateE = enemyBulletValidMove(enemyBullet[i]);
    if (stateE == 0) {
      drawProjectile(enemyBullet[i].x, enemyBullet[i].y, ST7735_WHITE);
      enemyBullet[i].y += moveDistance;
      drawProjectile(enemyBullet[i].x, enemyBullet[i].y, ST7735_RED);
    }
    if (stateE == 1) {
      drawProjectile(enemyBullet[i].x, enemyBullet[i].y, ST7735_WHITE);
      Remove(enemyBullet[i], 1);
      i--;
    }
    if (stateE == 2) {
      Remove(enemyBullet[i], 1);
      i--;
      state = gameOver;

    }
  }
}
int enemyValidMove(Point enemyBody) {
  if (enemyBody.id >= 10 && enemyBody.id < 15)
    return 0;
  if (enemyBody.id >= 15 && enemyBody.id < 20)
    return 1;
  if ( enemyBody.id == 20) {
    return 2;
  }
}
void enemyBodyMove() {
  for (byte i = 0; i < enemyCounter; i++) {
    if (enemy[i].id != 0 && enemy[i].id != -1) {
      byte state = enemyValidMove(enemy[i]);
      switch (state) {
        case 0:
          drawEnemy(enemy[i].x, enemy[i].y, enemyWidth, enemyHeight, ST7735_WHITE);
          enemy[i].x += enemyMoveDistance;
          drawEnemy(enemy[i].x, enemy[i].y, enemyWidth, enemyHeight, ST7735_BLACK);
          enemy[i].id++;
          break;
        case 1:
          drawEnemy(enemy[i].x, enemy[i].y, enemyWidth, enemyHeight, ST7735_WHITE);
          enemy[i].x -= enemyMoveDistance;
          drawEnemy(enemy[i].x, enemy[i].y, enemyWidth, enemyHeight, ST7735_BLACK);
          enemy[i].id++;
          break;
        case 2:
          drawEnemy(enemy[i].x, enemy[i].y, enemyWidth, enemyHeight, ST7735_WHITE);
          enemy[i].x += enemyMoveDistance;
          drawEnemy(enemy[i].x, enemy[i].y, enemyWidth, enemyHeight, ST7735_BLACK);
          enemy[i].id = 11;
          break;
      }
    }
  }
}


/////////////////////////////////////////////////////////////// OPERATII VECTOR GLOANTE ///////////////////////////////////////////////////////////////
void Remove(Point mybullet, byte vector) {

  if (vector == 0) {
    for (byte i = mybullet.id; i < maxBullet - 1; i++) {
      bullet[i] = bullet[i + 1];
      bullet[i].id = i;
    }
    bulletCounter--;
  }
  else {
    if (vector == 1) {
      for (byte i = mybullet.id; i < maxBullet - 1; i++) {
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
void Add(Point point, byte flag) {
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
void RESET(byte flag) {//flag==1 reset total, 0 pentru alt level
  enemyCounter = 0;
  enemyBulletCounter = 0;
  bulletCounter = 0;
  if (flag == 1)
    currentLevel = 1;
}
