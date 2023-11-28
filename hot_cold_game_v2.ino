#define BUTTON_RESET 8
#define BUTTON_MINUS 2
#define BUTTON_ENTER 3
#define BUTTON_PLUS 4
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x3F for a 16 chars and 2 line display

// ============================ CUSTOM CHARS =======================
byte snowflakeChar1[8] = {
  0b10101,
  0b01001,
  0b10101,
  0b00011,
  0b00011,
  0b10101,
  0b01001,
  0b10101
};

byte snowflakeChar2[8] = {
  0b10101,
  0b10010,
  0b10101,
  0b11000,
  0b11000,
  0b10101,
  0b10010,
  0b10101
};

byte flameChar[8] = {
  0b00011,
  0b00001,
  0b01011,
  0b11011,
  0b11110,
  0b01100,
  0b01110,
  0b11111
};

byte FaceLeft[8] = {
  B10001,
  B11010,
  B01110,
  B01110,
  B00110,
  B00110,
  B00010,
  B00001
};

byte FaceRight[8] = {
  B10001,
  B01011,
  B01110,
  B01110,
  B01100,
  B01100,
  B01000,
  B10000
};

byte FaceMiddle[8] = {
  B00000,
  B11011,
  B11011,
  B00100,
  B00000,
  B11111,
  B01010,
  B00100
};

// ========================== VARIABLES ===========================
int number = 0;    // randomly generated number
int selected = 0;  // player-number confirmed by player
int marker = 0;    // player-number not confirmed by player

bool gameStart;  // for "press any button to start" style prompt
bool inputMode;  // player is in player-number selection mode

bool flashing = true;         // display if true
unsigned long prevMilis = 0;  // counters for handling flashing without delaying the board
unsigned long currMilis = 0;


void setup() {
  pinMode(BUTTON_MINUS, OUTPUT);
  pinMode(BUTTON_PLUS, OUTPUT);
  pinMode(BUTTON_ENTER, OUTPUT);
  pinMode(BUTTON_RESET, OUTPUT);
  Serial.begin(9600);

  lcd.init();

  // Make sure backlight is on
  lcd.backlight();
  lcd.clear();

  lcd.createChar(0, snowflakeChar1);
  lcd.createChar(1, snowflakeChar2);
  lcd.createChar(2, flameChar);
  lcd.createChar(3, FaceLeft);
  lcd.createChar(4, FaceMiddle);
  lcd.createChar(5, FaceRight);

  // generate random number here
  gameStart = false;
}


void loop() {

  flashing_handler();

  if (gameStart) {

    // ==================== GAME STARTED ======================

    if (digitalRead(BUTTON_MINUS) == HIGH) {
      inputMode = true;
      input_minus(BUTTON_MINUS);

      Serial.println("Current choice ");
      Serial.print(marker);
      Serial.println("");

    } else if (digitalRead(BUTTON_PLUS) == HIGH) {
      inputMode = true;
      input_plus(BUTTON_PLUS);

      Serial.println("Current choice ");
      Serial.print(marker);
      Serial.println("");
    } else if (digitalRead(BUTTON_ENTER) == HIGH) {
      inputMode = false;
      input_confirm(BUTTON_ENTER);

      Serial.println("Confirmed choice ");
      Serial.print(marker);
      Serial.println("");
    }

    // ================= PRINT TO LCD =======================
    if (inputMode) {
      print_marker(marker);
    } else {
      print_selected(marker);
    }

    if (number > selected) {
      // show cold
      print_cold();

    } else if (number < selected) {
      // show hot
      print_hot();

    } else if (number == selected) {
      // show jackpot
      print_jackpot();
    }

  } else {
    // ======================= BEFORE GAME ============================
    if (flashing) {
      lcd.setCursor(1, 0);
      lcd.print("PRESS RESET TO");
      lcd.setCursor(3, 1);
      lcd.print("START GAME");
    }
  }

  // Reset/Start Game
  if (digitalRead(BUTTON_RESET) == HIGH) {
    gameStart = true;
    // delay(500);
    reset_game();
  }
}

// =============== FLASHING HANDLER =================

void flashing_handler() {
  currMilis = millis();
  if (currMilis - prevMilis >= 500) {
    prevMilis = currMilis;
    if (flashing) {
      flashing = false;
      lcd.clear();
    } else {
      flashing = true;
    }
  }
}

// =============== PRINT METHODS =================

void print_selected(int m) {
  lcd.setCursor(7, 0);
  lcd.print(m);
}

void print_marker(int m) {
  if (flashing) {
    lcd.setCursor(7, 0);
    lcd.print(m);
  }
}

void print_cold() {
  lcd.setCursor(1, 1);
  lcd.write(0);
  lcd.write(1);
  lcd.print(" TOO COLD ");
  lcd.write(0);
  lcd.write(1);
}

void print_hot() {
  lcd.setCursor(3, 1);
  lcd.write(2);
  lcd.print(" TOO HOT ");
  lcd.write(2);
}

void print_jackpot() {
  String GS = "GLORIOUS SUCCESS";
  lcd.setCursor(0, 1);
  if (flashing) { 
    lcd.print(GS); 

    lcd.setCursor(1, 0);
    lcd.write(3); lcd.write(4); lcd.write(5);
    lcd.setCursor(12, 0);
    lcd.write(3); lcd.write(4); lcd.write(5);
  }

}

// =============== INPUT METHODS =================


void input_minus(int pin) {
  if (digitalRead(pin) == HIGH) {
    delay(30);

    while (digitalRead(pin) == HIGH) {
      marker--;
      print_selected(marker);
      Serial.println(marker);
      delay(100);
    }

    //while (digitalRead(pin) == HIGH) {}
    delay(30);
  }
}

void input_plus(int pin) {
  {
    delay(30);

    while (digitalRead(pin) == HIGH) {
      marker++;
      print_selected(marker);
      Serial.println(marker);
      delay(100);
    }

    //while (digitalRead(pin) == HIGH) {}
    delay(30);
  }
}

void input_confirm(int pin) {
  if (digitalRead(pin) == HIGH) {
    delay(50);

    selected = marker;
    print_selected(marker);

    while (digitalRead(pin) == HIGH) {}
    delay(50);
  }
}

void reset_game() {
  // Get the current time in milliseconds
  unsigned long currentTime = millis();
  // Use the current time as a seed for random number generation
  randomSeed(currentTime);
  number = random(100);
  // number = 0;
  marker = 0;
  selected = 0;

  Serial.print("GAME RESET.");
  delay(1000);
  Serial.print(".");
  delay(1000);
  Serial.println(".");
  delay(1000);
  Serial.println("current number to guess is: ");
  Serial.println(number);
}
