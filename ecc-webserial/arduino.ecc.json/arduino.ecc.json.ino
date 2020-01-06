/*
#define DUMP_VAR(x)  { \
  Serial.print(__FUNCTION__);\
  Serial.print("::");\
  Serial.print(__LINE__);\
  Serial.print(":"#x"=<");\
  Serial.print(x);\
  Serial.print(">\r\n}");\
}

#define DUMP_HEX(x)  { \
  Serial.print(__FUNCTION__);\
  Serial.print("::");\
  Serial.print(__LINE__);\
  Serial.print(":"#x"=<");\
  for(int i = 0;i < sizeof(x);i++) {\
    Serial.print(x[i],HEX);\
  }\
  Serial.print(">\r\n}");\
}
*/


#define DUMP_VAR(x)
#define DUMP_HEX(x)


#include <uECC.h>
#include <ArduinoJson.h>

uint8_t gPrivateKey[32+1] = {0};
uint8_t gPublicKey[64 + 1] = {0};
uECC_Curve gCurve;

#define RESP_JSON(x) {\
  String output;\
  serializeJson(x, output);\
  Serial.write(output.c_str());\
  Serial.write("\r\n");\
}

void setup() {
  Serial.begin(115200);
  uECC_set_rng(&RNG2);
  checkKeySize();
  createKey();

  StaticJsonDocument<255> resp;
  resp["setup"] = true;
  RESP_JSON(resp);
}

void loop() {
  if (Serial.available() > 0) {
    pushIncomingStream(Serial.read());
  }
}

String gIncommingStream;
void pushIncomingStream(char incomingByte) {
  DUMP_VAR(incomingByte);
  gIncommingStream += incomingByte;
  if(incomingByte == '}') {
    DynamicJsonDocument doc(256);
    auto result = deserializeJson(doc, gIncommingStream);
    if(DeserializationError::Ok == result) {
      onReadGoodJsonMsg(doc);
      gIncommingStream = "";
    } else if(DeserializationError::NoMemory == result) {
      gIncommingStream = "";
    } else {
      StaticJsonDocument<128> error;
      error["json_error"] = result;
      RESP_JSON(error);
    }
  }
}

void onReadGoodJsonMsg(DynamicJsonDocument &doc) {
  RESP_JSON(doc);
  //JsonObject root = doc.to<JsonObject>();
  bool fetch = doc.containsKey("f");
  if(fetch) {
    auto fecthObject = doc["f"];
    if(fecthObject == "pub") {
      StaticJsonDocument<128> pub;
      pub["pub"] = true;
      RESP_JSON(pub);    
    }
    StaticJsonDocument<128> fetchResp;
    fetchResp["fetch"] = true;
    RESP_JSON(fetchResp);    
  }
  StaticJsonDocument<128> ack;
  ack["ack"] = true;
  ack["fetch"] = fetch;
  RESP_JSON(ack);
}

bool checkKeySize(void) {
  bool goodKeySize = false;
  gCurve = uECC_secp256r1();
  auto prvSize = uECC_curve_private_key_size(gCurve);
  DUMP_VAR(prvSize);
  DUMP_VAR(sizeof(gPrivateKey));
  auto pubSize = uECC_curve_public_key_size(gCurve);
  DUMP_VAR(pubSize);
  DUMP_VAR(sizeof(gPublicKey));
  if(sizeof(gPrivateKey) == prvSize +1 && sizeof(gPublicKey) == pubSize +1) {
    goodKeySize = true;
  }
  DUMP_VAR(goodKeySize);
  return goodKeySize;
}
void createKey(void) {
  uECC_make_key(gPublicKey, gPrivateKey, gCurve);
  DUMP_HEX(gPublicKey);
  DUMP_HEX(gPrivateKey);
}


static int RNG2(uint8_t *dest, unsigned size) {
  while (size) {
    uint8_t val = 0;
    for (unsigned i = 0; i < 8; ++i) {
      int init = analogRead(0);
      int count = 0;
      while (analogRead(0) == init) {
        ++count;
      }
      
      if (count == 0) {
         val = (val << 1) | (init & 0x01);
      } else {
         val = (val << 1) | (count & 0x01);
      }
    }
    *dest = val;
    ++dest;
    --size;
  }
  return 1;
}
