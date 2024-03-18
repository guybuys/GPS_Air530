/*!
 * @file gps.h
 *
 * This is part of the Air530 GPS class that is written
 * to be used in TSM (Technische Scholen Mechelen) projects.
 *
 * It makes use of the TinyGPSPlus library so make sure
 * to include mikalhart/TinyGPSPlus@^1.0.3 to the lib_deps
 * in your ini file.
 *
 * First, create a HardwareSerial object. E.g.:
 * gpsSerial(1); // Using UART 1 on ESP32
 * Next, define the GPS object with as parameters the 
 * HardwareSerial object, the GPS_TX_PIN and GPS_RX_PIN.
 * E.g.: GPS gps(gpsSerial, GPS_TX_PIN, GPS_RX_PIN)
 * To begin, use the begin(GPS_BAUD_RATE) method with 
 * GPS_BAUD_RATE set to 9600bps.
 * In the loop, call the update() method to handle the serial
 * communication send by the GPS.
 * The method gps.getGpsStatus() returns a variable of the
 * GPS_Status type. With this, it's possible to create a hard 
 * beat of about 1 second. Default, the returned value is IDLE.
 *
 * For questions, contact G. Buys
 */

#ifndef gps_h
#define gps_h


#include <Arduino.h>
#include <HardwareSerial.h>
#include <TinyGPS++.h>

enum class GPS_Status {
        IDLE,
        LOCATION_OK,
        LOCATION_NOK,
        NO_TIME,
        NO_GPS
    };

class GPS {
public:
    GPS(HardwareSerial& serial, int txPin, int rxPin);
    void begin(int baudrate);
    void update();
    bool isDataValid();
    GPS_Status getGpsStatus();
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
    int getSatellites();
    void setDebug(bool state);
    char* getLocalTime();

private:
    HardwareSerial& gpsSerial;
    TinyGPSPlus gps;
    bool _gps_debug;
    bool _isNewSecond;
    uint8_t _seconds;
    bool _isDSTInEffect(int year, int month, int day);
    unsigned long _rxTimeReload;
    unsigned long _locationUpdateTime;
    GPS_Status _gps_status;
};

#endif
