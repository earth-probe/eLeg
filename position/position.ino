#define DUMP_VAR(x)  { \
  Serial.print(__LINE__);\
  Serial.print("@@"#x"=<");\
  Serial.print(x);\
  Serial.print(">");\
  Serial.print("\n");\
}

#define PORT_PHOTO_1 9

void setup() {
  // initialize port for motor control
  pinMode(PORT_PHOTO_1, INPUT);
 
  Serial.begin(115200);
  DUMP_VAR("goto start!!!\n");
}


void loop() {
  auto photo1 = digitalRead(PORT_PHOTO_1);
  if(photo1 != 0) {
    DUMP_VAR(photo1);
  }
}
