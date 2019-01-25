/*
  Festlegeúng für for-Schleifen: im Kontext der ledMatrix heißen die Laufvariablen col und row
                               im Kontext des PixelArr heißen die Laufvariablen Stripe und Pixel

  Für die Arrays ledMatrix, currentTetromino und tetrominoBuffer gilt: name[spalte][zeile]
  current Tetromino  03 13 23 33
                   02 12 22 32
                   01 11 21 31
                   00 10 20 30

*/
//includes
#include <Adafruit_NeoPixel.h>

//Festlegen der Alias
//ledPinx: x beschreibt die Spalte der Tetrismatrix
#define ledPin0 23
#define ledPin1 25
#define ledPin2 27
#define ledPin3 29
#define ledPin4 31
#define ledPin5 33
#define ledPin6 35
#define ledPin7 37
#define ledPin8 39
#define ledPin9 41

//Buttons
#define buttonPinReset    10
#define buttonPinRotate   11
#define buttonPinDefault  12

//Joystick1
#define joyStickUp1
#define joyStickDown1
#define joyStickLeft1
#define joyStickRight1

//Joystick2
#define joyStickUp2
#define joyStickDown2
#define joyStickLeft2
#define joyStickRight2


#define rotate  1
#define left    2
#define right   3
#define fall    4

int rotation = 0;

Adafruit_NeoPixel pixelArr[10];
uint32_t landedArr[10][20];
uint32_t ledMatrix[10][20];


//Farbdeklerationen
//Umwandlung von RGB - Wert in 32 Bit Integer ist eine Funktion der Adafruit_NeoPixel-Objekte --> das Objekt des ersten Strips wird verwendent
uint32_t black = pixelArr[0].Color(0, 0, 0);
uint32_t red = pixelArr[0].Color(255, 0, 0);
uint32_t green = pixelArr[0].Color(0, 255, 0);
uint32_t darkblue = pixelArr[0].Color(0, 0, 255);
uint32_t lightblue = pixelArr[0].Color(0, 255, 255);
uint32_t yellow = pixelArr[0].Color(255, 255, 0);
uint32_t orange = pixelArr[0].Color(255, 100, 0);
uint32_t purple = pixelArr[0].Color(255, 0, 255);

//Tetrominos
uint32_t currentTetromino[4][4];
uint32_t tetrominoBuffer[4][4];
int currentX;
int currentY;
int nextType = 0;
int nextRotation = 0;

void setup()
{
  //Definition der Pins als Out oder Input
  pinMode(ledPin0, OUTPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(ledPin4, OUTPUT);
  pinMode(ledPin5, OUTPUT);
  pinMode(ledPin6, OUTPUT);
  pinMode(ledPin7, OUTPUT);
  pinMode(ledPin8, OUTPUT);
  pinMode(ledPin9, OUTPUT);

  pinMode(buttonPinReset, INPUT_PULLUP);
  pinMode(buttonPinRotate, INPUT_PULLUP);
  pinMode(buttonPinDefault, INPUT_PULLUP);

  Serial.begin(9600);
  Serial.println("Start");

  //Initialisierung des Pixelarry

  pixelArr[0] = Adafruit_NeoPixel(20, ledPin0, NEO_GRB + NEO_KHZ800);
  pixelArr[1] = Adafruit_NeoPixel(20, ledPin1, NEO_GRB + NEO_KHZ800);
  pixelArr[2] = Adafruit_NeoPixel(20, ledPin2, NEO_GRB + NEO_KHZ800);
  pixelArr[3] = Adafruit_NeoPixel(20, ledPin3, NEO_GRB + NEO_KHZ800);
  pixelArr[4] = Adafruit_NeoPixel(20, ledPin4, NEO_GRB + NEO_KHZ800);
  pixelArr[5] = Adafruit_NeoPixel(20, ledPin5, NEO_GRB + NEO_KHZ800);
  pixelArr[6] = Adafruit_NeoPixel(20, ledPin6, NEO_GRB + NEO_KHZ800);
  pixelArr[7] = Adafruit_NeoPixel(20, ledPin7, NEO_GRB + NEO_KHZ800);
  pixelArr[8] = Adafruit_NeoPixel(20, ledPin8, NEO_GRB + NEO_KHZ800);
  pixelArr[9] = Adafruit_NeoPixel(20, ledPin9, NEO_GRB + NEO_KHZ800);

  Serial.println("Stripes instanziert");

  //Starten der LED Stripes
  for (int stripe = 0; stripe < 10; stripe++) {
    pixelArr[stripe].begin();
    pixelArr[stripe].setBrightness(32);
    for (int pixel = 0; pixel < 20; pixel++) {
      pixelArr[stripe].setPixelColor(pixel, black);
    }
    pixelArr[stripe].show();
  };
  Serial.println("Stripes gestartet und auf schwaz gesetzt");

  //Initiales Tetromino
  nextType = random(0, 7);
  nextRotation = random(0, 4);
  generateTetromino(nextType, nextRotation);

  //Timer
  TCCR1A = 0;
  TCCR1B = 0;
  TIMSK1 |= (1 << TOIE1); // Timer Overflow aktivieren

  Serial.println("Timer konfiguriert");
}

ISR(TIMER1_OVF_vect) {
  lowerCurrentTetromino();
}

void loop()
{
  //Serial.println("Beginn der loop");

}

void wipeScreen()
{
  Serial.println("Wipe Screen");
  for (int col = 0; col < 10; col++) {
    for (int row = 0; row < 20; row++) {
      ledMatrix[col][row] = black;

    }
  }
}

void drawCurrentTetromino(int x, int y)
{
  Serial.println("DrawCurrentTetromino");
  Serial.println(x);
  Serial.println(y);
  for (int col = 0; col < 4; col++) {
    for (int row = 0; row < 4;  row++) {
      if (currentTetromino[col][row] != 0) {
        ledMatrix[x + col][y + row] = currentTetromino[col][row];
      }
    }
  }
  setLeds();
}

void wipeCurrentTetromino(int x, int y) {
  Serial.println("WipeCurrentTetromino");
  for (int col = 0; col < 4; col++) {
    for (int row = 0; row < 4;  row++) {
      if (currentTetromino[col][row] != 0) {
        ledMatrix[x + col][y + row] = black;
      }
    }
  }
}

void lowerCurrentTetromino()
{
  Serial.println("lowerCurrentTetromino");
  if (!checkCollision(currentX, currentY - 1)) {//kann raus
    //löschen des "alten" Tetromino
    wipeCurrentTetromino(currentX, currentY);
    --currentY;
    drawCurrentTetromino(currentX, currentY);
  }
}


//bekommt GEPLANTE Koordinaten übergeben und prüft ob eine Kollision vorhanden wäre
//Gibt true zurück bei drohender Kollision
boolean checkCollision(int futureX, int futureY) {
  //TODO: Kollision mit anderen Tetrominos
  //TODO: Kollision bei Rotation ggf. in anderen Methode
  Serial.println("checkCollision");
  for (int col = 0; col < 4; col++) {
    for (int row = 0; row < 4; row++) {
      if (currentTetromino[col][row] != 0) {
        //checken ob am unteren Rand --> neues Tetromino generieren
        if ((futureY + row) < 0) {
          Serial.println("Kollison unten");
          nextType = random(0, 7);
          nextRotation = random(0, 4);
          generateTetromino(nextType, nextRotation);
          return true;
        }
        //checken ob am linken Rand
        if ((futureX + col) < 0) {
          Serial.println("Kollison links");
          return true;
        }
        //checken ob am rechten Rand
        if ((futureX + col) > 9) {
          Serial.println("Kollison rechts");
          return true;
        }
      }
    }
  }
  return false;
}


//Methode zum Anziegen auf der Led Matrix
void setLeds()
{
  Serial.println("Beginn Daten zu Strips");
  for (int stripe = 0; stripe < 10; stripe++) {
    for (int pixel = 0; pixel < 20; pixel++) {
      pixelArr[stripe].setPixelColor(pixel, ledMatrix[stripe][pixel]);
    };
    pixelArr[stripe].show();
  }
}
void generateTetromino(int type, int rotation)
{
  //TODO: neue Übergabeparameter x und y Koordiante, Abfrage ob y > 16 für Höhenkorrektur bei betroffenen Tetrominos
  //Achtung ganz am ende werden Momentan currentX und currentY hart gesetzt
  Serial.println("Beginn der generateTetromino");
  Serial.println(type);
  Serial.println(rotation);
  int nextTetromino = type * 4 + rotation;
  Serial.println(nextTetromino);
  //Wegen Debug
  //wipeScreen();

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      currentTetromino[i][j] = black;
    }
  }
  switch (nextTetromino) {
    //normal
    //xxxx
    //lightblue
    case 0:
      Serial.println("lightblue");
      currentTetromino[0][2] = lightblue;
      currentTetromino[1][2] = lightblue;
      currentTetromino[2][2] = lightblue;
      currentTetromino[3][2] = lightblue;
      break;

    //rotate 1
    case 1:
      Serial.println("lightblue");
      currentTetromino[2][0] = lightblue;
      currentTetromino[2][1] = lightblue;
      currentTetromino[2][2] = lightblue;
      currentTetromino[2][3] = lightblue;
      break;

    //rotate 2
    case 2:
      Serial.println("lightblue");
      currentTetromino[0][1] = lightblue;
      currentTetromino[1][1] = lightblue;
      currentTetromino[2][1] = lightblue;
      currentTetromino[3][1] = lightblue;
      break;

    //rotate 3
    case 3:
      Serial.println("lightblue");
      currentTetromino[1][3] = lightblue;
      currentTetromino[1][2] = lightblue;
      currentTetromino[1][1] = lightblue;
      currentTetromino[1][0] = lightblue;
      break;

    //normal
    //x
    //xxx
    //darkblue
    case 4:
      Serial.println("darkblue");
      currentTetromino[0][3] = darkblue;
      currentTetromino[0][2] = darkblue;
      currentTetromino[1][2] = darkblue;
      currentTetromino[2][2] = darkblue;
      break;

    //rotate 1
    case 5:
      Serial.println("darkblue");
      currentTetromino[2][3] = darkblue;
      currentTetromino[1][3] = darkblue;
      currentTetromino[1][2] = darkblue;
      currentTetromino[1][1] = darkblue;
      break;

    //rotate 2
    case 6:
      Serial.println("darkblue");
      currentTetromino[2][1] = darkblue;
      currentTetromino[2][2] = darkblue;
      currentTetromino[1][2] = darkblue;
      currentTetromino[0][2] = darkblue;
      break;

    //rotate 3
    case 7:
      Serial.println("darkblue");
      currentTetromino[1][3] = darkblue;
      currentTetromino[1][2] = darkblue;
      currentTetromino[1][1] = darkblue;
      currentTetromino[0][1] = darkblue;
      break;


    //normal
    //  x
    //xxx
    //orange
    case 8:
      Serial.println("orange");
      currentTetromino[3][3] = orange;
      currentTetromino[3][2] = orange;
      currentTetromino[2][2] = orange;
      currentTetromino[1][2] = orange;
      break;

    //rotate 1
    case 9:
      Serial.println("orange");
      currentTetromino[2][3] = orange;
      currentTetromino[2][2] = orange;
      currentTetromino[2][1] = orange;
      currentTetromino[3][1] = orange;
      break;

    //rotate 2
    case 10:
      Serial.println("orange");
      currentTetromino[1][1] = orange;
      currentTetromino[1][2] = orange;
      currentTetromino[2][2] = orange;
      currentTetromino[3][2] = orange;
      break;

    //rotate 3
    case 11:
      Serial.println("orange");
      currentTetromino[1][3] = orange;
      currentTetromino[2][3] = orange;
      currentTetromino[2][2] = orange;
      currentTetromino[2][1] = orange;
      break;

    //normal
    //xx
    //xx
    //yellow
    case 12:
      Serial.println("yellow");
      currentTetromino[1][3] = yellow;
      currentTetromino[2][3] = yellow;
      currentTetromino[1][2] = yellow;
      currentTetromino[2][2] = yellow;
      break;

    //rotate 1
    case 13:
      Serial.println("yellow");
      currentTetromino[1][3] = yellow;
      currentTetromino[2][3] = yellow;
      currentTetromino[1][2] = yellow;
      currentTetromino[2][2] = yellow;
      break;

    //rotate 2
    case 14:
      Serial.println("yellow");
      currentTetromino[1][3] = yellow;
      currentTetromino[2][3] = yellow;
      currentTetromino[1][2] = yellow;
      currentTetromino[2][2] = yellow;
      break;

    //rotate 3
    case 15:
      Serial.println("yellow");
      currentTetromino[1][3] = yellow;
      currentTetromino[2][3] = yellow;
      currentTetromino[1][2] = yellow;
      currentTetromino[2][2] = yellow;
      break;

    //normal
    // xx
    //xx
    //green
    case 16:
      Serial.println("green");
      currentTetromino[0][2] = green;
      currentTetromino[1][2] = green;
      currentTetromino[1][3] = green;
      currentTetromino[2][3] = green;
      break;

    //rotate 1
    case 17:
      Serial.println("green");
      currentTetromino[1][3] = green;
      currentTetromino[1][2] = green;
      currentTetromino[2][2] = green;
      currentTetromino[2][1] = green;
      break;

    //rotate 2
    case 18:
      Serial.println("green");
      currentTetromino[0][1] = green;
      currentTetromino[1][1] = green;
      currentTetromino[1][2] = green;
      currentTetromino[2][2] = green;
      break;

    //rotate 3
    case 19:
      Serial.println("green");
      currentTetromino[0][3] = green;
      currentTetromino[0][2] = green;
      currentTetromino[1][2] = green;
      currentTetromino[1][1] = green;
      break;

    //normal
    // x
    //xxx
    //purple
    case 20:
      Serial.println("purple");
      currentTetromino[0][2] = purple;
      currentTetromino[1][2] = purple;
      currentTetromino[1][3] = purple;
      currentTetromino[2][2] = purple;
      break;

    //rotate 1
    case 21:
      Serial.println("purple");
      currentTetromino[1][3] = purple;
      currentTetromino[1][2] = purple;
      currentTetromino[1][1] = purple;
      currentTetromino[2][2] = purple;
      break;

    //rotate 2
    case 22:
      Serial.println("purple");
      currentTetromino[0][2] = purple;
      currentTetromino[1][2] = purple;
      currentTetromino[1][1] = purple;
      currentTetromino[2][2] = purple;
      break;

    //rotate 3
    case 23:
      Serial.println("purple");
      currentTetromino[0][2] = purple;
      currentTetromino[1][3] = purple;
      currentTetromino[1][2] = purple;
      currentTetromino[1][1] = purple;
      break;

    //normal
    //xx
    // xx
    //red
    case 24:
      Serial.println("red");
      currentTetromino[1][3] = red;
      currentTetromino[2][3] = red;
      currentTetromino[2][2] = red;
      currentTetromino[3][2] = red;
      break;


    //rotate 1
    case 25:
      Serial.println("red");
      currentTetromino[1][1] = red;
      currentTetromino[1][2] = red;
      currentTetromino[2][2] = red;
      currentTetromino[2][3] = red;
      break;


    //rotate 2
    case 26:
      Serial.println("red");
      currentTetromino[1][3] = red;
      currentTetromino[2][3] = red;
      currentTetromino[2][2] = red;
      currentTetromino[3][2] = red;
      break;

    //rotate 3
    case 27:
      Serial.println("red");
      currentTetromino[2][1] = red;
      currentTetromino[2][2] = red;
      currentTetromino[3][2] = red;
      currentTetromino[3][3] = red;
      break;
  }
  currentX = 0;
  currentY = 16;
  drawCurrentTetromino(currentX, currentY);
}
