#ifndef GPS_h
#define GPS_h

#include <Arduino.h>
#include <HardwareSerial.h>
#include <TinyGPS++.h>

class GPS {
public:
    GPS(HardwareSerial& serial, int txPin, int rxPin);
    void begin(int baudrate);
    void update();
    bool isDataValid();
    int8_t getGPSinfo();
    float getLatitude();
    float getLongitude();
    float getAltitude();
    float getSpeed();
    float getCourse();
    int getYear();
    int getMonth();
    int getDay();
    int getHour();
    int getMinute();
    int getSecond();
    void setDebug(bool state);
    char* getLocalTime();

private:
    HardwareSerial& gpsSerial;
    TinyGPSPlus gps;
    bool _gps_debug;
    bool _isNewSecond;
    uint8_t _seconds;
    bool _isDSTInEffect(int year, int month, int day);
    bool _isNew();
    bool _isReceiving();
    unsigned long _rxTime;
    bool _locationValid;
    bool _rxOK;
};

#endif
