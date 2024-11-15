#include <LiquidCrystal_I2C.h>

const int pinUpButton = 11;
const int pinDownButton = 12;
const int pinBuzzer = 10;

LiquidCrystal_I2C lcd(0x27, 20, 4);

byte bird[8] = {
  0b00000,
  0b00000,
  0b01110,
  0b11110,
  0b11111,
  0b01110,
  0b00000,
  0b00000
};

byte birdUp[8] = {
  0b00000,
  0b01110,
  0b11110,
  0b11111,
  0b01110,
  0b00000,
  0b00000,
  0b00000
};

byte birdDown[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b01110,
  0b11110,
  0b11111,
  0b01110,
  0b00000
};

byte pipeFull[8] = {
  0b01110,
  0b01110,
  0b01110,
  0b01110,
  0b01110,
  0b01110,
  0b01110,
  0b01110
};

byte pipeTopFull[8] = {
  0b01110,
  0b01110,
  0b01110,
  0b01110,
  0b01110,
  0b01110,
  0b01110,
  0b11111
};

byte pipeBottomFull[8] = {
  0b11111,
  0b01110,
  0b01110,
  0b01110,
  0b01110,
  0b01110,
  0b01110,
  0b01110
};

byte dizzy0[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00110,
  0b10001,
  0b01100
};

byte dizzy1[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b01010,
  0b10001,
  0b01010
};

byte dizzy2[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b01100,
  0b10001,
  0b00110
};


// Game Variable
unsigned long startScreenPreviousMillis = 0;
unsigned long birdPreviousMillis = 0;
unsigned long buttonPreviousMillis = 0;
unsigned long obstaclePreviousMillis = 0;
unsigned long dizzyPreviousMillis = 0;

int birdChangingCount = 0;
int dizzyChangingCount = 0;
int birdXPosition = 3;
int birdYPosition = 2;
int speed = 500;
int stepCount = 0;
int score = 0;

bool intro = false;
bool start = false;
bool gameOver = false;

int freeSpace[20] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 2, -1};


void clearSingle(int column, int row) {
  lcd.setCursor(column, row);
  lcd.write(0x20);
}

void clearArea(int columnStart, int columnEnd, int rowStart, int rowEnd) {
  for (int row = rowStart; row <= rowEnd; row++) {
    for (int column = columnStart; column <= columnEnd; column++) {
      lcd.setCursor(column, row);
      lcd.write(0x20);
    }
  }
}

void birdAnimation(int decider) {
  if (decider % 2 == 0) lcd.createChar(0, bird);
  else if (decider % 3 == 0) lcd.createChar(0, birdDown);
  else lcd.createChar(0, birdUp);
}

void introduction() {
  lcd.setCursor(1, 1);
  lcd.print("Flappy Bird");
  delay(1000);

  for (int i = 1; i < 20; i++) {
    if (i % 2 == 0) lcd.createChar(0, bird);
    else if (i % 3 == 0) lcd.createChar(0, birdDown);
    else lcd.createChar(0, birdUp);

    lcd.setCursor(i, 1);
    lcd.write((byte) 0);

    if (i > 1) {
      lcd.setCursor(i-1, 1);
      lcd.write(0x20);
    }

    delay(250);
  }

  lcd.clear();
  String txt1 = "Project by";
  String txt2 = "Kelompok 4";

  int txt_x = 4;
  for (char ch : txt1) {
    lcd.setCursor(txt_x, 1);
    lcd.print(ch);

    delay(50);
    txt_x++;
  }

  txt_x = 5;
  for (char ch : txt2) {
    lcd.setCursor(txt_x, 2);
    lcd.print(ch);

    delay(100);
    txt_x++;
  }
  delay(1000);

  lcd.clear();
}

void startScreen() {
  lcd.setCursor(6, 1);
  lcd.print("Press button");
  lcd.setCursor(6, 2);
  lcd.print("to play!");
}

void createObstacle(int freeSpace, int x) {
  int pipeBottom = 3 - freeSpace;
  int pipeTop = 3 - pipeBottom;

  lcd.setCursor(x, 0);
  if (pipeTop == 0) lcd.write(0x20);
  else lcd.write((byte) (pipeTop > 1 ? 1 : 2)); 

  lcd.setCursor(x, 1);
  if (pipeTop == 1) lcd.write(0x20);
  else lcd.write((byte) (pipeTop > 2 ? 1 : pipeTop == 0 ? 3 : 2));

  lcd.setCursor(x, 2);
  if (pipeBottom == 1) lcd.write(0x20);
  else lcd.write((byte) (pipeBottom > 2 ? 1 : pipeBottom == 0 ? 2 : 3));

  lcd.setCursor(x, 3);
  if (pipeBottom == 0) lcd.write(0x20);
  else lcd.write((byte) (pipeBottom > 1 ? 1 : 3));
}

void reset() {
  gameOver = false;
  start = false;
  stepCount = 0;
  score = 0;
  birdYPosition = 2;
  birdXPosition = 1;

  for (int i = 0; i < 20; i++) freeSpace[i] = -1;

  freeSpace[18] = 2;
  lcd.clear();
}

void gameOverDisplay() {
  lcd.setCursor(6, 1);
  lcd.print("Game Over!");
  lcd.setCursor(6, 2);
  lcd.print("score: ");
  lcd.setCursor(13, 2);
  lcd.print(score);
}

void dizzyAnimation(int decider) {
  if (decider % 2 == 0) lcd.createChar(4, dizzy1);
  else if (decider % 3 == 0) lcd.createChar(4, dizzy2);
  else lcd.createChar(4, dizzy0);
}

void gameOverChecker() {
  if (freeSpace[1] != birdYPosition && freeSpace[1] != -1) {
    birdXPosition = 0;
    gameOver = true;
    start = false;
    clearArea(3, 19, 0, 3);
    
    if (birdYPosition > 0) {
      lcd.setCursor(birdXPosition, birdYPosition-1);
      lcd.write((byte) 4);
    }
    
     tone(pinBuzzer, 600, 150); // Nada pertama 600 Hz selama 150ms
  delay(150); // Jeda singkat setelah nada pertama
  
  tone(pinBuzzer, 500, 150); // Nada kedua 500 Hz selama 150ms
  delay(150); // Jeda singkat setelah nada kedua
  
  tone(pinBuzzer, 400, 150); // Nada ketiga 400 Hz selama 150ms
  delay(150); // Jeda singkat setelah nada ketiga
  
  tone(pinBuzzer, 300, 150); // Nada keempat 300 Hz selama 150ms
  delay(150); // Jeda singkat setelah nada keempat


    noTone(pinBuzzer);
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(pinUpButton, INPUT_PULLUP);
  pinMode(pinDownButton, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();

  lcd.createChar(0, bird);
  lcd.createChar(1, pipeFull);
  lcd.createChar(2, pipeTopFull);
  lcd.createChar(3, pipeBottomFull);
  lcd.createChar(4, dizzy0);

  randomSeed(analogRead(0));
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentMillis = millis();

  if (!intro) introduction();
  else {
    if (currentMillis - birdPreviousMillis >= 100 && !gameOver) {
      birdPreviousMillis = currentMillis;
      birdAnimation(birdChangingCount);
      birdChangingCount++;
    }
    if (currentMillis - dizzyPreviousMillis >= 200 && gameOver) {
      dizzyPreviousMillis = currentMillis;
      dizzyAnimation(dizzyChangingCount);
      dizzyChangingCount++;
    }
  }
  intro = true;
  
  // display bird
  lcd.setCursor(birdXPosition, birdYPosition);
  lcd.write((byte) 0);

  if (stepCount % 6 == 0 && stepCount != 0) freeSpace[19] = random(0, 4);

  if (start) {
    if (score >= 100) {
      lcd.setCursor(17, 0);
      lcd.print(score);
    } else {
      if (score >= 10) {
        lcd.setCursor(17, 0);
        lcd.print("0");
        lcd.setCursor(18, 0);
        lcd.print(score);
      } else {
        lcd.setCursor(17, 0);
        lcd.print("00");
        lcd.setCursor(19, 0);
        lcd.print(score);
      }
    }

    if (currentMillis - obstaclePreviousMillis >= speed) {
      obstaclePreviousMillis = currentMillis;

      if (stepCount > 0) {
        for (int i = 0; i < 20; i++) {
          if (i != 0) {
            freeSpace[i-1] = freeSpace[i];
            freeSpace[i] = -1;
          }
        }
      }

      for (int i = 0; i < 20; i++) {
        if (freeSpace[i] != -1) {
          clearArea(i+1, i+1, 0, 3);
          createObstacle(freeSpace[i], i);
        }
        if (i == 0 && freeSpace[i] == -1) clearArea(i, i, 0, 3);
      }

      if (score >= 10) speed = 400;
      else if (score >= 15) speed = 300;
      else if (score >= 20) speed = 250;
      else if (score >= 30) speed = 100;
      else if (score >= 50) speed = 50;

      if (freeSpace[1] == birdYPosition) score++;

      stepCount++;
    }
    
    gameOverChecker();
    
  } else if (gameOver) {
    gameOverDisplay();
    createObstacle(freeSpace[1], 1);
  } else {
    if (currentMillis - startScreenPreviousMillis >= 750) {
      startScreenPreviousMillis = currentMillis;
      clearArea(6, 19, 1, 2);
    }
    startScreen();
  }
  
  bool buttonUpClicked = digitalRead(pinUpButton) == LOW ? true : false;
  bool buttonDownClicked = digitalRead(pinDownButton) == LOW ? true : false;

  if (currentMillis - buttonPreviousMillis >= 200) {
    if ((!start && !gameOver && buttonUpClicked) || (!start && !gameOver && buttonDownClicked)) {
      start = true;
      birdXPosition = 1;
      clearArea(3, 19, 1, 2);
      buttonPreviousMillis = currentMillis;
    } else if ((start && buttonUpClicked) || (start && buttonDownClicked)) {
      clearSingle(1,birdYPosition);
      if (buttonUpClicked && birdYPosition > 0 && !gameOver) birdYPosition--;
      if (buttonDownClicked && birdYPosition < 3 && !gameOver) birdYPosition++;
      buttonPreviousMillis = currentMillis;
      tone(pinBuzzer, 800, 100);
    } else if ((gameOver && buttonUpClicked) || (gameOver && buttonDownClicked)) {
      reset();
      buttonPreviousMillis = currentMillis;
    }
  }
}
