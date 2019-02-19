/*
  Festlegeúng für for-Schleifen: im Kontext der ledMatrix, des landedArr und des currentTetromino heißen die Laufvariablen col und row
                                 im Kontext des PixelArr heißen die Laufvariablen Stripe und Pixel
  Für die Arrays ledMatrix, landedArr, currentTetromino und tetrominoBuffer gilt: name[spalte][zeile]
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
#define buttonPinReset    44
#define buttonPinRotate   42
#define buttonPinDrop     46

//Joystick
#define buttonPinLeft     38
#define buttonPinRight    36
#define buttonPinDown     40
#define buttonPinUp       48 //keine Funktionalitäten zugewiesen

//zeitlicher Abstand zwischen 2 Aktionen des Spielers
#define actionDelay 100

Adafruit_NeoPixel pixelArr[10];
//interne Matrix größer als Spielfeld, damit Tetrominos von oben reinlaufen können
uint32_t landedArr[10][25];
uint32_t ledMatrix[10][25];

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
uint32_t white = pixelArr[0].Color(255, 255, 255);

//Tetrominos
uint32_t currentTetromino[4][4];
uint32_t tetrominoBuffer[4][4];
int currentX = 0;
int currentY = 0;
int nextType = 0;
int nextRotation = 0;

//Spielende
boolean lose = false;

//Regelung der Eingaben
unsigned long lastInteraction; // Abstand zwischen Aktionen zur besseren Steuerbarkeit
boolean rotateReleased = true; //Zwang den Rotieren-Knopf für jeden Rotationsschritt zu drücken, für bessere Steuerbarkeit
boolean dropReleased = true; //Zwang den Sofort-Runter-Knopf für jedes Tetromino zu drücken, für bessere Steuerbarkeit

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

  //Aktivierung der internen Pullup Widerstände
  pinMode(buttonPinReset, INPUT_PULLUP);
  pinMode(buttonPinRotate, INPUT_PULLUP);
  pinMode(buttonPinDrop, INPUT_PULLUP);
  pinMode(buttonPinLeft, INPUT_PULLUP);
  pinMode(buttonPinRight, INPUT_PULLUP);
  pinMode(buttonPinDown, INPUT_PULLUP);

  //Initialisierung des Pixelarrys
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

  //Initialisierung der ledMatrix, des landedArr und des currentTetromino
  for (int col = 0; col < 10; col++) {
    for (int row = 0; row < 25; row++) {
      ledMatrix[col][row] = 0;
    }
  }

  for (int col = 0; col < 10; col++) {
    for (int row = 0; row < 25; row++) {
      landedArr[col][row] = 0;
    }
  }

  for (int col = 0; col < 4; col++) {
    for (int row = 0; row < 4; row++) {
      currentTetromino[col][row] = 0;
    }
  }

  //Starten der LED Stripes, setzen der Hellingkeit auf 1/8 und Farbe Schwarz
  for (int stripe = 0; stripe < 10; stripe++) {
    pixelArr[stripe].begin();
    pixelArr[stripe].setBrightness(128);
    for (int pixel = 0; pixel < 25; pixel++) {
      pixelArr[stripe].setPixelColor(pixel, black);
    }
    pixelArr[stripe].show();
  };

  //Initialisierung der Tetrominos
  int Random1 = random(0, 49); //zufällig gewählte Zahlen um die Random zufälliger zu machen
  nextType = (Random1 % 7);
  nextRotation = random(0, 4);
  generateTetromino(nextType, nextRotation, true, true);

  //Timer
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1 << CS12); // TCCR1B |= (1 << CS11)|(1<<CS10) für Prescaler 64 ~ 0,25 Sekunden   TCCR1B |= (1<<CS12) für Prescaler 256 ~ 1 Sekunde   TCCR1B |= (1<<CS12) | (1<<CS10) für Prescaler 1024 ~ 4 Sekunden
  TIMSK1 |= (1 << TOIE1); // Timer Overflow aktivieren
}

//Interrupt Service Routine um die Abwärtsbewegung der Tetrominos zu steuern und zwischen der Runterbewegung Eingaben entgehennehmen zu können
ISR(TIMER1_OVF_vect) {
  if (!lose) {
    lowerCurrentTetromino();
  }
  else {}
}

void loop() //Überprüfung der Zustände auf den Button- und Joystick-Pins
{
  bool sensorButtonReset = digitalRead(buttonPinReset);
  bool sensorButtonRotate = digitalRead(buttonPinRotate);
  bool sensorButtonDrop = digitalRead(buttonPinDrop);
  bool sensorButtonLeft = digitalRead(buttonPinLeft);
  bool sensorButtonRight = digitalRead(buttonPinRight);
  bool sensorButtonDown = digitalRead(buttonPinDown);


  if (lastInteraction < (millis() - actionDelay)) { //Erzwingen, dass zwischen zwei Aktionen mindestens das actionDelay dazwischenliegen muss um Versehentliche Mehrfacheingaben durch minimal längeres Drücken zu verhinden
    lastInteraction = millis();

    //Überprüfung ob Reset Button gedrückt > Startet Spiel neu
    if (sensorButtonReset == LOW) {
      cli(); //sperrt ISR -> Damit die Methoden die nachfolgend aufgerufen werden bis zum Ende durchgeführt werden und nicht durch die ISR unterbrochen werden
      wipeScreen();
      int currentX = 0;
      int currentY = 0;
      int nextType = 0;
      int nextRotation = 0;
      setup();
      sei(); //öffnet ISR -> damit die Abwärtsbewegung wieder ausgeführt werden kann
    }
    else {
      //Überprüfung ob Rotate Button gedrückt > currentTetromino drehen
      if (sensorButtonRotate == LOW) {
        cli();
        if (rotateReleased) {
          
          rotateReleased = false;
          rotateCurrentTetromino();
        }
        sei();
      }

      else {
        //Drehen-Button Prüfung auf losgelassen setzen
        rotateReleased = true;

        //Überprüfung ob Drop Button gedrückt
        if (sensorButtonDrop == LOW) {
          cli();
          if (dropReleased) {
            dropReleased = false;
            dropCurrentTetromino();
          }
          sei();
        }

        else {
          //Drop-Button Prüfung auf losgelassen setzen
          dropReleased = true;

          //Überprüfung ob links Button gedrückt > currentTetromino nach links
          if (sensorButtonLeft == LOW) {
            cli();
            lefterCurrentTetromino();
            sei();
          }

          else {
            //Überprüfung ob rechts Button gedrückt > currentTetromino nach rechts
            if (sensorButtonRight == LOW) {
              cli();
              righterCurrentTetromino();
              sei();
            }

            else {
              //Überprüfung ob unten Butteon gedrückt > currentTetromino nach unten
              if (sensorButtonDown == LOW) {
                cli();
                lowerCurrentTetromino();
                sei();
              }
            }
          }
        }
      }
    }
  }
}

//setzt die gesamte ledMatrix auf schwarz
void wipeScreen()
{
  for (int col = 0; col < 10; col++) {
    for (int row = 0; row < 25; row++) {
      ledMatrix[col][row] = black;
    }
  }
}

//schreibt das aktuelle Tetromino in die ledMatrix und setzt die LED's sodass es auch angezeigt wird
void drawCurrentTetromino(int x, int y)
{
  for (int col = 0; col < 4; col++) {
    for (int row = 0; row < 4;  row++) {
      if (currentTetromino[col][row] != 0) {
        ledMatrix[x + col][y + row] = currentTetromino[col][row];
      }
    }
  }
  setLeds();
}

//setzt alle Felder des currentTetromino die ungleich 0 sind (also auf der ledMatrix farblich angezeigt werden) auf schwarz
void wipeCurrentTetromino(int x, int y) {
  for (int col = 0; col < 4; col++) {
    for (int row = 0; row < 4;  row++) {
      if (currentTetromino[col][row] != 0 && landedArr[x + col][y + row] == 0) {
        ledMatrix[x + col][y + row] = black;
      }
    }
  }
}

//Falls eine Kollision entdeckt wurde, wird diese Methode aufgerufen
//Schreibt das currentTetromino in seiner letzten Position in das landedArr
void collisionDetected() {
  lose = false;
  //cli();
  for (int col = 0; col < 4; col++) {
    for (int row = 0; row < 4; row++) {
      if (currentTetromino[col][row] != 0) {
        landedArr[currentX + col][currentY + row] = currentTetromino[col][row];
      }
    }
  }

  //Überüft ob eine Zeile vollständig gefüllt werden konnte
  for (int row = 0; row < 20; row ++) { //prüfen ob eine Zeile voll ist
    int completeRow = 0;
    for (int col = 0; col < 10; col ++) {
      if (landedArr[col][row] != 0) {
        completeRow++;
      }
    }
    if (completeRow == 10) { //falls eine Zeile komplett gefüllt wurde

      for (int col = 0; col < 10; col++) {
        //komplette Zeile schwarz zeichnen...
        ledMatrix[col][row] = 0;
        landedArr[col][row] = 0;
      }

      int newRow = row + 1; //erste Zeile über der schwarz gemalten Zeile

      for (newRow; newRow < 20; newRow++) { //ab einer Zeile über der schwarzen Zeile nach oben bis Zeile 19
        for (int col = 0; col < 10; col++) {
          //Verschieben der Zeilen über der schwarzen Zeile um jeweils eine Zeile nach unten in ledMatrix und landedArr
          ledMatrix[col][newRow - 1] = ledMatrix[col][newRow];
          landedArr[col][newRow - 1] = landedArr[col][newRow];
        }
      }
      --row; //Damit die nach unten gerückte Zeile auch kontrolliert wird auf Vollständigkeit der Zeile und bei Bedarf mit gelöscht wird
    }

    completeRow = 0; //Rücksetzen des Zählers für volle Zeilen

  } //Ende der Überprüfung für Vollständigkeit der Zeilen

  //Spielende definieren -> Sobald ein currentTetromino in Zeile 20 liegt, wird lose auf true gesetzt
  for (int col = 0; col < 10; col++) {
    if (landedArr[col][20] != 0) {
      lose = true;
      break;
    }
  }

  //Wenn lose false ist
  if (!lose) {
    int Random1 = random(0, 84); //zufällig gewählte Zahlen um die Random zufälliger zu machen
    nextType = (Random1 % 7);
    nextRotation = random(0, 4);
    generateTetromino(nextType, nextRotation, true, true);
  }

  //Wenn lose true ist
  else {
    gameOver();
  }
}

void gameOver() {
  cli();
  //Endanimation > Das Spielfeld wird Feld für Feld unten links beginnend farblich gefüllt
  for (int col = 0; col < 10; col++) {
    for (int row = 0; row < 20; row++) {
      ledMatrix[col][row] = white;
      setLeds();
    }
  }
  lose = false; //lose zurück auf false setzen, damit das Spiel automatisch von vorne beginnen kann
  setup();
  sei();
}

//Zum Drehen des currentTetrominos
void rotateCurrentTetromino() {
  int actRotation = nextRotation; //speichern der alten Rotation falls keine Kollision auftritt
  
  //Zählen der Rotation umlaufend von 0..3
  if (nextRotation == 3) {
    nextRotation = 0;
  }
  else {
    nextRotation = nextRotation + 1;
  }

  //Wenn keine Kollision auftritt
  wipeCurrentTetromino(currentX, currentY);
  generateTetromino(nextType, nextRotation, false, false);
  
  
  //Wenn eine Kollision auftritt
  if (checkCollision(currentX, currentY)) {
    wipeCurrentTetromino(currentX, currentY);
    generateTetromino(nextType, actRotation, false, false);    
  }
  else {
    drawCurrentTetromino(currentX, currentY);   
  }
}

//Bewegung des currentTetromino nach rechts -> KEINE Überführung in das landedArr bei Kollision
void righterCurrentTetromino() {
  //Vor dem Durchführen der Bewegung Überprüfung ob eine Kollision droht
  if (!checkCollision(currentX + 1, currentY)) { //Falls keine Kollision droht
    //löschen des "alten" Tetromino
    wipeCurrentTetromino(currentX, currentY);
    ++currentX;
    drawCurrentTetromino(currentX, currentY);
  }

}

//Bewegung des currentTetromino nach links -> KEINE Überführung in das landedArr bei Kollision
void lefterCurrentTetromino() {
  //Vor dem Durchführen der Bewegung Überprüfung ob eine Kollision droht
  if (!checkCollision(currentX - 1, currentY)) {
    //löschen des "alten" Tetrominos
    wipeCurrentTetromino(currentX, currentY);
    --currentX;
    drawCurrentTetromino(currentX, currentY);
  }

}

//Bewegung des currentTetromino nach unten -> Überführung in das landedArr bei Kollision
void lowerCurrentTetromino()
{
  //Vor dem Durchführen der Bewegung Überprüfung ob eine Kollision droht
  if (!checkCollision(currentX, currentY - 1)) {
    //löschen des "alten" Tetromino
    wipeCurrentTetromino(currentX, currentY);
    --currentY;
    drawCurrentTetromino(currentX, currentY);
  }
  else {
    collisionDetected();
  }
}

//Sofortiges Herunterfallen des currentTetromino bis zur Kollision
void dropCurrentTetromino()
{
  wipeCurrentTetromino(currentX, currentY);

  while (!checkCollision(currentX, currentY - 1)) {
    --currentY;
  }

  drawCurrentTetromino(currentX, currentY);
  setLeds();
  collisionDetected();
}


//bekommt GEPLANTE Koordinaten übergeben und prüft ob eine Kollision vorhanden wäre
//Gibt true zurück bei drohender Kollision
boolean checkCollision(int futureX, int futureY) {
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      if (currentTetromino[col][row] != 0) {
        //checken ob Kollision am unteren Rand --> neues Tetromino generieren
        if ((futureY + row) < 0) {
          return true;
        }
        //checken ob Kollision am linken Rand
        if ((futureX + col) < 0) {
          return true;
        }
        //checken ob Kollision am rechten Rand
        if ((futureX + col) > (9)) { // da future Y oben links liegt
          return true;
        }
        //checken ob Kollision mit Tetrominos im landedArr mit bereits gelandeten Tetrominos
        if (landedArr[futureX + col][futureY + row] != 0) {
          return true;
        }
      }
    }
  }
  return false;
}

//Methode zum Anzeigen auf der Led Matrix
void setLeds()
{
  for (int stripe = 0; stripe < 10; stripe++) {
    for (int pixel = 0; pixel < 20; pixel++) {
      pixelArr[stripe].setPixelColor(pixel, ledMatrix[stripe][pixel]);
    };
    pixelArr[stripe].show();
  }
}

//generieren (isNew = true) oder modifzieren (isNew = false) des currentTetrominos
void generateTetromino(int type, int rotation, bool isNew, bool isDrawing)
{
  int nextTetromino = type * 4 + rotation; //Es gibt 7 Tetromino-Typen mit jeweils 4 Rotationsausrichtungen

  //Korrektur der Start-Position für die Tetrominos
  int offsetY = 0;
  int offsetX = 0;

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      currentTetromino[i][j] = black; //Setzen des kompletten Tetrominos auf schwarz
    }
  }

  switch (nextTetromino) {
    //Setzen der Felder im currentTetromino für die unterschiedlichen Tetromino-Typen und Rotationsausrichtungen

    //normal
    //xxxx
    //lightblue
    case 0:      
      currentTetromino[0][2] = lightblue;
      currentTetromino[1][2] = lightblue;
      currentTetromino[2][2] = lightblue;
      currentTetromino[3][2] = lightblue;
      offsetX = 0;
      offsetY = -2;
      break;

    //rotate 1
    case 1:
      currentTetromino[2][0] = lightblue;
      currentTetromino[2][1] = lightblue;
      currentTetromino[2][2] = lightblue;
      currentTetromino[2][3] = lightblue;
      offsetX = 0;
      offsetY = 0;
      break;

    //rotate 2
    case 2:
      currentTetromino[0][1] = lightblue;
      currentTetromino[1][1] = lightblue;
      currentTetromino[2][1] = lightblue;
      currentTetromino[3][1] = lightblue;
      offsetX = 0;
      offsetY = -2;
      break;

    //rotate 3
    case 3:
      currentTetromino[1][3] = lightblue;
      currentTetromino[1][2] = lightblue;
      currentTetromino[1][1] = lightblue;
      currentTetromino[1][0] = lightblue;
      offsetX = 0;
      offsetY = 0;
      break;

    //normal
    //x
    //xxx
    //darkblue
    case 4:
      currentTetromino[0][3] = darkblue;
      currentTetromino[0][2] = darkblue;
      currentTetromino[1][2] = darkblue;
      currentTetromino[2][2] = darkblue;
      offsetX = 0;
      offsetY = -2;
      break;

    //rotate 1
    case 5:
      currentTetromino[2][3] = darkblue;
      currentTetromino[1][3] = darkblue;
      currentTetromino[1][2] = darkblue;
      currentTetromino[1][1] = darkblue;
      offsetX = 0;
      offsetY = -1;
      break;

    //rotate 2
    case 6:
      currentTetromino[2][1] = darkblue;
      currentTetromino[2][2] = darkblue;
      currentTetromino[1][2] = darkblue;
      currentTetromino[0][2] = darkblue;
      offsetX = 0;
      offsetY = -1;
      break;

    //rotate 3
    case 7:
      currentTetromino[1][3] = darkblue;
      currentTetromino[1][2] = darkblue;
      currentTetromino[1][1] = darkblue;
      currentTetromino[0][1] = darkblue;
      offsetX = 1;
      offsetY = -1;
      break;


    //normal
    //  x
    //xxx
    //orange
    case 8:
      currentTetromino[3][3] = orange;
      currentTetromino[3][2] = orange;
      currentTetromino[2][2] = orange;
      currentTetromino[1][2] = orange;
      offsetX = 0;
      offsetY = -2;
      break;

    //rotate 1
    case 9:
      currentTetromino[2][3] = orange;
      currentTetromino[2][2] = orange;
      currentTetromino[2][1] = orange;
      currentTetromino[3][1] = orange;
      offsetX = -1;
      offsetY = -1;
      break;

    //rotate 2
    case 10:
      currentTetromino[1][1] = orange;
      currentTetromino[1][2] = orange;
      currentTetromino[2][2] = orange;
      currentTetromino[3][2] = orange;
      offsetX = 0;
      offsetY = -1;
      break;

    //rotate 3
    case 11:
      currentTetromino[1][3] = orange;
      currentTetromino[2][3] = orange;
      currentTetromino[2][2] = orange;
      currentTetromino[2][1] = orange;
      offsetX = 0;
      offsetY = -1;
      break;

    //normal
    //xx
    //xx
    //yellow
    case 12:
      currentTetromino[1][3] = yellow;
      currentTetromino[2][3] = yellow;
      currentTetromino[1][2] = yellow;
      currentTetromino[2][2] = yellow;
      offsetX = 0;
      offsetY = -2;
      break;

    //rotate 1
    case 13:
      currentTetromino[1][3] = yellow;
      currentTetromino[2][3] = yellow;
      currentTetromino[1][2] = yellow;
      currentTetromino[2][2] = yellow;
      offsetX = 0;
      offsetY = -2;
      break;

    //rotate 2
    case 14:
      currentTetromino[1][3] = yellow;
      currentTetromino[2][3] = yellow;
      currentTetromino[1][2] = yellow;
      currentTetromino[2][2] = yellow;
      offsetX = 0;
      offsetY = -2;
      break;

    //rotate 3
    case 15:
      currentTetromino[1][3] = yellow;
      currentTetromino[2][3] = yellow;
      currentTetromino[1][2] = yellow;
      currentTetromino[2][2] = yellow;
      offsetX = 0;
      offsetY = -2;
      break;

    //normal
    // xx
    //xx
    //green
    case 16:
      currentTetromino[0][2] = green;
      currentTetromino[1][2] = green;
      currentTetromino[1][3] = green;
      currentTetromino[2][3] = green;
      offsetX = 0;
      offsetY = -2;
      break;

    //rotate 1
    case 17:
      currentTetromino[1][3] = green;
      currentTetromino[1][2] = green;
      currentTetromino[2][2] = green;
      currentTetromino[2][1] = green;
      offsetX = 0;
      offsetY = -1;
      break;

    //rotate 2
    case 18:
      currentTetromino[0][1] = green;
      currentTetromino[1][1] = green;
      currentTetromino[1][2] = green;
      currentTetromino[2][2] = green;
      offsetX = 0;
      offsetY = -1;
      break;

    //rotate 3
    case 19:
      currentTetromino[0][3] = green;
      currentTetromino[0][2] = green;
      currentTetromino[1][2] = green;
      currentTetromino[1][1] = green;
      offsetX = 1;
      offsetY = -1;
      break;

    //normal
    // x
    //xxx
    //purple
    case 20:
      currentTetromino[0][2] = purple;
      currentTetromino[1][2] = purple;
      currentTetromino[1][3] = purple;
      currentTetromino[2][2] = purple;
      offsetX = 0;
      offsetY = -2;
      break;

    //rotate 1
    case 21:
      currentTetromino[1][3] = purple;
      currentTetromino[1][2] = purple;
      currentTetromino[1][1] = purple;
      currentTetromino[2][2] = purple;
      offsetX = 0;
      offsetY = -1;
      break;

    //rotate 2
    case 22:
      currentTetromino[0][2] = purple;
      currentTetromino[1][2] = purple;
      currentTetromino[1][1] = purple;
      currentTetromino[2][2] = purple;
      offsetX = 0;
      offsetY = -1;
      break;

    //rotate 3
    case 23:
      currentTetromino[0][2] = purple;
      currentTetromino[1][3] = purple;
      currentTetromino[1][2] = purple;
      currentTetromino[1][1] = purple;
      offsetX = 0;
      offsetY = -1;
      break;

    //normal
    //xx
    // xx
    //red
    case 24:
      currentTetromino[1][3] = red;
      currentTetromino[2][3] = red;
      currentTetromino[2][2] = red;
      currentTetromino[3][2] = red;
      offsetX = 0;
      offsetY = -2;
      break;

    //rotate 1
    case 25:
      currentTetromino[1][1] = red;
      currentTetromino[1][2] = red;
      currentTetromino[2][2] = red;
      currentTetromino[2][3] = red;
      offsetX = 0;
      offsetY = -1;
      break;

    //rotate 2
    case 26:
      currentTetromino[1][3] = red;
      currentTetromino[2][3] = red;
      currentTetromino[2][2] = red;
      currentTetromino[3][2] = red;
      offsetX = 0;
      offsetY = -2;
      break;

    //rotate 3
    case 27:
      currentTetromino[2][1] = red;
      currentTetromino[2][2] = red;
      currentTetromino[3][2] = red;
      currentTetromino[3][3] = red;
      offsetX = -1;
      offsetY = -1;
      break;
  }

  if (isNew) { //bei einem neu generierten Tetromino werden currentX und currentY auf die Startwerte gesetzt
    currentX = 3 + offsetX;
    currentY = 20  + offsetY;
  }

  if (isDrawing = true) { //soll das Tetromino nach dem generieren tatsächlich auch in die ledMatrix übertragen werden? -> Nicht bei der CollisionDetection für die rotateCurrentTetromino
    if (!checkCollision(currentX, currentY)) {
      drawCurrentTetromino(currentX, currentY);
    }
  }
}
