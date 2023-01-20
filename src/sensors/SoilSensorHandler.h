//
// Created by mikestotik on 18.12.22.
//

#ifndef GROW_DEVICE_SOILSENSORHANDLER_H
#define GROW_DEVICE_SOILSENSORHANDLER_H


class SoilSensorHandler : public SensorHandler {
    short soilHumidityPin;

public:
    explicit SoilSensorHandler(short soilHumidityPin) : soilHumidityPin(soilHumidityPin) {}


    std::list<SensorValue> getMeta() override {
        return {
            *new SensorValue(0, "humidity", 0, 4048)
        };
    }


    bool getStatus() override {
        int humidity = analogRead(this->soilHumidityPin);
        return humidity != 0;
    }


    String getData() override {
        StaticJsonDocument<1024> data;
        data["humidity"] = analogRead(this->soilHumidityPin);

        String json;
        serializeJsonPretty(data, json);

        return json;
    }


};


#endif //GROW_DEVICE_SOILSENSORHANDLER_H
