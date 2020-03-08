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
  Serial.print("goto start!!!\n");
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
static uint16_t volatile iPositionByHallCounter= 0;

void loop() {
  verifyLimitSwitch();
  //printTurnCounter();
  handleIncommingCommand();
}


void HallTurnCounterInterrupt(void) {
  iHallTurnCounter++;
  DUMP_VAR(iHallTurnCounter);
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
      iPositionByHallCounter= 163;
      stopMotor();
    }
  }
  int f = digitalRead(PORT_LIMIT_F);
  if(f == 0) {
    //DUMP_VAR(f);
    if(iBoolMotorCWFlag == false) {
      iPositionByHallCounter= 0;
      stopMotor();
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

void handleIncommingCommand(void) {
  if (Serial.available() > 0) {
    char incomingByte = Serial.read();
    if(incomingByte == 'z') {
      iBoolMotorCWFlag = true;
      digitalWrite(PORT_CW,HIGH);
    }
    if(incomingByte == 'f') {
      iBoolMotorCWFlag = false;
      digitalWrite(PORT_CW,LOW);
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
