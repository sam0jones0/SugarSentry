#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "dexcom_constants.h"
#include "esp32_secure_client.h"
#include "dexcom_client.h"

void setupSerial()
{
  Serial.begin(115200);
  while (!Serial)
  {
    ; // wait for serial port to connect
  }
}

void WiFiEventHandler(WiFiEvent_t event)
{
  switch (event)
  {
  case SYSTEM_EVENT_STA_GOT_IP:
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    break;
  case SYSTEM_EVENT_STA_DISCONNECTED:
    Serial.println("WiFi lost connection");
    WiFi.reconnect();
    break;
  default:
    break;
  }
}

void connectToWiFi()
{
  Serial.println("Connecting to WiFi...");
  WiFi.onEvent(WiFiEventHandler);
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
  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2)
  {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.println(asctime(&timeinfo));
}

void resolveDexcomHost()
{
  IPAddress ip;
  if (WiFi.hostByName(DexcomConst::DEXCOM_BASE_URL_OUS, ip))
  {
    Serial.print(DexcomConst::DEXCOM_BASE_URL_OUS);
    Serial.print(" resolved to: ");
    Serial.println(ip);
  }
  else
  {
    Serial.print("DNS lookup failed for ");
    Serial.println(DexcomConst::DEXCOM_BASE_URL_OUS);
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
      Serial.print(reading->getMmolL());
      Serial.println(" mmol/L");
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
  secureClient.setTimeout(30000); // 30 seconds timeout

  Serial.println("Creating DexcomClient...");
  DexcomClient dexcomClient(secureClient, DEXCOM_USERNAME, DEXCOM_ACCOUNT_ID, DEXCOM_PASSWORD, true);
  Serial.println("DexcomClient created successfully");

  Serial.println("Attempting to fetch glucose reading...");
  fetchAndPrintGlucoseReading(dexcomClient);
  Serial.println("Fetch attempt completed");
}

void loop()
{
  // Main loop code (if needed)
  delay(10000); // Wait for 10 seconds
  Serial.printf("Free heap in loop: %d\n", ESP.getFreeHeap());
}