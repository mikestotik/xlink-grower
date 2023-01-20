//
// Created by mikestotik on 18.12.22.
//


#ifndef GROW_DEVICE_RELAY_H
#define GROW_DEVICE_RELAY_H


enum class ControlType {
    Socket,
    System,
};


enum class SystemControlType {
    Restart,
};


class Control {
    short id;
    short pin;
    ControlType type;

public:
    explicit Control(short id, ControlType type, short pin) : id(id), type(type), pin(pin) {
        pinMode(pin, OUTPUT);
    }


    short int getId() const {
        return this->id;
    }


    void setValue(int value) const {
        if (this->getValue() != value) {
            digitalWrite(this->pin, value);
        }
    }


    int getValue() const {
        return digitalRead(this->pin);
    }


    ControlType getType() {
        return this->type;
    }
};


#endif //GROW_DEVICE_RELAY_H
