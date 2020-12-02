#include <Keypad.h>
#include "enums.h"
#include <TFT_HX8357.h> 
#define HX8357_GREY 0x2104 // Dark grey 16 bit colour

#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 480
#define X_OFFSET 6 // fix the bad installation work where its off ot the right by a bit
#define BG_COLOR HX8357_BLUE
#define FG_COLOR HX8357_WHITE
#define ROW_HEIGHT 68.6571

const byte rows = 4; //four rows
const byte cols = 4; //three columns
char keys[rows][cols] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};

byte colPins[rows] = {A7, A6, A5, A4}; 
byte rowPins[cols] = {A3, A2, A1, A0}; 


TFT_HX8357 tft = TFT_HX8357(); 
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );
State _st;
#define NUMBER_OF_FLOORS 58
#define BOTTOM_FLOOR -2

char floorsToCars[NUMBER_OF_FLOORS][27];

void cf(int floorNumber, char* cars) {
    int idx = floorNumber + abs(BOTTOM_FLOOR);
    strcpy(floorsToCars[idx], cars);
}


boolean isCarToTheLeft(char car) {
  return car >= 'A' && car <= 'K';
}

void configureBanks() {
   cf(-2, "QRSTUVWXYZ");
   cf(-1, "QRSTUVWXYZ");
   cf(1, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
   cf(2, "QRSTUVWXY");
   cf(3, "ABCDEFGHIJKLMNOPQRSTUVWXY");
   cf(4, "QRSTUVWXY");
   cf(5, "QRSTUVWXY");
   cf(6, "QRSTUVWXY");
   cf(7, "QRSTUVWXY");
   cf(8, "QRSTUVWXY");
   cf(9, "FGYQRSTUVWX");
   cf(10, "GHIJKLOPY");
   cf(11, "GHIJKLOPY");
   cf(12, "GHIJKLOPQRSTUVWXY");
   cf(14, "GHIJKLOPQRSTUVWXYZ");
   cf(15, "GHIJKLOPQRSTUVWXY");
   cf(16, "GHIJKLOPQRSTUVWXY");
   cf(17, "GHIJKLOPQRSTUVWXY");
   cf(18, "GHIJKLOPQRSTUVWXY");
   cf(19, "GHIJKLOPQRSTUVWY");
   cf(20, "GHIJKLMNOPQRSTUVWXZ");
   cf(21, "GHIJKLOPQRSTUVWXZ");
   cf(22, "GHIJKLOPQRSTUVWXZ");
   cf(23, "ABCDEGHIJKLMNOPQRSTUVWXZ");
   cf(24, "GHIJKLOPQRSTUVWXZ");
   cf(25, "GHIJKLOPQRSTUVWXZ");
   cf(26, "GHIJKLOPQRSTUVWXZ");
   cf(27, "GHIJKLOPQRSTUVWXZ");
   cf(28, "ABCDEFMNQRSTUVWXZ");
   cf(29, "ABCDEFMNQRSTUVWXZ");
   cf(30, "ABCDEFMNQRSTUVWXZ");
   cf(31, "ABCDEFMNQRSTUVWXZ");
   cf(32, "ABCDEFMNQRSTUVWXZ");
   cf(33, "ABCDEFMNQRSTUVWXZ");
   cf(34, "ABCDEFMNQRSTUVWXZ");
   cf(35, "ABCDEFMNQRSTUVWXZ");
   cf(36, "ABCDEFMNQRSTUVWX");
   cf(37, "ABCDEFMNQRSTUVWX");
   cf(38, "ABCDEFMNQRSTUVWX");
   cf(39, "ABCDEFMNQRSTUVWX");
   cf(40, "ABCDEFMNQRSTUVWX");
   cf(41, "ABCDEFMNQRSTUVWX");
   cf(42, "ABCDEFMNQRSTUVWX");
   cf(43, "ABCDEFMNQRSTUVWX");
   cf(44, "ABCDEFMNQRSTUVWX");
   cf(45, "ABCDEFMNQRSTUVWX");
   cf(46, "FGQRSTUVWX");
   cf(47, "FG");
   cf(48, "FG");
   cf(49, "FG");
   cf(50, "FG");
   cf(51, "FG");
   cf(52, "FG");
   cf(53, "FG");
   cf(54, "FG");
   cf(55, "FG");
   
}


void setup(void) {
  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(BG_COLOR);
  configureBanks();
}


void loop() {
   updateState(_st);
   renderState(_st);
}

void updateState(State &st) {
   switch(st.panelState) {
      case ACCEPTING_NEW_CALLS: 
         acceptNewCalls(st);
         break;
      case INPUT_IN_PROGRESS:
         acceptNewInputOrTimeout(st);
         break;
      case TELLING_CAR:
         waitForTellingCarTimeout(st);
         break;   
      case INVALID_SELECTION:
         waitForInvalidSelectionTimeout(st);
         break;
      case SHOWING_FLOOR_SERVES_CHART:
         waitForFloorServesChartTimeout(st);
         break;
      case DAD_MODE:
         waitForDadModeTimeout(st);
         break;      
   }
}

void waitForDadModeTimeout(State& st) {
  if(millis() - st.lastDadModeTime > 8000) {
     st.dirty = true;
     st.panelState = ACCEPTING_NEW_CALLS;
  }  
}

void interpretFloorInput(char* floorInput) {
  char* offsetForNegative;
  boolean isNegative = floorInput[0] == '-';
  if(isNegative) {
    offsetForNegative = floorInput + 1;
  }
  else {
    offsetForNegative = floorInput;
  }
  
  //Remove leading zeroes
  int numZeroes = 0;
  for(int i=0; i<3; i++) {
    if(offsetForNegative[i] != '0') {
       break;
    }
    numZeroes++;
  }
  char* offsetForNegativeAndZeroes = offsetForNegative + numZeroes;
  char buf[8];
  strncpy(buf, offsetForNegativeAndZeroes, 7);
  if(! isNegative) {
     strncpy(floorInput, buf, 7);
     return;  
  }

  char buf2[8];
  
  strcpy(buf2, "-");
  strcat(buf2, buf);
  strncpy(floorInput, buf2, 7);
  
}

void acceptNewCalls(State &st) {
  char key = keypad.getKey();
  if(key == NO_KEY ) {
    return;
  }
  if( key == '*') {
    st.panelState = SHOWING_FLOOR_SERVES_CHART;
    st.dirty = true;
    st.lastFloorChartTime = millis();
    return;
  }

  if( key == '#') {
    key = '-';
  }
  st.dirty = true;
  st.panelState = INPUT_IN_PROGRESS;
  memset(st.floorInput, 0, 8);
  st.floorInput[0] = key;
  st.lastInputTime = millis();
  
}



boolean isValidFloor(char* floorInput) {
  if(strcmp("13", floorInput) == 0) {
    return false;
  }
  if(strlen(floorInput) == 0) {
    return false;
  }
  
  char* endptr;
  int numericValue = strtol(floorInput, &endptr, 10);
  if(numericValue < -2 || numericValue > 55) {
    return false; 
  }
  if(numericValue == 0) {
    return false;
  }
  
  if(endptr == NULL || *endptr == 0) {
    return true; 
  }
  return false;  
}

char carForFloor(int n) {
  char* possibleCars = floorsToCars[n - BOTTOM_FLOOR];
  return possibleCars[rand() % strlen(possibleCars)];
}

InvalidFloorMessage randomInvalidFloorMessage() {
  return static_cast<InvalidFloorMessage>(rand() % 4);
}
void selectCar(State &st) {
  char* endptr;
  int n = strtol(st.floorInput,&endptr, 10);
  char buf[2];
  buf[0] = carForFloor(n);
  buf[1] = 0;
  strcpy(st.carName, buf);
}

void acceptNewInputOrTimeout(State &st) {
  if(millis() - st.lastInputTime > 4000) {
    st.dirty = true;
    interpretFloorInput(st.floorInput);
    if(!isValidFloor(st.floorInput)) {
       st.invalidFloorMessage = randomInvalidFloorMessage();
       st.panelState = INVALID_SELECTION; 
       st.lastInvalidSelectionTime = millis();
       return;
    }
    st.lastTellTime = millis();
    selectCar(st);
    st.panelState = TELLING_CAR; 
    return;
  }
  
  char key = keypad.getKey();
  if(key == NO_KEY) {
    return;
  }

  st.dirty = true;
  if(key == '*' || key == '#') {
    interpretFloorInput(st.floorInput);
    if(!isValidFloor(st.floorInput)) {
      st.invalidFloorMessage = randomInvalidFloorMessage();
       st.panelState = INVALID_SELECTION; 
       st.lastInvalidSelectionTime = millis();
       return;
    }
    selectCar(st);
    st.panelState = TELLING_CAR; 
    st.lastTellTime = millis();
    return; 
  }

  char buf[2] = " ";
  buf[0] = key;
  st.lastInputTime = millis();
  strncat(st.floorInput, buf, 1);

  if(strlen(st.floorInput) == 3) {
    if(strcmp("DAD", st.floorInput) == 0) {
      st.panelState = DAD_MODE;
      st.lastDadModeTime = millis();
      st.dirty = true;
      return;
    }
    interpretFloorInput(st.floorInput);
    if(!isValidFloor(st.floorInput)) {
       st.invalidFloorMessage = randomInvalidFloorMessage();
       st.panelState = INVALID_SELECTION; 
       st.lastInvalidSelectionTime = millis();
       st.dirty = true;
       return;
    }
    selectCar(st);
    st.panelState = TELLING_CAR; 
    st.lastTellTime = millis();
    return;
  }
    
}    

void waitForTellingCarTimeout(State &st) {
  if(millis() - st.lastTellTime > 3000) {
     st.dirty = true;
     st.panelState = ACCEPTING_NEW_CALLS;
  }
}


void waitForInvalidSelectionTimeout(State &st) {
  if(millis() - st.lastInvalidSelectionTime > 3000) {
     st.dirty = true;
     st.panelState = ACCEPTING_NEW_CALLS;
  }
}

void waitForFloorServesChartTimeout(State &st) {
  if(millis() - st.lastFloorChartTime > 10000) {
     st.dirty = true;
     st.panelState = ACCEPTING_NEW_CALLS;
  }
}

void renderState(State &st) {
   if(!st.dirty) {
    return;
   }
   switch(st.panelState) {
      case ACCEPTING_NEW_CALLS: 
         renderAcceptingNewCalls(st);
         break;
      case INPUT_IN_PROGRESS:
         renderInputInProgress(st);
         break;
      case TELLING_CAR:
         renderTellingCar(st);
         break;   
      case INVALID_SELECTION:
         renderInvalidSelection(st);
         break;   
      case SHOWING_FLOOR_SERVES_CHART:
         renderShowingFloorServesChart(st);
         break;
      case DAD_MODE:
         renderDadMode(st);
         break;      
        
   }
   st.dirty = false;
  
}

void renderDadMode(State &st) {
  tft.setTextFont(4);  
  tft.fillScreen(BG_COLOR);
  tft.setTextSize(1);
  tft.setTextColor(FG_COLOR, BG_COLOR);

  //Header line
  tft.fillRect(0, 66, DISPLAY_WIDTH, 3, FG_COLOR);

  //Vertical line
  tft.drawRect(DISPLAY_WIDTH/2 - X_OFFSET - 1, 0, 2, DISPLAY_HEIGHT, FG_COLOR);
  
  //Horizontal lines
  for(int i=2; i<7; i++) {
    tft.drawRect( 0,  (int)(ROW_HEIGHT * i), DISPLAY_WIDTH, 1, FG_COLOR); 
  }

  //Headers
  tft.setCursor(15, 30 );
  tft.println("Name");

  tft.setCursor(DISPLAY_WIDTH / 2  + 15 - X_OFFSET, 30 );
  tft.println("Birthday");
  
  int i = 1;
  //ROW 
  setCursorForTableEntry(i, 0, 0);
  tft.println("Daddy");
  setCursorForTableEntry(i, 1, 0);
  tft.println("11/30/1983"); 
  i++;

  setCursorForTableEntry(i, 0, 0);
  tft.println("Mommy");
  setCursorForTableEntry(i, 1, 0);
  tft.println("04/17/1987"); 
  i++; 

  setCursorForTableEntry(i, 0, 0);
  tft.println("Rowan");
  setCursorForTableEntry(i, 1, 0);
  tft.println("06/19/2015"); 
  i++; 

  setCursorForTableEntry(i, 0, 0);
  tft.println("Nana");
  setCursorForTableEntry(i, 1, 0);
  tft.println("02/17/1959"); 
  i++;

  setCursorForTableEntry(i, 0, 0);
  tft.println("Papa");
  setCursorForTableEntry(i, 1, 0);
  tft.println("02/14/1959"); 
  i++; 

  setCursorForTableEntry(i, 0, 0);
  tft.println("Ganny");
  setCursorForTableEntry(i, 1, 0);
  tft.println("07/15/1946"); 
  i++; 
  

}

void renderAcceptingNewCalls(State &st) {

  tft.fillScreen(BG_COLOR);
  tft.setTextColor(FG_COLOR, BG_COLOR);
  tft.setTextDatum(MC_DATUM);
  tft.setTextPadding(55*3); // Allow for 3 digits each 55 pixels wide
  tft.setTextSize(2);
  tft.drawString("Enter Floor", DISPLAY_WIDTH/2 - X_OFFSET, (DISPLAY_HEIGHT/2), 4); 
}

void renderInputInProgress(State &st) {
  tft.fillScreen(BG_COLOR);
  tft.setTextSize(2);
  tft.setTextColor(FG_COLOR, BG_COLOR);
  tft.setTextDatum(MC_DATUM);
  tft.setTextPadding(55*3); // Allow for 3 digits each 55 pixels wide
  tft.setTextSize(4);
  tft.drawString(st.floorInput, DISPLAY_WIDTH/2 - X_OFFSET, (DISPLAY_HEIGHT/2), 4); 
}

void drawLeftArrow() {
  int leftX = (DISPLAY_WIDTH - X_OFFSET - 150) / 2;
  tft.fillScreen(BG_COLOR);
  tft.fillTriangle( leftX, 60, leftX + 50,20, leftX + 50,100,FG_COLOR); 
  tft.fillRect( leftX + 50, 45, 100, 30, FG_COLOR);
}

void drawRightArrow() {
  int rightX = (DISPLAY_WIDTH - X_OFFSET + 150) / 2;
  tft.fillScreen(BG_COLOR);
  tft.fillTriangle( rightX, 60, rightX - 50,20, rightX - 50,100,FG_COLOR); 
  tft.fillRect( rightX - 50 - 100, 45, 100, 30, FG_COLOR);
}

void renderTellingCar(State &st) {
  tft.fillScreen(BG_COLOR);
  if(isCarToTheLeft(st.carName[0])) {
    drawLeftArrow();   
  }
  else {
    drawRightArrow();
  }
  
  tft.setTextSize(2);
  tft.setTextColor(FG_COLOR, BG_COLOR);
  tft.setTextDatum(MC_DATUM);
  tft.setTextPadding(55*3); // Allow for 3 digits each 55 pixels wide
  
  tft.drawString("Take car:", DISPLAY_WIDTH/2 - X_OFFSET, (DISPLAY_HEIGHT/2) - 65, 4);
  tft.drawString(st.carName, DISPLAY_WIDTH/2 - X_OFFSET, (DISPLAY_HEIGHT/2), 4);
  tft.drawString("To floor:", DISPLAY_WIDTH/2 - X_OFFSET, (DISPLAY_HEIGHT/2) + 65, 4);
  tft.drawString(st.floorInput, DISPLAY_WIDTH/2- X_OFFSET, (DISPLAY_HEIGHT/2) + 65*2, 4);
}


void renderInvalidSelection(State &st) {
  
  tft.fillScreen(BG_COLOR);
  tft.setTextSize(2);
  tft.setTextColor(FG_COLOR, BG_COLOR);
  tft.setTextDatum(MC_DATUM);
  tft.setTextPadding(55*3); // Allow for 3 digits each 55 pixels wide
  
  switch(st.invalidFloorMessage) {
    case SILLY_FLOOR:
      tft.drawString("Silly Floor!", DISPLAY_WIDTH/2- X_OFFSET , (DISPLAY_HEIGHT/2) - 65, 4);    
    break;
    case INVALID_FLOOR:
      tft.drawString("Invalid Floor!", DISPLAY_WIDTH/2- X_OFFSET , (DISPLAY_HEIGHT/2) - 65, 4);    
    break;

    case THIS_SELECTION_NEEDS_HELP:
      tft.drawString("This Selection", DISPLAY_WIDTH/2- X_OFFSET , (DISPLAY_HEIGHT/2) - 65 * 2, 4);
      tft.drawString("Needs Help!", DISPLAY_WIDTH/2- X_OFFSET , (DISPLAY_HEIGHT/2) - 65, 4);
    break;

    case THIS_BUILDING_DOES_NOT_SERVE_THIS_FLOOR:
      tft.drawString("This Building", DISPLAY_WIDTH/2- X_OFFSET , (DISPLAY_HEIGHT/2) - 65 * 2, 4);
      tft.drawString("Doesn't Serve", DISPLAY_WIDTH/2- X_OFFSET , (DISPLAY_HEIGHT/2) - 65, 4);
    
    break;
  }
  
  tft.drawString(st.floorInput, DISPLAY_WIDTH/2- X_OFFSET , (DISPLAY_HEIGHT/2) , 4); 
}


void setCursorForTableEntry(int rowIdx, int colIdx, int lineIdx) {
  tft.setCursor(12 + (DISPLAY_WIDTH/2) * colIdx - X_OFFSET , (int)(ROW_HEIGHT * rowIdx) + 10 + (lineIdx * 28)) ; 
}


void renderShowingFloorServesChart(State &st) {
  tft.setTextFont(4);  
  tft.fillScreen(BG_COLOR);
  tft.setTextSize(1);
  tft.setTextColor(FG_COLOR, BG_COLOR);

  //Header line
  tft.fillRect(0, 66, DISPLAY_WIDTH, 3, FG_COLOR);

  //Vertical line
  tft.drawRect(DISPLAY_WIDTH/2 - X_OFFSET - 1, 0, 2, DISPLAY_HEIGHT, FG_COLOR);
  
  //Horizontal lines
  for(int i=2; i<7; i++) {
    tft.drawRect( 0,  (int)(ROW_HEIGHT * i), DISPLAY_WIDTH, 1, FG_COLOR); 
  }

  //Headers
  tft.setCursor(15, 30 );
  tft.println("Floor");

  tft.setCursor(DISPLAY_WIDTH / 2  + 15 - X_OFFSET, 30 );
  tft.println("Take Car");
  
  int i = 1;
  //ROW 
  setCursorForTableEntry(i, 0, 0);
  tft.println("9 47-55");
  setCursorForTableEntry(i, 1, 0);
  tft.println("F G"); 
  i++;

  //ROW 
  setCursorForTableEntry(i, 0, 0);
  tft.println("1 3-8");
  setCursorForTableEntry(i, 0, 1);
  tft.println("10-12 14-27");
  
  setCursorForTableEntry(i, 1, 0);
  tft.println("G H I J");
  setCursorForTableEntry(i, 1, 1);
  tft.println("K L O P"); 
  i++;

  //ROW 
  setCursorForTableEntry(i, 0, 0);
  tft.println("1 3-8 23");
  setCursorForTableEntry(i, 0, 1);
  tft.println("28-45");
  
  setCursorForTableEntry(i, 1, 0);
  tft.println("A B C D");
  setCursorForTableEntry(i, 1, 1);
  tft.println("E F M N"); 
  i++;

  //ROW 
  setCursorForTableEntry(i, 0, 0);
  tft.println("-2 -1");
  setCursorForTableEntry(i, 0, 1);
  tft.println("2-12 14-46");
  
  setCursorForTableEntry(i, 1, 0);
  tft.println("Q R S T");
  setCursorForTableEntry(i, 1, 1);
  tft.println("U V W X"); 
  i++;

  //ROW 
  setCursorForTableEntry(i, 0, 0);
  tft.println("-2 -1");
  setCursorForTableEntry(i, 0, 1);
  tft.println("1-12 14-19");
  
  setCursorForTableEntry(i, 1, 0);
  tft.println("Y"); 
  i++;

  //ROW 
  setCursorForTableEntry(i, 0, 0);
  tft.println("-2 -1 1");
  setCursorForTableEntry(i, 0, 1);
  tft.println("20-35");
  
  setCursorForTableEntry(i, 1, 0);
  tft.println("Z"); 
  i++;
}
