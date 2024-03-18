#include "gps.h"

GPS::GPS(HardwareSerial& serial, int txPin, int rxPin) : gpsSerial(serial) {
    gpsSerial.begin(9600, SERIAL_8N1, txPin, rxPin);
    bool _gps_debug = false;
    unsigned long _rxTimeReload = 1000;
    _gps_status = GPS_Status::NO_GPS;
}

void GPS::begin(int baudrate) {
    gpsSerial.begin(baudrate);
}

void GPS::update() {
    while (gpsSerial.available() > 0) {
        char rx_char = gpsSerial.read();
        if (_gps_debug) {
            Serial.write(rx_char);
        }
        if (gps.encode(rx_char)) {
            // Process GPS data
            if(_gps_status == GPS_Status::NO_GPS) {
                _gps_status = GPS_Status::NO_TIME;
                _seconds = gps.time.second();
                _locationUpdateTime = millis() + 1000; // Reused this for heartbeat
            }
            else {
                _rxTimeReload = millis() + 1000;
            }
        }
    }
}

void GPS::setDebug(bool state) {
    _gps_debug = state; 
}

bool GPS::isDataValid() {
    return gps.location.isValid();
}

GPS_Status GPS::getGpsStatus() {
    unsigned long time_now = millis();
    if (time_now >= _rxTimeReload) {
        // No valid rx within 1 second
        _rxTimeReload += 1000;
        _gps_status = GPS_Status::NO_GPS; 
        return GPS_Status::NO_GPS;
    } else {   
        if (_gps_status == GPS_Status::NO_GPS) {
            return GPS_Status::IDLE;
        }  
    }
    // If we got here, there is at least communication received from the GPS module
    bool new_second = false;
    uint8_t seconds = gps.time.second();
    if (seconds != _seconds) {
        _seconds = seconds;
        new_second = true;
        if (_gps_status == GPS_Status::NO_TIME) {
            _gps_status = GPS_Status::LOCATION_NOK;
            return GPS_Status::LOCATION_NOK;
        }
    }
    if (_gps_status == GPS_Status::NO_TIME) {
        if (time_now >= _locationUpdateTime) {
            _locationUpdateTime += 1000;
            if (!new_second) {
                return GPS_Status::NO_TIME;
            }
        }
    }   
    if (new_second) {
        if (gps.location.isUpdated()) {
            _locationUpdateTime = millis();
            _gps_status = GPS_Status::LOCATION_OK;
            return GPS_Status::LOCATION_OK;
        }
        else {
            if ((time_now - _locationUpdateTime) > 1000) {
                // No location update happened in 1 second
                _locationUpdateTime += 1000;
            
                // A new second is elapsed
                if (_gps_status == GPS_Status::LOCATION_OK) {
                    _gps_status = GPS_Status::LOCATION_NOK;
                    return GPS_Status::LOCATION_NOK;
                }
                if (_gps_status == GPS_Status::LOCATION_NOK) {
                    return GPS_Status::LOCATION_NOK;
                }
            }  
        }
    }
    return GPS_Status::IDLE;
}

float GPS::getLatitude() {
    return gps.location.lat();
}

float GPS::getLongitude() {
    return gps.location.lng();
}

float GPS::getAltitude() {
    return gps.altitude.meters();
}

float GPS::getSpeed() {
    return gps.speed.kmph();
}

float GPS::getCourse() {
    return gps.course.deg();
}

int GPS::getYear() {
    return gps.date.year();
}

int GPS::getMonth() {
    return gps.date.month();
}

int GPS::getDay() {
    return gps.date.day();
}

int GPS::getHour() {
    return gps.time.hour();
}

int GPS::getMinute() {
    return gps.time.minute();
}

int GPS::getSecond() {
    return gps.time.second();
}

int GPS::getSatellites() {
    return gps.satellites.value();
}

char* GPS::getLocalTime() {
    static char localTimeString[9]; // "HH:MM:SS\0"

    // Calculate local time including DST adjustment
    int localHour = gps.time.hour() + 1;    // Brussel Timezone has +1h offset to UTC
    int localMinute = gps.time.minute();
    int localSecond = gps.time.second();

    // Determine if DST is in effect
    bool isDST = _isDSTInEffect(gps.date.year(), gps.date.month(), gps.date.day());

    // Adjust local time for DST if necessary
    if (isDST) {
        localHour += 1; // Add one hour for DST
    }

    // Ensure local hour remains within 0-23 range
    localHour %= 24;

    // Convert local time components to string format
    sprintf(localTimeString, "%02d:%02d:%02d", localHour, localMinute, localSecond);

    return localTimeString;
}

bool GPS::_isDSTInEffect(int year, int month, int day) {
    // DST starts on the last Sunday of March and ends on the last Sunday of October
    if (month < 3 || month > 10) {
        return false; // DST is not in effect outside of March-October
    }

    int previousMonth = (month == 1) ? 12 : month - 1;
    int previousYear = (month == 1) ? year - 1 : year;

    int century = previousYear / 100;
    int yearOfCentury = previousYear % 100;

    // Calculate day of the week using Zeller's Congruence
    int dayOfWeek = (day + ((13 * (previousMonth + 1)) / 5) + yearOfCentury + (yearOfCentury / 4) + (century / 4) + (5 * century)) % 7;

    // If the month is March, DST starts on the last Sunday of March
    if (month == 3) {
        // If the day is a Sunday and it's after the 24th, DST starts
        return (dayOfWeek == 0 && day > 24);
    }
    // If the month is October, DST ends on the last Sunday of October
    else if (month == 10) {
        // If the day is a Sunday and it's before the 31st, DST ends
        return (dayOfWeek == 0 && day < 31);
    }
    return false;
}
