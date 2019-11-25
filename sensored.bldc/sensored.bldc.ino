#include <TimerOne.h>
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

static long iHallTurnCounter = 0;

void setup() {
  // initialize port for motor control
  pinMode(PORT_BRAKE, OUTPUT);
  pinMode(PORT_CW, OUTPUT);
  pinMode(PORT_PWM, OUTPUT);
  pinMode(PORT_HALL, INPUT_PULLUP);
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
void loop() {
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
  if (Serial.available() > 0) {
    char incomingByte = Serial.read();
    if(incomingByte == 'z') {
      digitalWrite(PORT_CW,HIGH);
    }
    if(incomingByte == 'f') {
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
  }

}

void HallTurnCounterInterrupt(void) {
  iHallTurnCounter++;
}
