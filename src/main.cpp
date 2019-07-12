#include <Arduino.h>
#include <WT450Receiver.h>

WT450Receiver receiver = WT450Receiver(2);

void setup()
{
  Serial.begin(9600);
  receiver.begin();
//  receiver.listenOnlyHouseAndChannel(6);
}

void loop()
{
  if (receiver.available())
  {
    Data data = receiver.getData();
    Serial.print("House/channel ");
    Serial.print(data.houseCode);
    Serial.print("/");
    Serial.print(data.channel);
    Serial.print(", temp ");
    Serial.print(data.temperature);
    Serial.print(" C");
    if(data.batteryLow) {
      Serial.print(" (LOW BATTERY)");
    } 
    Serial.println();
  }
  else
  {
    delay(1000);
  }
}
