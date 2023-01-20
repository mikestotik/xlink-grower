//
// Created by mikestotik on 18.12.22.
//

#ifndef GROW_DEVICE_SENSORVALUE_H
#define GROW_DEVICE_SENSORVALUE_H


struct SensorValue {
public:
    short int id;
    String name;
    int max;
    int min;


    SensorValue(short int id, String name, int min, int max) : id(id), name(std::move(name)), min(min), max(max) {};
};


#endif //GROW_DEVICE_SENSORVALUE_H
