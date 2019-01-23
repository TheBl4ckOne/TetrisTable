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
#include "SeqTimer.h"
#include "TimerOne.h"

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

#define ROTATE 1
#define LEFT   2
#define RIGHT  3
#define FALL   4

bool rotation = false;

Adafruit_NeoPixel pixelArr[10];
Adafruit_NeoPixel landedArr[10];
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

  //Initialisierung des Arrays für die untenliegenden Tetrominos

  landedArr[0] = Adafruit_NeoPixel(20, ledPin0, NEO_GRB + NEO_KHZ800);
  landedArr[1] = Adafruit_NeoPixel(20, ledPin1, NEO_GRB + NEO_KHZ800);
  landedArr[2] = Adafruit_NeoPixel(20, ledPin2, NEO_GRB + NEO_KHZ800);
  landedArr[3] = Adafruit_NeoPixel(20, ledPin3, NEO_GRB + NEO_KHZ800);
  landedArr[4] = Adafruit_NeoPixel(20, ledPin4, NEO_GRB + NEO_KHZ800);
  landedArr[5] = Adafruit_NeoPixel(20, ledPin5, NEO_GRB + NEO_KHZ800);
  landedArr[6] = Adafruit_NeoPixel(20, ledPin6, NEO_GRB + NEO_KHZ800);
  landedArr[7] = Adafruit_NeoPixel(20, ledPin7, NEO_GRB + NEO_KHZ800);
  landedArr[8] = Adafruit_NeoPixel(20, ledPin8, NEO_GRB + NEO_KHZ800);
  landedArr[9] = Adafruit_NeoPixel(20, ledPin9, NEO_GRB + NEO_KHZ800);
  
  Serial.println("Stripes instanziert");

  //Starten der LED Stripes
  for (int stripe = 0; stripe < 10; stripe++) {
    pixelArr[stripe].begin();
    pixelArr[stripe].setBrightness(32);
    for (int pixel = 0; pixel < 20; pixel++) {
      pixelArr[stripe].setPixelColor(pixel, black);
    }
    pixelArr[stripe].show();

  //Timer
  Timer1.initialize(1000000);
  Timer1.attachInterrupt(moveTetromino,1000000);
    
  };
  
  Serial.println("Stripes gestartet und auf schwaz gesetzt");  
}

void loop()
{
  Serial.println("Beginn der loop"); 
  
}

void moveTetromino(){
  if (currentY > 3) {
    /*rotateCurrentTetromino();
    delay(500);*/
    lowerCurrentTetromino();
        
  } else {
        
    generateTetromino();
  }
}

void generateTetromino() 
{
  Serial.println("Beginn der generateTetromino");
  //Wegen Debug
  wipeScreen();
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {         
     
      currentTetromino[i][j] = black;
      
    }
  }

  switch (random(1, 20)) {
    
  wipeScreen();
    
    case 1:
    //xxxx
    //lightblue    
      Serial.println("lightblue");
      
      currentTetromino[0][2] = lightblue;
      currentTetromino[1][2] = lightblue;
      currentTetromino[2][2] = lightblue;
      currentTetromino[3][2] = lightblue;
      break;

    case 2:
    case 3:
    
    case 4:   
    //x
    //xxx
    //darkblue
      Serial.println("darkblue");
      currentTetromino[0][3] = darkblue;
      currentTetromino[0][2] = darkblue;
      currentTetromino[1][2] = darkblue;
      currentTetromino[2][2] = darkblue;
      break;

    case 5:
    case 6:

    case 7: 
    //  x
    //xxx
    //orange  
      Serial.println("orange");
      currentTetromino[1][2] = orange;
      currentTetromino[2][2] = orange;
      currentTetromino[3][2] = orange;
      currentTetromino[3][3] = orange;
      break;

    case 8:
    case 9:

    case 10: 
    //xx
    //xx
    //yellow   
      Serial.println("yellow");
      currentTetromino[1][3] = yellow;
      currentTetromino[2][3] = yellow;
      currentTetromino[1][2] = yellow;
      currentTetromino[2][2] = yellow;
      break;

    case 11:
    case 12:

    case 13:    
    // xx
    //xx
    //green   
      Serial.println("green");
      currentTetromino[0][2] = green;
      currentTetromino[1][2] = green;
      currentTetromino[1][3] = green;
      currentTetromino[2][3] = green;
      break;

    case 14:
    case 15:

    case 16:    
    // x
    //xxx
    //purple   
      Serial.println("purple");
      currentTetromino[0][2] = purple;
      currentTetromino[1][2] = purple;
      currentTetromino[1][3] = purple;
      currentTetromino[2][2] = purple;
      break;

    case 17:
    case 18:

    case 19:
    //xx
    // xx
    //red    
      Serial.println("red");
      currentTetromino[1][3] = red;
      currentTetromino[2][3] = red;
      currentTetromino[2][2] = red;
      currentTetromino[3][2] = red;
      break;
    
  }
  currentX = 0;
  currentY = 19;
  drawCurrentTetromino(currentX, currentY);
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
  for (int col = 0; col < 4; col++) {
    for (int row = 0; row < 4;  row++) {
      ledMatrix[x + col][y - row] = currentTetromino[col][3 - row];
    }
  }
  setLeds();
}

void lowerCurrentTetromino() 
{
  bool rotation = false;
  const byte direction = 0;
    
  switch ( direction ) 
    {
      case ROTATE:      
        break;
  
      case LEFT:      
        break;
  
      case RIGHT:      
        break;
  
      case FALL:      
        break;
  
      default:  
        break;

    }
    
  
  Serial.println("lowerCurrentTetromino");
  /*if (direction = (FALL)){
       
  }*/
  
  wipeCurrentTetromino(currentX, currentY);
  currentY = currentY - 1;
  drawCurrentTetromino(currentX, currentY);
}

void rotateCurrentTetromino() 
{
  /*if (random == 'case 1' && rotation == true){
    
  }*/
  
  /*Serial.println("rotateCurrentTetromino");
  uint32_t tetrominoBuffer[4][4];
  //rotieren und in den Buffer schreiben
  for (int currentCol = 0; currentCol < 4; currentCol++) {
    for (int currentRow = 0; currentRow < 4; currentRow++) {
      tetrominoBuffer[currentRow][3 - currentCol] = currentTetromino[currentCol][currentRow];
    }  
  }
   //Buffer in current schreiben
  for (int currentCol = 0; currentCol < 4; currentCol++) {
    for (int currentRow = 0; currentRow < 4; currentRow++) {
      currentTetromino[currentCol][currentRow] = tetrominoBuffer[currentCol][currentRow];
      tetrominoBuffer[currentCol][currentRow] = 0;
    }
  }*/
  drawCurrentTetromino(currentX, currentY);
}

void wipeCurrentTetromino(int x, int y) 
{
  Serial.println("wipeCurrentTetromino");
  for (int col = 0; col < 4; col++) {
    for (int row = 0; row < 4;  row++) {
      ledMatrix[x + col][y - row] = 0;
    }
  }
  setLeds();
}

/*//normal
    //xxxx
    //lightblue
    case 1:
      Serial.println("lightblue");
      wipeScreen();
      currentTetromino[0][2] = lightblue;
      currentTetromino[1][2] = lightblue;
      currentTetromino[2][2] = lightblue;
      currentTetromino[3][2] = lightblue;
      break;

    //rotate 1
    case 2: 
      Serial.println("lightblue");
      currentTetromino[2][0] = lightblue;
      currentTetromino[2][1] = lightblue;
      currentTetromino[2][2] = lightblue;
      currentTetromino[2][3] = lightblue;
      break;

    //rotate 2
    case 3:
      Serial.println("lightblue");
      currentTetromino[0][1] = lightblue;
      currentTetromino[1][1] = lightblue;
      currentTetromino[2][1] = lightblue;
      currentTetromino[3][1] = lightblue;
      break;

    //rotate 3
    case 4:
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
    case 5:
      Serial.println("darkblue");
      wipeScreen();
      currentTetromino[0][3] = darkblue;
      currentTetromino[0][2] = darkblue;
      currentTetromino[1][2] = darkblue;
      currentTetromino[2][2] = darkblue;
      break;

    //rotate 1
    case 6:
      Serial.println("darkblue");
      wipeScreen();
      currentTetromino[2][3] = darkblue;
      currentTetromino[1][3] = darkblue;
      currentTetromino[1][2] = darkblue;
      currentTetromino[1][1] = darkblue;
      break;

    //rotate 2
    case 7:
      Serial.println("darkblue");
      wipeScreen();
      currentTetromino[2][1] = darkblue;
      currentTetromino[2][2] = darkblue;
      currentTetromino[1][2] = darkblue;
      currentTetromino[0][2] = darkblue;
      break;

    //rotate 3
    case 8:
      Serial.println("darkblue");
      wipeScreen();
      currentTetromino[1][3] = darkblue;
      currentTetromino[1][2] = darkblue;
      currentTetromino[1][1] = darkblue;
      currentTetromino[0][1] = darkblue;
      break;


    //normal
    //  x
    //xxx
    //orange
    case 9:
      Serial.println("orange");
      wipeScreen();
      currentTetromino[3][3] = orange;
      currentTetromino[3][2] = orange;
      currentTetromino[2][2] = orange;
      currentTetromino[1][2] = orange;
      break;
      
  //rotate 1
  case 10:
    Serial.println("orange");
    wipeScreen();
    currentTetromino[2][3] = orange;
    currentTetromino[2][2] = orange;
    currentTetromino[2][1] = orange;
    currentTetromino[3][1] = orange;
    break;

  //rotate 2
  case 11:
    Serial.println("orange");
    wipeScreen();
    currentTetromino[1][1] = orange;
    currentTetromino[1][2] = orange;
    currentTetromino[2][2] = orange;
    currentTetromino[3][2] = orange;
    break;

  //rotate 3
  case 12:
    Serial.println("orange");
    wipeScreen();
    currentTetromino[1][3] = orange;
    currentTetromino[2][3] = orange;
    currentTetromino[2][2] = orange;
    currentTetromino[2][1] = orange;
    break;

    //normal
    //xx
    //xx
    //yellow
    case 13:
      Serial.println("yellow");
      wipeScreen();
      currentTetromino[1][3] = yellow;
      currentTetromino[2][3] = yellow;
      currentTetromino[1][2] = yellow;
      currentTetromino[2][2] = yellow;
      break;

    //rotate 1
    case 14:
      Serial.println("yellow");
      wipeScreen();
      currentTetromino[1][3] = yellow;
      currentTetromino[2][3] = yellow;
      currentTetromino[1][2] = yellow;
      currentTetromino[2][2] = yellow;
      break;     

    //rotate 2
    case 15:
      Serial.println("yellow");
      wipeScreen();
      currentTetromino[1][3] = yellow;
      currentTetromino[2][3] = yellow;
      currentTetromino[1][2] = yellow;
      currentTetromino[2][2] = yellow;
      break; 

    //rotate 3
    case 16:
      Serial.println("yellow");
      wipeScreen();
      currentTetromino[1][3] = yellow;
      currentTetromino[2][3] = yellow;
      currentTetromino[1][2] = yellow;
      currentTetromino[2][2] = yellow;
      break;    

    //normal
    // xx
    //xx
    //green
    case 17:
      Serial.println("green");
      wipeScreen();
      currentTetromino[0][2] = green;
      currentTetromino[1][2] = green;
      currentTetromino[1][3] = green;
      currentTetromino[2][3] = green;
      break;

    //rotate 1
    case 18:
      Serial.println("green");
      wipeScreen();
      currentTetromino[1][3] = green;
      currentTetromino[1][2] = green;
      currentTetromino[2][2] = green;
      currentTetromino[2][1] = green;
      break;

    //rotate 2
    case 19:     
      Serial.println("green");
      wipeScreen();
      currentTetromino[0][1] = green;
      currentTetromino[1][1] = green;
      currentTetromino[1][2] = green;
      currentTetromino[2][2] = green;
      break;

    //rotate 3
    case 20:
      Serial.println("green");
      wipeScreen();
      currentTetromino[0][3] = green;
      currentTetromino[0][2] = green;
      currentTetromino[1][2] = green;
      currentTetromino[1][1] = green;
      break;
      
    //normal
    // x
    //xxx
    //purple
    case 21:
      Serial.println("purple");
      wipeScreen();
      currentTetromino[0][2] = purple;
      currentTetromino[1][2] = purple;
      currentTetromino[1][3] = purple;
      currentTetromino[2][2] = purple;
      break;

    //rotate 1
    case 22:
      Serial.println("purple");
      wipeScreen();
      currentTetromino[1][3] = purple;
      currentTetromino[1][2] = purple;
      currentTetromino[1][1] = purple;
      currentTetromino[2][2] = purple;
      break;

    //rotate 2
    case 23:
      Serial.println("purple");
      wipeScreen();
      currentTetromino[0][2] = purple;
      currentTetromino[1][2] = purple;
      currentTetromino[1][1] = purple;
      currentTetromino[2][2] = purple;
      break;

    //rotate 3
    case 24:
      Serial.println("purple");
      wipeScreen();
      currentTetromino[0][2] = purple;
      currentTetromino[1][3] = purple;
      currentTetromino[1][2] = purple;
      currentTetromino[1][1] = purple;
      break;

    //normal
    //xx
    // xx
    //red
    case 25:
      Serial.println("red");
      wipeScreen();
      currentTetromino[1][3] = red;
      currentTetromino[2][3] = red;
      currentTetromino[2][2] = red;
      currentTetromino[3][2] = red;
      break;


    //rotate 1
    case 26:
      Serial.println("red");
      wipeScreen();
      currentTetromino[1][1] = red;
      currentTetromino[1][2] = red;
      currentTetromino[2][2] = red;
      currentTetromino[2][3] = red;
      break;


    //rotate 2
    case 27:
      Serial.println("red");
      wipeScreen();
      currentTetromino[1][3] = red;
      currentTetromino[2][3] = red;
      currentTetromino[2][2] = red;
      currentTetromino[3][2] = red;
      break;

    //rotate 3
    case 28:
      Serial.println("red");
      wipeScreen();
      currentTetromino[2][1] = red;
      currentTetromino[2][2] = red;
      currentTetromino[3][2] = red;
      currentTetromino[3][3] = red;
      break;

      */
