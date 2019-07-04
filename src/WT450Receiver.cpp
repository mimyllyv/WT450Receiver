#include <WT450Receiver.h>

volatile bool WT450Receiver::keepOnlyNewest = true;
volatile unsigned int WT450Receiver::bufferSize = 0;
volatile unsigned long *WT450Receiver::value = NULL;
volatile unsigned int WT450Receiver::treshold = 0;
volatile unsigned int WT450Receiver::shortValue = 0;
volatile unsigned int WT450Receiver::longValue = 0;

WT450Receiver::WT450Receiver(int bufferSize, bool keepOnlyNewest)
{
    WT450Receiver::keepOnlyNewest = keepOnlyNewest;
    WT450Receiver::bufferSize = bufferSize;
    value = new volatile unsigned long[bufferSize];
}

void WT450Receiver::begin(uint8_t digitalInterruptPin, unsigned int treshold_, unsigned int shortValue_, unsigned int longValue_)
{
    treshold = treshold_;
    shortValue = shortValue_;
    longValue = longValue_;
    this->digitalInterruptPin = digitalInterruptPin;
    attachInterrupt(digitalPinToInterrupt(digitalInterruptPin), handleInterrupt, CHANGE);
}

void WT450Receiver::end()
{
    detachInterrupt(digitalPinToInterrupt(digitalInterruptPin));
}

bool WT450Receiver::available()
{
    if (WT450Receiver::value == NULL)
    {
        return false;
    }

    for (unsigned int i = 0; i < WT450Receiver::bufferSize; i++)
    {
        if (WT450Receiver::value[i] != 0)
        {
            return true;
        }
    }

    return false;
}

Data WT450Receiver::decode(unsigned long value)
{
    Data data;
    data.houseCode = value >> 28;

    uint8_t channel = value >> 24;
    channel = channel & 0b0011;
    data.channel = channel;

    bool bat = value >> 23;
    bat = bat & 0b0001;
    data.batteryLow = bat;

    uint8_t sequence = value;
    sequence = sequence << 4;
    sequence = sequence >> 6;
    data.sequence = sequence;

    long tempWhole = value;
    tempWhole = tempWhole << 16;
    tempWhole = tempWhole >> 24;

    long tempFract = value;
    tempFract = tempFract << 24;
    tempFract = tempFract >> 28;

    data.temperature = (tempWhole - 50) + (tempFract / 16.0);

    return data;
}

Data WT450Receiver::getData()
{
    unsigned long val = 0;

    for (unsigned int i = WT450Receiver::bufferSize; i > 0; i--)
    {
        if (WT450Receiver::value[i - 1] != 0)
        {
            val = WT450Receiver::value[i - 1];
            WT450Receiver::value[i - 1] = 0;
            break;
        }
    }

    return decode(val);
}

void WT450Receiver::handleInterrupt()
{
    static unsigned long lastTime = 0;
    static unsigned int shorts = 0;
    static unsigned int longs = 0;
    static unsigned int datapos = 32;
    static unsigned long data = 0;
    static unsigned int databit = 0;
    const long time = micros();
    const unsigned int duration = time - lastTime;

    unsigned int value;
    if (duration > WT450Receiver::shortValue - WT450Receiver::treshold && duration < WT450Receiver::shortValue + WT450Receiver::treshold)
    {
        value = 1;
    }
    else if (duration > WT450Receiver::longValue - WT450Receiver::treshold && duration < WT450Receiver::longValue + WT450Receiver::treshold)
    {
        value = 2;
    }
    else
    {
        value = 0;
    }

    if (value == 0)
    {
        // Reset
        shorts = 0;
        longs = 0;
        databit = 0;
        datapos = 32;
        data = 0;
    }
    else
    {
        if (shorts == 4 && longs == 2)
        {
            // Data begins

            bool bt;
            if (value == 1)
            {
                databit++;
                bt = true;
            }
            else if (value == 2)
            {
                databit += 2;
                bt = false;
            }

            if (databit == 2)
            {
                datapos--;
                if (bt)
                {
                    data |= (1UL << (datapos));
                }
                databit = 0;
            }

            if (datapos == 0)
            {
                uint8_t houseCode = data >> 28;
                uint8_t channel = data >> 24;
                channel = channel & 0b0011;

                for (unsigned int i = 0; i < WT450Receiver::bufferSize; i++)
                {
                    if (WT450Receiver::keepOnlyNewest)
                    {
                        uint8_t houseCode_ = WT450Receiver::value[i] >> 28;
                        uint8_t channel_ = WT450Receiver::value[i] >> 24;
                        channel_ = channel_ & 0b0011;

                        if (houseCode == houseCode_ && channel == channel_)
                        {
                            WT450Receiver::value[i] = 0;
                        }
                    }
                    if (WT450Receiver::value[i] == 0)
                    {
                        WT450Receiver::value[i] = data;
                        break;
                    }
                }

                shorts = 0;
                longs = 0;
                databit = 0;
                datapos = 32;
                data = 0;
            }
        }
        else
        {
            if (value == 1)
            {
                shorts++;
            }
            else if (value == 2)
            {
                longs++;
            }
        }
    }

    lastTime = time;
}
