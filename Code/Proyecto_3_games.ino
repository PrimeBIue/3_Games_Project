
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Define Pins
#define OLED_RESET 4
#define BEEPER 13
#define CONTROL_A A0
#define CONTROL_B A1


//Define Visuals
#define FONT_SIZE 2
#define SCREEN_WIDTH 127  //real size minus 1, because coordinate system starts with 0
#define SCREEN_HEIGHT 63  //real size minus 1, because coordinate system starts with 0
#define PADDLE_WIDTH 4
#define PADDLE_HEIGHT 10
#define PADDLE_PADDING 10
#define BALL_SIZE 3
#define SCORE_PADDING 10

#define EFFECT_SPEED 0.5
#define MIN_Y_SPEED 0.5
#define MAX_Y_SPEED 2


//Define Variables
Adafruit_SSD1306 display(OLED_RESET);
const int MAX_LEVEL = 100;
int sequence[MAX_LEVEL];
int your_sequence[MAX_LEVEL];
int level = 1;

int velocity = 1000;
int paddleLocationA = 0;
int paddleLocationB = 0;

float ballX = SCREEN_WIDTH/2;
float ballY = SCREEN_HEIGHT/2;
float ballSpeedX = 2;
float ballSpeedY = 1;

int lastPaddleLocationA = 0;
int lastPaddleLocationB = 0;

int scoreA = 0;
int scoreB = 0;
int sel=0;
int sel2=0;


//Setup 
void setup() 
{
  pinMode(0,INPUT_PULLUP);
  pinMode(5,INPUT_PULLUP);//green
  pinMode(6,INPUT_PULLUP);//red
  pinMode(7,INPUT_PULLUP);//start button and yellow
  pinMode(8,INPUT_PULLUP);//blue

  pinMode(9,OUTPUT);//greenn
  pinMode(10,OUTPUT);//red
  pinMode(11,OUTPUT);//yellow
  pinMode(12,OUTPUT);//blue

  digitalWrite(9, LOW);
digitalWrite(10, LOW);
digitalWrite(11, LOW);
digitalWrite(12, LOW);


  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.clearDisplay();   // clears the screen and buffer
  display.display();   
  display.setTextWrap(false);

    splash();
  
  display.setTextColor(WHITE);
  display.setTextSize(FONT_SIZE);
  display.clearDisplay(); 
}

//Splash Screen
void splash()
{
 
do{
  

  display.clearDisplay(); 

  display.setTextColor(WHITE);
  centerPrint("3 GAMES",0,3);
  centerPrint("By Jorge Cabiedes",24,1);
  centerPrint("& Camila Ramirez",33,1);


  display.fillRect(0,SCREEN_HEIGHT-10,SCREEN_WIDTH,10,WHITE);
  display.setTextColor(BLACK);
  centerPrint("Press any button!",SCREEN_HEIGHT-9,1);

  display.display();
 


 
  }while (digitalRead(5) == HIGH&&digitalRead(6) == HIGH&&digitalRead(7) == HIGH&&digitalRead(8) == HIGH&&digitalRead(0) == HIGH);

  soundStart();
  menu();
}
void menu(){
 do{
  
 
  display.clearDisplay();
if(sel2>2){
  sel2=2;
}
else if(sel2<0){
  sel2=0;
}
if(digitalRead(6)==LOW){
  sel2++;  
  delay(200);
}
else if(digitalRead(5)==LOW){
  sel2--;  
  delay(200);
}
if(sel2==0){
display.setTextColor(WHITE);
centerPrint("SIMON SAYS",25,1);
centerPrint("STOP LIGHT",45,1);

display.fillRect(0,4,SCREEN_WIDTH,10,WHITE);
display.setTextColor(BLACK);
centerPrint("PONG",5,1);
display.display();
}
else if(sel2==1){
display.setTextColor(WHITE);
centerPrint("STOP LIGHT",45,1);
centerPrint("PONG",5,1);

display.fillRect(0,24,SCREEN_WIDTH,10,WHITE);
display.setTextColor(BLACK);
centerPrint("SIMON SAYS",25,1);
display.display();
}
else if(sel2==2){
display.setTextColor(WHITE);
centerPrint("SIMON SAYS",25,1);
centerPrint("PONG",5,1);

display.fillRect(0,44,SCREEN_WIDTH,10,WHITE);
display.setTextColor(BLACK);
centerPrint("STOP LIGHT",45,1);
display.display();
}
if(digitalRead(7)==LOW&&sel2==0){
  sel=1;
  pong();
}
if(digitalRead(7)==LOW&&sel2==1){
  sel=2;
  simon();
}
if(digitalRead(7)==LOW&&sel2==2){
  sel=3;
  error();
}


 }while(sel==0);



  
}
//Loop
void loop()
{
if(sel==1){
  pong();
}
else if(sel==2){
  simon();     
}
else if(sel==3){
  error();     
}

}

void calculateMovement() 
{
 
  int controlA = analogRead(CONTROL_A);
  int controlB = analogRead(CONTROL_B);

  paddleLocationA = map(controlA, 0, 1024, 0, SCREEN_HEIGHT - PADDLE_HEIGHT);
  paddleLocationB = map(controlB, 0, 1024, 0, SCREEN_HEIGHT - PADDLE_HEIGHT);

  int paddleSpeedA = paddleLocationA - lastPaddleLocationA;
  int paddleSpeedB = paddleLocationB - lastPaddleLocationB;

  ballX += ballSpeedX;
  ballY += ballSpeedY;

  //bounce from top and bottom
  if (ballY >= SCREEN_HEIGHT - BALL_SIZE || ballY <= 0) {
    ballSpeedY *= -1;
    soundBounce();
  }

  //bounce from paddle A
  if (ballX >= PADDLE_PADDING && ballX <= PADDLE_PADDING+BALL_SIZE && ballSpeedX < 0) {
    if (ballY > paddleLocationA - BALL_SIZE && ballY < paddleLocationA + PADDLE_HEIGHT) {
      soundBounce();
      ballSpeedX *= -1;
    
      addEffect(paddleSpeedA);
    }

  }

  //bounce from paddle B
  if (ballX >= SCREEN_WIDTH-PADDLE_WIDTH-PADDLE_PADDING-BALL_SIZE && ballX <= SCREEN_WIDTH-PADDLE_PADDING-BALL_SIZE && ballSpeedX > 0) {
    if (ballY > paddleLocationB - BALL_SIZE && ballY < paddleLocationB + PADDLE_HEIGHT) {
      soundBounce();
      ballSpeedX *= -1;
    
      addEffect(paddleSpeedB);
    }

  }

  //score points if ball hits wall behind paddle
  if (ballX >= SCREEN_WIDTH - BALL_SIZE || ballX <= 0) {
    if (ballSpeedX > 0) {
      scoreA++;
      ballX = SCREEN_WIDTH / 4;
    }
    if (ballSpeedX < 0) {
      scoreB++;
      ballX = SCREEN_WIDTH / 4 * 3;
    }

    soundPoint();   
  }

  //set last paddle locations
  lastPaddleLocationA = paddleLocationA;
  lastPaddleLocationB = paddleLocationB;  
}

void draw() 
{

  display.clearDisplay(); 

  //draw paddle A
  display.fillRect(PADDLE_PADDING,paddleLocationA,PADDLE_WIDTH,PADDLE_HEIGHT,WHITE);

  //draw paddle B
  display.fillRect(SCREEN_WIDTH-PADDLE_WIDTH-PADDLE_PADDING,paddleLocationB,PADDLE_WIDTH,PADDLE_HEIGHT,WHITE);

  //draw center line
  for (int i=0; i<SCREEN_HEIGHT; i+=4) {
    display.drawFastVLine(SCREEN_WIDTH/2, i, 2, WHITE);
  }

  //draw ball
  display.fillRect(ballX,ballY,BALL_SIZE,BALL_SIZE,WHITE);

  //print scores

  //backwards indent score A. This is dirty, but it works ... ;)
  int scoreAWidth = 5 * FONT_SIZE;
  if (scoreA > 9) scoreAWidth += 6 * FONT_SIZE;
  if (scoreA > 99) scoreAWidth += 6 * FONT_SIZE;
  if (scoreA > 999) scoreAWidth += 6 * FONT_SIZE;
  if (scoreA > 9999) scoreAWidth += 6 * FONT_SIZE;

  display.setCursor(SCREEN_WIDTH/2 - SCORE_PADDING - scoreAWidth,0);
  display.print(scoreA);

  display.setCursor(SCREEN_WIDTH/2 + SCORE_PADDING+1,0); //+1 because of dotted line.
  display.print(scoreB);
  
  display.display();
} 

void addEffect(int paddleSpeed)
{
  float oldBallSpeedY = ballSpeedY;

  //add effect to ball when paddle is moving while bouncing.
  //for every pixel of paddle movement, add or substact EFFECT_SPEED to ballspeed.
  for (int effect = 0; effect < abs(paddleSpeed); effect++) {
    if (paddleSpeed > 0) {
      ballSpeedY += EFFECT_SPEED;
    } else {
      ballSpeedY -= EFFECT_SPEED;
    }
    
  }

  //limit to minimum speed
  if (ballSpeedY < MIN_Y_SPEED && ballSpeedY > -MIN_Y_SPEED) {
    if (ballSpeedY > 0) ballSpeedY = MIN_Y_SPEED;
    if (ballSpeedY < 0) ballSpeedY = -MIN_Y_SPEED;
    if (ballSpeedY == 0) ballSpeedY = oldBallSpeedY;
  }

  //limit to maximum speed
  if (ballSpeedY > MAX_Y_SPEED) ballSpeedY = MAX_Y_SPEED;
  if (ballSpeedY < -MAX_Y_SPEED) ballSpeedY = -MAX_Y_SPEED;
}

void soundStart() 
{
  tone(BEEPER, 250);
  delay(100);
  tone(BEEPER, 500);
  delay(100);
  tone(BEEPER, 1000);
  delay(100);
  noTone(BEEPER);
}

void soundBounce() 
{
  tone(BEEPER, 500, 50);
}

void soundPoint() 
{
  tone(BEEPER, 150, 150);
}

void centerPrint(char *text, int y, int size)
{
  display.setTextSize(size);
  display.setCursor(SCREEN_WIDTH/2 - ((strlen(text))*6*size)/2,y);
  display.print(text);
}



void pong(){

  calculateMovement();
    draw();
} 

void simon(){
  int scoreAWidth = 5 * FONT_SIZE;
  display.clearDisplay();
  display.setTextColor(WHITE);
  centerPrint("SIMON SAYS",10,2);
  if(level==1){
    centerPrint("Press start!",SCREEN_HEIGHT-9,1);
    display.display();
  }
  else{
display.setCursor(80 - SCORE_PADDING - scoreAWidth,50);
  display.print(level);

centerPrint("LVL",35,1.5);
    
    display.display();
  }
  
  
if (level == 1)
generate_sequence();//generate a sequence;

if (digitalRead(0) == LOW || level != 1) //If start button is pressed or you're winning
{
show_sequence();    //show the sequence
get_sequence();     //wait for your sequence
}
}

void show_sequence()
{
digitalWrite(9, LOW);
digitalWrite(10, LOW);
digitalWrite(11, LOW);
digitalWrite(12, LOW);

for (int i = 0; i < level; i++)
{
  Serial.println(sequence[i]);
digitalWrite(sequence[i], HIGH);
delay(velocity);
digitalWrite(sequence[i], LOW);
delay(200);
}
}

void get_sequence()
{
 //lvl
 
int flag = 0; //this flag indicates if the sequence is correct

for (int i = 0; i < level; i++)
{
flag = 0;
while(flag == 0)
{
if (digitalRead(5) == LOW)
{
  
digitalWrite(12, HIGH);
your_sequence[i] = 12;
flag = 1;
delay(500);
if (your_sequence[i] != sequence[i])
{
wrong_sequence();
return;
}
digitalWrite(12, LOW);
}

if (digitalRead(6) == LOW)
{
 
digitalWrite(11, HIGH);
your_sequence[i] = 11;
flag = 1;
delay(500);
if (your_sequence[i] != sequence[i])
{
wrong_sequence();
return;
}
digitalWrite(11, LOW);
}

if (digitalRead(7) == LOW)
{
  
digitalWrite(10, HIGH);
your_sequence[i] = 10;
flag = 1;
delay(500);
if (your_sequence[i] != sequence[i])
{
wrong_sequence();
return;
}
digitalWrite(10, LOW);
}

if (digitalRead(8) == LOW)
{
 
digitalWrite(9, HIGH);
your_sequence[i] = 9;
flag = 1;
delay(500);
if (your_sequence[i] != sequence[i])
{
wrong_sequence();
return;
}
digitalWrite(9, LOW);
}

}
}
right_sequence();
}

void generate_sequence()
{
randomSeed(millis()); //in this way is really random!!!

for (int i = 0; i < MAX_LEVEL; i++)
{
sequence[i] = random(9,13);
}
}
void wrong_sequence()
{
for (int i = 0; i < 3; i++)
{
digitalWrite(9, HIGH);
digitalWrite(10, HIGH);
digitalWrite(11, HIGH);
digitalWrite(12, HIGH);
delay(250);
digitalWrite(9, LOW);
digitalWrite(10, LOW);
digitalWrite(11, LOW);
digitalWrite(12, LOW);
delay(250);
}
level = 1;
velocity = 1000;
}

void right_sequence()
{
digitalWrite(9, LOW);
digitalWrite(10, LOW);
digitalWrite(11, LOW);
digitalWrite(12, LOW);
delay(250);

digitalWrite(9, HIGH);
digitalWrite(10, HIGH);
digitalWrite(11, HIGH);
digitalWrite(12, HIGH);
delay(500);
digitalWrite(9, LOW);
digitalWrite(10, LOW);
digitalWrite(11, LOW);
digitalWrite(12, LOW);
delay(500);

if (level < MAX_LEVEL);
level++;

velocity -= 50; //increase difficulty
}

void error(){
  display.clearDisplay();
  centerPrint("ERROR",10,2);
  centerPrint("Game unavailable",30,1);
  display.display();

}

