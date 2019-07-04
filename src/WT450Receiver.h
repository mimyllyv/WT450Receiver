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
    WT450Receiver(int bufferSize = 10, bool keepOnlyNewest = true);

    void begin(uint8_t digitalInterruptPin, unsigned int treshold_ = 250, unsigned int shortValue_ = 1000, unsigned int longValue_ = 2000);
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

    Data decode(unsigned long value);
    uint8_t digitalInterruptPin;
};

#endif