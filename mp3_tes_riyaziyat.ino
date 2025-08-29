#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

#define BUTTON_D2 2
#define BUTTON_D3 3
#define BUTTON_D4 4  // Repeat question

int questionNumber = 0;   // 0 = intro, 1-5 = questions
int correctCount = 0;
int wrongCount = 0;
bool waitingForAnswer = false;

// Customize the text for each question
String leftText[5]   = {"17", "30", "6", "2500", "20"};
String rightText[5]  = {"15", "25", "24", "2000", "25"};
String middleText[5] = {"", "", "", "", ""}; // blank by default

void setup() {
  pinMode(BUTTON_D2, INPUT_PULLUP);
  pinMode(BUTTON_D3, INPUT_PULLUP);
  pinMode(BUTTON_D4, INPUT_PULLUP);

  mySoftwareSerial.begin(9600);
  if (!myDFPlayer.begin(mySoftwareSerial)) while(true);
  myDFPlayer.volume(23);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) while(true);
  display.clearDisplay();
  display.display();

  // Play intro sound (File 6)
  myDFPlayer.play(6);
}

void loop() {
  // Wait for any button to start quiz
  if (questionNumber == 0 && (digitalRead(BUTTON_D2) == LOW || digitalRead(BUTTON_D3) == LOW)) {
    questionNumber = 1;
    delay(200);
    playQuestion();
  }

  // Handle answers
  if (waitingForAnswer) {
    if (digitalRead(BUTTON_D2) == LOW) { checkAnswer(BUTTON_D2); delay(200); }
    if (digitalRead(BUTTON_D3) == LOW) { checkAnswer(BUTTON_D3); delay(200); }
    if (digitalRead(BUTTON_D4) == LOW) { replayQuestion(); delay(200); } // Repeat question
  }
}

void playQuestion() {
  myDFPlayer.play(questionNumber); // Play file 1-5
  waitingForAnswer = true;
  drawQuestionScreen();
}

void drawQuestionScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 30);   
  display.print(leftText[questionNumber-1]);
  
  display.setCursor(100, 30); 
  display.print(rightText[questionNumber-1]);

  display.setCursor(50, 30);  
  display.print(middleText[questionNumber-1]);

  display.display();
}

void replayQuestion() {
  if (questionNumber >= 1 && questionNumber <= 5) {
    myDFPlayer.play(questionNumber); 
  }
}

void checkAnswer(int button) {
  bool isCorrect = false;

  // correct sequence: Q1->D3, Q2->D2, Q3->D3, Q4->D2, Q5->D3
  if (questionNumber == 1 && button == BUTTON_D2) isCorrect = true;
  if (questionNumber == 2 && button == BUTTON_D3) isCorrect = true;
  if (questionNumber == 3 && button == BUTTON_D3) isCorrect = true;
  if (questionNumber == 4 && button == BUTTON_D2) isCorrect = true;
  if (questionNumber == 5 && button == BUTTON_D3) isCorrect = true;

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(50, 25);
  if (isCorrect) {
    display.print("duzgun");
    correctCount++;
    myDFPlayer.play(7); // correct sound
  } else {
    display.print("yanlis");
    wrongCount++;
    myDFPlayer.play(8); // wrong sound
  }
  display.display();
  delay(1000);

  questionNumber++;
  waitingForAnswer = false;

  if (questionNumber <= 5) playQuestion();
  else showResults();
}

void showResults() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(10, 20);
  display.print("Correct: "); display.print(correctCount);
  display.setCursor(10, 40);
  display.print("Wrong: "); display.print(wrongCount);
  display.display();
  delay(5000);

  // Reset game
  questionNumber = 0;
  correctCount = 0;
  wrongCount = 0;
  display.clearDisplay();
  display.display();
  myDFPlayer.play(6); // replay intro
}
