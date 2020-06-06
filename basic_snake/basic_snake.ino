#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//DISPLAY 
#define OLED_RESET    -1    // Reset pin # (or -1 if sharing Arduino reset pin)
#define OLED_ADDRESS  0x3C  // I2C address of the display.
#define SCREEN_WIDTH  128   // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels

//BUTTON
#define LEFT_B_IN     A0    // A0
#define LEFT_B_OUT    A2    // A2   
#define RIGHT_B_IN    A1    // A1
#define RIGHT_B_GND   A3    // A3
#define RIGHT_B_OUT   13   // D13

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Snake stuff
typedef struct coord
{
  byte x;
  byte y;
} coord;

coord snake[210];

void makeSnake()
{

}

//Draw square on the 21x10 grid
//x is between 0 and 20 inclusive
//y is between 0 and 9 inclusive
void drawSquare(int x, int y, bool on)
{
  if (on)
  {
    display.fillRect(6*x+3,6*y+2,4,4,SSD1306_WHITE);
  }
  else
  {
    display.fillRect(6*x+3,6*y+2,4,4,SSD1306_BLACK);
  }
}

void setup() {

  //Random numbers
  randomSeed(analogRead(0));

  //Set up the buttons
  pinMode(LEFT_B_IN, INPUT);
  pinMode(LEFT_B_OUT, OUTPUT);
  digitalWrite(LEFT_B_OUT,1);

  pinMode(RIGHT_B_IN, INPUT);
  pinMode(RIGHT_B_GND, OUTPUT);
  pinMode(RIGHT_B_OUT, OUTPUT);
  digitalWrite(RIGHT_B_OUT,1);
  digitalWrite(RIGHT_B_GND,0);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {for(;;);}

  //Set up the title screen
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20,5);
  display.println(F("SNAKE"));

  display.setTextSize(1);
  display.setCursor(26,40);
  display.println(F("Hit L to play"));
  display.display();
  
  while (!digitalRead(LEFT_B_IN)){}

  //Set up borders
  display.clearDisplay();
  display.fillRect(0,0,128,1,SSD1306_WHITE);
  display.fillRect(0,63,128,1,SSD1306_WHITE);
  display.fillRect(0,0,2,64,SSD1306_WHITE);
  display.fillRect(126,0,2,64,SSD1306_WHITE);
  drawSquare(0,0,1);
  drawSquare(0,1,1);
  display.display();
}

bool R,L;

//Game loop
void loop() {
  makeSnake();
  R = digitalRead(RIGHT_B_IN);
  L = digitalRead(LEFT_B_IN);

  if (L){ 
    display.invertDisplay(true);
  }
  if (R){
    display.invertDisplay(false);
  }

}

