#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "dexcom_constants.h"
#include "esp32_secure_client.h"
#include "dexcom_client.h"

void setupSerial()
{
  Serial.begin(115200);
}

void connectToWiFi()
{
  Serial.println("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("DNS server: ");
  Serial.println(WiFi.dnsIP());
}

void syncTime()
{
  Serial.println("Syncing time...");
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  while (time(nullptr) < 1000000000)
  {
    delay(100);
  }
  Serial.println("Time synchronized");
}

void resolveDexcomHost()
{
  IPAddress ip;
  // TODO: Use host from config
  if (WiFi.hostByName("shareous1.dexcom.com", ip))
  {
    Serial.print("shareous1.dexcom.com resolved to: ");
    Serial.println(ip);
  }
  else
  {
    Serial.println("DNS lookup failed for shareous1.dexcom.com");
  }
}

void fetchAndPrintGlucoseReading(DexcomClient &dexcomClient)
{
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

void setup()
{
  setupSerial();
  connectToWiFi();
  syncTime();
  resolveDexcomHost();

  ESP32SecureClient secureClient;
  secureClient.setCACert(DexcomConst::rootCA);
  DexcomClient dexcomClient(secureClient, DEXCOM_USERNAME, DEXCOM_ACCOUNT_ID, DEXCOM_PASSWORD, true);

  fetchAndPrintGlucoseReading(dexcomClient);
}

void loop()
{
  // Main loop code (if needed)
}