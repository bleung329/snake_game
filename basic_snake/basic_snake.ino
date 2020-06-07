#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//DISPLAY THINGS
#define OLED_RESET    4   // Reset pin # (or -1 if sharing Arduino reset pin)
#define OLED_ADDRESS  0x3C  // I2C address of the display.
#define SCREEN_WIDTH  128   // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels

//Coordinate struct
typedef struct
{
  byte x;
  byte y;
} coord;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//A 128x64 board reduced to 126x60
//Draw square on the 21x10 board
//x is between 0 and 20 inclusive
//y is between 0 and 9 inclusive
//thing: 1 = snake, 2 = food, 0 = erase
void drawSquare(byte x, byte y, byte thing)
{
  if (thing == 1){
      display.fillRect(6*x+2,6*y+3,4,4,WHITE);  
      return;
  }
  if (thing == 2){
      display.drawRect(6*x+2,6*y+3,4,4,WHITE);
      return;
  }
  //Serial.println(F("Fired"));
  display.fillRect(6*x+2,6*y+3,4,4,BLACK);
}

//BUTTON THINGS
#define LEFT_B_IN     A0    // A0
#define LEFT_B_OUT    A2    // A2   
#define RIGHT_B_IN    A1    // A1
#define RIGHT_B_GND   A3    // A3
#define RIGHT_B_OUT   13   // D13

//ACTUAL GAME STUFF

#define CYCLE_INTERVAL 500
#define BUTTON_INTERVAL 400

unsigned long previousTime = 0;

//SNAKE THINGS
coord snake[100];
//Apparently snake[] took up so much space that it interferes with the OLED
//Keep it a reasonable size.
short directions[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};
short dirIndex = 0;
byte snakeLength = 2;

coord foodCoord;

//Creates the snake with an initial length of 2
//and an initial direction right.
void makeSnake()
{
  snake[0] = {1, (byte) random(0,10)};
  snake[1] = {0, snake[0].y};
  drawSquare(snake[0].x,snake[0].y,1);
  drawSquare(snake[1].x,snake[1].y,1);
}

//Modify direction according to button press
bool R,L;
void redirect()
{
  unsigned long tempTime = millis();
  R = false;
  L = false;
  //Listen for button presses
  while (millis()-tempTime < BUTTON_INTERVAL)
  {
      Serial.println(millis()-tempTime);

    if (digitalRead(LEFT_B_IN)){L = true;}
    if (digitalRead(RIGHT_B_IN)){R = true;}
  }
  //Ignore double presses and non presses
  if ((!R && !L) || (R && L)){
    return;
  }
  //If right turn, increment direction index
  if (R){
    dirIndex++;
    if (dirIndex > 3){dirIndex = 0;}
    return;
  }
  //If left turn, decrement direction index
  dirIndex--;
  if (dirIndex < 0){dirIndex = 3;}
}

//Moves the snake
bool moveSnake()
{
  int x = snake[0].x+directions[dirIndex][0];
  int y = snake[0].y+directions[dirIndex][1];

  //If out of bounds, exit and lose.
  if (x > 20 || x < 0 || y > 9 || y < 0)
  {
    return 1;
  }

  coord newHead = {byte(x),byte(y)};
  drawSquare(newHead.x,newHead.y,1);
  bool food = false;
  
  //Shift snake back
  for (int i = snakeLength; i != 0; --i)
  {
    snake[i] = snake[i-1];
  }
  snake[0] = newHead;
  //Erase tail
  drawSquare(snake[snakeLength].x,snake[snakeLength].y,0);
  return 0;
}
  /*
  //If food eaten, increment snakeLength
  if (newHead.y == foodCoord.y && newHead.x == foodCoord.x)
  {
    food = true;
    snakeLength++;
  }
  //Shift the snake
  for (int i = snakeLength-1; i > 0; --i)
  {
    //If self contact, LOSE
    if (!food && newHead.y == snake[i].y && newHead.x == snake[i].x)
    {
      return 1;
    }
    snake[i] = snake[i-1];
  }

  //If no food, erase the tail.
  if (!food)
  {
    snake[snakeLength-1] = {255,255};
    drawSquare(snake[snakeLength-1].x,snake[snakeLength-1].y,0);
  }
  else
  {
    Serial.println(F("Fired"));
    //Just continue, ignore the tail is behind you
  }
  snake[0] = newHead;
  return 0;
  */


void putFood()
{
  bool foodOkay = false;
  //Make sure the food doesnt fall on top of the snake
  while (!foodOkay)
  {
    foodCoord = {byte(random(0,21)),byte(random(0,10))};
    foodOkay = true;
    for (byte i = 0; i < snakeLength; ++i)
    {
      if (foodCoord.y == snake[i].y && foodCoord.x == snake[i].x)
      {
        foodOkay = false;
        break;
      }
    }
  }
  drawSquare(foodCoord.x,foodCoord.y,2);
  display.display();
}

void setup() 
{
  Serial.begin(9600);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("Oh no"));
    for(;;);
  }

  //Random numbers
  randomSeed(analogRead(7));

  //Set up the buttons
  pinMode(LEFT_B_IN, INPUT);
  pinMode(LEFT_B_OUT, OUTPUT);
  digitalWrite(LEFT_B_OUT,1);

  pinMode(RIGHT_B_IN, INPUT);
  pinMode(RIGHT_B_GND, OUTPUT);
  pinMode(RIGHT_B_OUT, OUTPUT);
  digitalWrite(RIGHT_B_OUT,1);
  digitalWrite(RIGHT_B_GND,0);

  
  //Set up the title screen
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(20,5);
  display.println(F("SNAKE"));

  display.setTextSize(1);
  display.setCursor(26,40);
  display.println(F("Hit L to play"));
  display.display();
  
  //Wait for user input
  while (!digitalRead(LEFT_B_IN)){}

  //Set up borders
  display.clearDisplay();
  display.fillRect(0,0,128,2,WHITE);
  display.fillRect(0,62,128,2,WHITE);
  display.fillRect(0,0,1,64,WHITE);
  display.fillRect(127,0,1,64,WHITE);
  display.display();  
}


//Game loop
void loop() {
  //GAME SETUP
  //Make the snake and place the food
  makeSnake();
  putFood();
  delay(400);

  //Start game
  for(;;)
  {
    if (millis() - previousTime > CYCLE_INTERVAL)
    {
      previousTime = millis();
      //Change direction
      redirect();
      //Self contact/Out of bounds condition
      if (moveSnake())
      {
        break;
      }
      display.display();
    }
  }
  
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(20,5);
  display.println(F("LOSE"));
  display.display();

  for(;;);
/*
  if (L){ 
    display.invertDisplay(true);
  }
  if (R){
    display.invertDisplay(false);
  }

*/
}

