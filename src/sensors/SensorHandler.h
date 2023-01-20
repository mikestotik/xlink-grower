//
// Created by mikestotik on 18.12.22.
//

#ifndef GROW_DEVICE_SENSORHANDLER_H
#define GROW_DEVICE_SENSORHANDLER_H


class SensorHandler {
public:
    virtual std::list<SensorValue> getMeta() = 0;

    virtual String getData() = 0;

    virtual bool getStatus() = 0;
};


#endif //GROW_DEVICE_SENSORHANDLER_H
