
enum PanelState { ACCEPTING_NEW_CALLS, INPUT_IN_PROGRESS, TELLING_CAR, INVALID_SELECTION, SHOWING_FLOOR_SERVES_CHART, DAD_MODE}; 
enum Direction {LEFT, RIGHT};
enum InvalidFloorMessage { SILLY_FLOOR, INVALID_FLOOR, THIS_SELECTION_NEEDS_HELP, THIS_BUILDING_DOES_NOT_SERVE_THIS_FLOOR};
struct State {
  PanelState panelState = ACCEPTING_NEW_CALLS;
  Direction tellCarDirection = LEFT;
  char floorInput[8];
  char carName[8];

  boolean dirty = true;
  unsigned long lastInputTime;
  unsigned long lastTellTime;
  unsigned long lastInvalidSelectionTime;
  unsigned long lastFloorChartTime;
  unsigned long lastDadModeTime;
  InvalidFloorMessage invalidFloorMessage;
  char lastKeyPressed;  
}; 
