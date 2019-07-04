#include <Arduino.h>
#include <WT450Receiver.h>

WT450Receiver *receiver = new WT450Receiver();

void setup()
{
  Serial.begin(9600);
  receiver->begin(2);
}

void loop()
{
  if (receiver->available())
  {
    Data data = receiver->getData();
    Serial.print("House code ");
    Serial.print(data.houseCode);
    Serial.print(", channel ");
    Serial.print(data.channel);
    Serial.print(", low bat ");
    Serial.print(data.batteryLow);
    Serial.print(", sequence ");
    Serial.print(data.sequence);
    Serial.print(", temperature ");
    Serial.println(data.temperature);
  }
  else
  {
    delay(1000);
  }
}
