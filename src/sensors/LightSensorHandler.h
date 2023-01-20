//
// Created by mikestotik on 18.12.22.
//

#ifndef GROW_DEVICE_LIGHTSENSORHANDLER_H
#define GROW_DEVICE_LIGHTSENSORHANDLER_H


class LightSensorHandler : public SensorHandler {
    bool status = false;

public:
    std::list<SensorValue> getMeta() override {
        return {
            *new SensorValue(0, "lux", 0, 100),
            *new SensorValue(1, "infrared", 0, 100),
            *new SensorValue(2, "full", 0, 100),
            *new SensorValue(3, "visible", 0, 100),
        };
    }


    bool getStatus() override {
        return this->status;
    }


    String getData() override {
        StaticJsonDocument<1024> data;
        data["lux"] = 32;
        data["infrared"] = 43;
        data["full"] = 23;
        data["visible"] = 23;

        String json;
        serializeJsonPretty(data, json);

        return json;
    }

};


#endif //GROW_DEVICE_LIGHTSENSORHANDLER_H
