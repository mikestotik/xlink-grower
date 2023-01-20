//
// Created by mikestotik on 18.12.22.
//

#ifndef GROW_DEVICE_SENSOR_H
#define GROW_DEVICE_SENSOR_H


#include "SensorValue.h"
#include "SensorHandler.h"


enum class SensorType {
    System,
    Air,
    Soil,
    Light,
};


class Sensor {
    bool status{};
    short int id;
    SensorType type;
    SensorHandler *handler;

public:
    explicit Sensor(short int id, SensorType type, SensorHandler *handler) :
        id(id),
        type(type),
        handler(handler) {
    }


    short int getId() const {
        return this->id;
    }


    bool getStatus() {
        return this->handler->getStatus();
    }


    SensorType getType() {
        return this->type;
    }


    std::list<SensorValue> getMeta() {
        return this->handler->getMeta();
    };


    String getData() {
        return this->handler->getData();
    };
};


#endif //GROW_DEVICE_SENSOR_H
