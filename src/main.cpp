#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "esp32_secure_client.h"
#include "dexcom_client.h"

ESP32SecureClient secureClient;
DexcomClient dexcomClient(secureClient, DEXCOM_USERNAME, DEXCOM_ACCOUNT_ID, DEXCOM_PASSWORD, true);

void setup()
{
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  try
  {
    auto reading = dexcomClient.getLatestGlucoseReading();
    if (reading)
    {
      Serial.print("Last glucose reading: ");
      Serial.print(reading->getValue());
      Serial.println(" mg/dL");
    }
    else
    {
      Serial.println("No current reading available");
    }
  }
  catch (const DexcomError &e)
  {
    Serial.print("Error: ");
    Serial.println(e.what());
  }
}

void loop()
{
}
