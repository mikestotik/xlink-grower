//
// Created by mikestotik on 18.12.22.
//

#ifndef GROW_DEVICE_AIRSENSORHANDLER_H
#define GROW_DEVICE_AIRSENSORHANDLER_H


#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>


#define I2C_BME280_ADDRESS 0x76

#define SEA_LEVEL_PRESSURE_HPA (1013.25)

Adafruit_BME280 bme;


class AirSensorHandler : public SensorHandler {
    bool status;

public:
    AirSensorHandler() {
        this->status = bme.begin(I2C_BME280_ADDRESS);
        if (!status) {
            Serial.println("Could not find a valid BME280 sensor, check wiring!");
        }
    }


    std::list<SensorValue> getMeta() override {
        return {
            *new SensorValue(0, "temperature", 0, 1024),
            *new SensorValue(1, "humidity", 0, 1024),
            *new SensorValue(2, "pressure", 0, 1024)
        };
    }


    bool getStatus() override {
        if (!this->status) {
            this->status = bme.begin(I2C_BME280_ADDRESS);
        }
        return this->status;
    }


    String getData() override {
        StaticJsonDocument<1024> data;
        data["temperature"] = bme.readTemperature();
        data["humidity"] = bme.readHumidity();
        data["pressure"] = bme.readPressure() / 100.0F;

        String json;
        serializeJsonPretty(data, json);
        return json;
    }
};


#endif //GROW_DEVICE_AIRSENSORHANDLER_H
