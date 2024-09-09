#include "Arduino.h"
// #include <WiFiClientSecure.h>


/*
Most of the code so far is in

https://github.com/sam0jones0/SugarSentry/tree/main/lib/DexcomClient/include
*/


namespace A::B::C
{
  void TestCpp17()
  {
    auto p = std::pair<int, int>(1, 2);
    auto [a, b] = p;
    Serial.printf("OK: [%d, %d]", a, b);
  }
} // namespace
using namespace A::B::C;

void setup()
{
  Serial.begin(115200);
  delay(2000);
  auto ver = __cplusplus;
  Serial.print("C++ Version: ");
  Serial.println(ver);
  TestCpp17();
}
void loop() {}


// void setup()
// {
//   Serial.begin(115200);
//   pinMode(LED_BUILTIN, OUTPUT);
// }

// void loop()
// {
//   digitalWrite(LED_BUILTIN, HIGH);
//   delay(500);
//   digitalWrite(LED_BUILTIN, LOW);
//   delay(500);
//   Serial.print("Hello world\n");
// }