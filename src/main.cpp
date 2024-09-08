#include "Arduino.h"
// #include <WiFiClientSecure.h>

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
