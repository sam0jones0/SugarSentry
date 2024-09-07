#include <Arduino.h>
#include <sample.h>

// put function declarations here:
int myFunction(int, int);

void setup()
{
  Serial.begin(115200);
  int result = add(1, 2);
  Serial.printf("1 + 2 = %d\n", result);
}

void loop()
{
  // put your main code here, to run repeatedly:
}
