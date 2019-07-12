#ifndef _WT450Receiver_h
#define _WT450Receiver_h

#include <Arduino.h>

struct Data
{
    uint8_t houseCode;
    uint8_t channel;
    bool batteryLow;
    float temperature;
    uint8_t sequence;
};

class WT450Receiver
{
public:
    WT450Receiver(uint8_t digitalInterruptPin, unsigned int treshold = 250, unsigned int shortValue = 1000, unsigned int longValue = 2000, int bufferSize = 10, bool keepOnlyNewest = true);

    void begin();
    void listenOnlyHouseAndChannel(uint8_t houseCode = 255, uint8_t channel = 255);
    void end();
    bool available();
    Data getData();

private:
    static void handleInterrupt();
    volatile static bool keepOnlyNewest;
    volatile static unsigned int bufferSize;
    volatile static unsigned long *value;
    volatile static unsigned int treshold;
    volatile static unsigned int shortValue;
    volatile static unsigned int longValue;
    volatile static uint8_t houseCode;
    volatile static uint8_t channel;

    Data decode(unsigned long value);
    uint8_t digitalInterruptPin;
};

#endif