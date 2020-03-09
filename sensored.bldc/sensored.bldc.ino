#define DUMP_VAR(x)  { \
  Serial.print(__LINE__);\
  Serial.print("@@"#x"=<");\
  Serial.print(x);\
  Serial.print(">&$");\
  Serial.print("\r\n");\
}

#define PORT_BRAKE 11
#define PORT_CW 12
#define PORT_PWM 3
#define PORT_HALL 2

#define PORT_LIMIT_Z 8
#define PORT_LIMIT_F 7


static long iHallTurnCounter = 0;

void setup() {
  // initialize port for motor control
  pinMode(PORT_BRAKE, OUTPUT);
  pinMode(PORT_CW, OUTPUT);
  pinMode(PORT_PWM, OUTPUT);
  
  pinMode(PORT_HALL, INPUT_PULLUP);
  pinMode(PORT_LIMIT_Z, INPUT_PULLUP);
  pinMode(PORT_LIMIT_F, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PORT_HALL),HallTurnCounterInterrupt , FALLING);

  // clock prescale by 1, PWM frequency: 32KHz
  TCCR2B = TCCR2B & 0xF8 | 0x01;
 
  Serial.begin(115200);
  DUMP_VAR("goto start!!!\n");
  digitalWrite(PORT_BRAKE,LOW);
  digitalWrite(PORT_CW,HIGH);
  analogWrite(PORT_PWM, 0);
}

static int32_t iMainLoopCounter = 0;

const static int32_t iMainLoopPrintA = 1024;
const static int32_t iMainLoopPrintB = 16;
const static int32_t iMainLoopPrintSkip = iMainLoopPrintA*iMainLoopPrintB;

static long volatile iHallTurnRunStep = 0;
static bool iBoolMotorCWFlag = false; //
static const  int16_t iPositionByHallRangeLow = 50;
static const  int16_t iPositionByHallRangeDistance = 163;
static const  int16_t iPositionByHallRangeHigh = iPositionByHallRangeLow + iPositionByHallRangeDistance;
static int16_t volatile iPositionByHallCounter= -1;

static bool iBoolRunCalibrate = false; //

void loop() {
  verifyLimitSwitch();
  //printTurnCounter();
  handleIncommingCommand();
}


void HallTurnCounterInterrupt(void) {
  iHallTurnCounter++;
  //DUMP_VAR(iHallTurnCounter);
  if(iHallTurnRunStep > 0) {
    iHallTurnRunStep--;
  } else {
    stopMotor();
  }
  if(iBoolMotorCWFlag) {
    iPositionByHallCounter++;
  } else {
    iPositionByHallCounter--;
  }
  DUMP_VAR(iPositionByHallCounter);
}

void verifyLimitSwitch(void) {
  int z = digitalRead(PORT_LIMIT_Z);
  if(z == 0) {
    //DUMP_VAR(z);
    if(iBoolMotorCWFlag == true) {
      iPositionByHallCounter= iPositionByHallRangeHigh;
      stopMotor();
    }
    if(iBoolRunCalibrate) {
      DUMP_VAR(iPositionByHallCounter);
      DUMP_VAR(iPositionByHallRangeLow);
      DUMP_VAR(iPositionByHallRangeHigh);
      iBoolRunCalibrate = false;
    }
  }
  int f = digitalRead(PORT_LIMIT_F);
  if(f == 0) {
    //DUMP_VAR(f);
    if(iBoolMotorCWFlag == false) {
      iPositionByHallCounter= iPositionByHallRangeLow;
      stopMotor();
    }
    if(iBoolRunCalibrate) {
      DUMP_VAR(iPositionByHallCounter);
      DUMP_VAR(iPositionByHallRangeLow);
      DUMP_VAR(iPositionByHallRangeHigh);
      iBoolRunCalibrate = false;
    }
  }
}


void printTurnCounter(void) {
  auto diff = iMainLoopCounter - iMainLoopPrintSkip;
  //DUMP_VAR(diff);
  //DUMP_VAR(iMainLoopPrintSkip);
  if(  diff > 0 ) {
    DUMP_VAR(iHallTurnCounter);
    iMainLoopCounter = 0;
  } else {
  }
  iMainLoopCounter++;
  //DUMP_VAR(iMainLoopCounter);
}

#define CW() {\
  iBoolMotorCWFlag = true;\
  digitalWrite(PORT_CW,HIGH);\
}

#define CCW() {\
  iBoolMotorCWFlag = false;\
  digitalWrite(PORT_CW,LOW);\
  }

void runLongCommand(char ch);

void handleIncommingCommand(void) {
  if (Serial.available() > 0) {
    char incomingByte = Serial.read();
    if(incomingByte == 'z') {
      CW();
      digitalWrite(PORT_CW,HIGH);
    }
    if(incomingByte == 'f') {
      CCW();
    }
    if(incomingByte == 'b') {
      digitalWrite(PORT_BRAKE,HIGH);
    }
    if(incomingByte == 'e') {
      digitalWrite(PORT_BRAKE,LOW);
    }
    if(incomingByte == '0') {
      analogWrite(PORT_PWM, 0);
      digitalWrite(PORT_BRAKE,LOW);
    }
    if(incomingByte == 'g') {
      startMotor();
    }
    runLongCommand(incomingByte);
    
    /*
    if(incomingByte == '1') {
      analogWrite(PORT_PWM, 16);
    }
    if(incomingByte == '2') {
      analogWrite(PORT_PWM, 32);
    }
    if(incomingByte == '3') {
      analogWrite(PORT_PWM, 64);
    }
    if(incomingByte == '4') {
      analogWrite(PORT_PWM, 128);
    }
    if(incomingByte == '5') {
      analogWrite(PORT_PWM, 255);
    }
    */
  }
}

static String gHandleStringCommand;
void runLongCommand(char ch) {
  gHandleStringCommand += ch;
  if(ch == '\n') {
    DUMP_VAR(gHandleStringCommand);
    if(gHandleStringCommand.startsWith("calibrate")) {
      runCalibrate();
    }
    if(gHandleStringCommand.startsWith("pos:")) {
      auto posValue = gHandleStringCommand.substring(4, gHandleStringCommand.length());
      posValue.trim();
      DUMP_VAR(posValue);
      auto pos = posValue.toInt();
      runToPosion(pos);
    }
    gHandleStringCommand = "";
  }
}

void runCalibrate(void) {
  iBoolMotorCWFlag = false;
  digitalWrite(PORT_CW,LOW);
  iBoolRunCalibrate = true;
  startMotor();
}

void runToPosion(int16_t position) {
  DUMP_VAR(position);
  int16_t currentPos = iPositionByHallCounter;
  DUMP_VAR(currentPos);
  int16_t diff = position - currentPos;
  DUMP_VAR(diff);
  iHallTurnRunStep = abs(diff);
  DUMP_VAR(iHallTurnRunStep);
  if(iHallTurnRunStep > 0) {
    if(diff > 0) {
      CW();
    } else {
      CCW();
    }
    analogWrite(PORT_PWM, 64);
    digitalWrite(PORT_BRAKE,HIGH);
  }
}

void stopMotor(void) {
  analogWrite(PORT_PWM, 0);
  digitalWrite(PORT_BRAKE,LOW);
  iHallTurnRunStep = -1;
}
void startMotor(void) {
  analogWrite(PORT_PWM, 32);
  digitalWrite(PORT_BRAKE,HIGH);
  iHallTurnRunStep = 6;
}

/*
void verifyRunTime(void) {
}
*/
