#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <MQ2.h>
#include <hpma115S0.h>
#include <SoftwareSerial.h>


#if SIMULATED_DATA
 
hpma115S0.Init();
hpma115S0.StartParticleMeasurement();

void initSensor()
{
    // use SIMULATED_DATA, no sensor need to be inited
}

float readTemperature()
{
    return random(20, 30);
}

float readHumidity()
{
    return random(30, 40);
}

#else

static DHT dht(DHT_PIN, DHT_TYPE);
static MQ2 mq2(MQ_PIN);
SoftwareSerial hpmaSerial(TX, RX);
void initSensor()
{
    dht.begin();
    hpmaSerial.begin();

}
int GetPM2_5()
{
  return hpma115S0.GetPM2_5();
}
float readLPG()
{
    return mq2.readLPG();
}

float readSmoke()
{
    return mq2.readSmoke();
}

float readCO()
{
    return mq2.readCO();
}
float readTemperature()
{
    return dht.readTemperature();
}
float readHumidity()
{
    return dht.readHumidity();
}

#endif

bool readMessage(int messageId, char *payload)
{
    float smoke = readSmoke();
    init _pm2_5 = GetPM2_5();
    float lpg = readLPG();
    float co  = readCO();
    float humidity = readHumidity();
    float temperature = readTemperature();
    StaticJsonBuffer<MESSAGE_MAX_LEN> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["deviceId"] = DEVICE_ID;
    root["messageId"] = messageId;    
    bool temperatureAlert = false;

    // NAN is not the valid json, change it to NULL
     if (std::isnan(_pm2_5))
    {
        root["_pm2_5"] = NULL;
    }
    else
    {
        root["_pm2_5"] = _pm2_5;

    }
    if (std::isnan(smoke))
    {
        root["smoke"] = NULL;
    }
    else
    {
        root["smoke"] = smoke;

    }
    if (std::isnan(lpg))
    {
        root["lpg"] = NULL;
    }
    else
    {
        root["lpg"] = lpg;

    }

    if (std::isnan(co))
    {
        root["co"] = NULL;
    }
    else
    {
        root["co"] = co;

    }

    if (std::isnan(temperature))
    {
        root["temperature"] = NULL;
    }
    else
    {
        root["temperature"] = temperature;
    }
    if (std::isnan(humidity))
    {
        root["humidity"] = NULL;
    }
    else
    {
        root["humidity"] = humidity;
    }
    /* My modification */    root["tempAlert"] = temperatureAlert;
    root.printTo(payload, MESSAGE_MAX_LEN);
    return temperatureAlert;
}

void parseTwinMessage(char *message)
{
    StaticJsonBuffer<MESSAGE_MAX_LEN> jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(message);
    if (!root.success())
    {
        Serial.printf("Parse %s failed.\r\n", message);
        return;
    }

    if (root["desired"]["interval"].success())
    {
        interval = root["desired"]["interval"];
    }
    else if (root.containsKey("interval"))
    {
        interval = root["interval"];
    }
}
