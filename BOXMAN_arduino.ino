/*
 * [][]    [][][]  []  []  [][][]    []    [][][]
 * [][][]  []  []    []    [][][]  [][][]  []  []
 * [][][]  [][][]  []  []  []  []  []  []  []  []
 *         By Nathan "Botboy3000" Paskach
*/

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <Entropy.h>
#include <EEPROM.h>

#include <time.h>
#include "song.h"
/*
#include <Fonts/FreeSerif24pt7b.h>
#include <Fonts/FreeSerifBold24pt7b.h>
#include <Fonts/FreeSerif12pt7b.h>
*/

#define TFT_CS     10
#define TFT_RST    8  // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to 0!
#define TFT_DC     9


//Basic Color Definitions
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define screenW 128
#define screenH 160

#define titleFont &FreeSerifBold24pt7b
#define scoreFont &FreeSerif12pt7b

#define buttonR 7
#define buttonL 2

#define bgColor BLACK

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

void setup() {
  // put your setup code here, to run once:
/*
  tft.setFont(titleFont);
  tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
*/

  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.fillScreen(bgColor);
  tft.setTextSize(1);
  //tft.setFont(scoreFont);
  delay(500);
  for(int x = 1; x < screenH / 2; x++)
  {
    /*
    tft.setCursor(screenW / 3, x - 1);
    tft.setTextColor(BLACK);
    tft.println("Boxman");
    */
    tft.fillRect(screenW / 3, x - 1, 40, 10, bgColor);
     
    tft.setCursor(screenW / 3, x);
    tft.setTextColor(GREEN);
    tft.println("Boxman");
    delay(10);
  }
  //delay(1);
  tft.print("Press both buttons to begin");
  randomSeed(time(NULL));
  int timer = 0;
  while(digitalRead(buttonR) + digitalRead(buttonL) < 2)
  {
    timer++;
  }

  tone(4, 840, 50);
  delay(50);
  tone(4, 890, 50);
  delay(50);
  tone(4, 1000, 100);
  
  tft.fillScreen(bgColor);
  delay(500);

  tft.setFont();

  Serial.begin(9600);
  Serial.println("testing");

  /*
  long randNumber;
  //srand(*(unsigned long*)0xF3004400);
  srand((unsigned)time(NULL));//seed
  */
  
  Entropy.Initialize();
  srand(timer);
}

int score = 0;
int boxsize = 8;
float boxy;
int boxx;
float boxvel;

int right = 0;
int left = 0;
int boxmanx = 0;
bool falling = false;
int level = 0;
int boxamount = 128 / boxsize;
int boxes[16];

void loop() {
  // put your main code here, to run repeatedly:
  for(int i = 0; i < 128 / boxsize; i++)
  {
    boxes[i] = 0;
  }

  tft.fillRect(0, 0, screenW, 9, BLACK);
  tft.fillScreen(bgColor);
  int total;
  for(int o = 0; o < 10; o++)
  {
    total = EEPROM.read(o); + total;
  }

  randomSeed(total);

  int noteCounter = 0;
  int note = 0;
  
  while(true) //////////////////////////////////////////////////////////////////////////////////////////////////
  {
    gameFrame();
    delay(10);
    
    noteCounter++;
    if(noteCounter == 2)
    {
        playNote(note - 1);
    }
    if(noteCounter > 6)
    {
      playNoteDown(note);
      note++;
      noteCounter = 0;
    }

    if(note > 1023)
    {
      note = 0;
    }
     
    
  }
}//////////////////////////////////////////////////////////////////////////////////////////////////////////////



void initGameVariables()
{
  int right = 0;
  int left = 0;
  int boxmanx = 0;
  bool falling = false;
  int level = 0;
  int boxes[128 / boxsize];
  for(int i = 0; i < 128 / boxsize; i++)
  {
    boxes[i] = 0;
  }
}

void resetGameVariables()
{
  score = 0;
  right = 0;
  left = 0;
  boxmanx = 0;
  falling = false;
  level = 0;
  for(int i = 0; i < 128 / boxsize; i++)
  {
    boxes[i] = 0;
  }
}

void breaktops()
{
  for(int j = 0; j < 128 / boxsize; j++)
  {
    tft.fillRect(boxsize * j, 160 - boxes[j], boxsize, boxsize, bgColor);
    if(boxes[j] > 0)
    {
      boxes[j] = boxes[j] - boxsize;
    }
  }
}

void gameFrame()
{
  int R = digitalRead(buttonR);
  int L = digitalRead(buttonL);

  Serial.write(12);
  Serial.print("L: ");
  Serial.println(analogRead(5));

  // Handling button presses (1 press = 1 movement)
  if(R == 1 && (boxmanx < (128 - boxsize) / boxsize) && (boxes[boxmanx + 1] - boxes[boxmanx] < 2 * boxsize))
  {
    right++;
  }
  if(L == 1 && (boxmanx > 0) && (boxes[boxmanx - 1] - boxes[boxmanx] < 2 * boxsize))
  {
    left++;
  }

  if(right == 2)
  {
    tft.fillRect(boxmanx * boxsize, (160 - boxsize) - boxes[boxmanx], boxsize, boxsize, bgColor);
    boxmanx++;
    right = 5;
  }
  if(left == 2)
  {
    tft.fillRect(boxmanx * boxsize, (screenH - boxsize) - boxes[boxmanx], boxsize, boxsize, bgColor);
    boxmanx--;
    left = 5;
  }

  if(R == 0)
  {
    right = 0;
  }
  if(L == 0)
  {
    left = 0;
  }

  //if boxman gets too high, scroll screen up
  if(boxes[boxmanx] > 5 * boxsize)
  {
    breaktops();
    level++;
  }

  //Draw Boxman
  tft.fillRect(boxmanx * boxsize, (screenH - boxsize) - boxes[boxmanx], boxsize, boxsize, GREEN);

  //Init a falling box if there isn't one
  if(falling == 0)
  {
    falling = 1;
    boxy = 11;
    boxx = (rand()%(screenW / boxsize));
    while(boxes[boxx] > 80)
    {
      boxx = (rand()%(screenW / boxsize));
    }
    boxvel = 0;
  }

  //Make the falling box fall
  if(falling == 1)
  {
    boxvel = boxvel + 0.03;
    boxy = boxy + boxvel;
    if(boxy > screenH - boxsize - boxes[boxx])
    {
      boxy = screenH - boxsize - boxes[boxx];
      falling = 0;
      boxes[boxx] = boxes[boxx] + boxsize;
      score = score + level + (boxes[boxmanx] / boxsize);

      tft.fillRect(38, 0, 70, 9, BLACK);
      tft.setCursor(1, 1);
      tft.setTextColor(WHITE);
      tft.print("Score: ");
      tft.println(score);
      //tone(4, 1000, 10);
    }

    if(boxy > (screenH - boxsize - (boxsize/2)) - boxes[boxmanx] && boxx == boxmanx)
    {
      EEPROM.write(0, (score+boxy)/(boxmanx+1) + time(NULL));
      tft.setTextColor(RED);
      tft.setCursor(1, 9);
      tft.println("Game Over");
      tone(4, 200, 10);
      delay(5);
      tone(4, 50, 100);
      delay(2000);
      resetGameVariables();
      loop();
    }
  }

  //draw box
  tft.fillRect(boxx * boxsize, boxy, boxsize, boxsize, BLUE);
  //erase old box
  tft.fillRect(boxx * boxsize, boxy - boxvel - 2, boxsize, boxvel + 3, bgColor);
  
}

