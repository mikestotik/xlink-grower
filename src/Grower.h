//
// Created by mikestotik on 18.12.22.
//

#ifndef GROW_DEVICE_GROWER_H
#define GROW_DEVICE_GROWER_H


#include <list>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "controls/Control.h"
#include "sensors/Sensor.h"
#include "sensors/SensorValue.h"


class Grower {
    String id;
    std::list<Control> controls;
    std::list<Sensor> sensors;

public:
    static String getSystemInfo() {
        DynamicJsonDocument info(2048);
        // Internal RAM
        info["heapSize"] = ESP.getHeapSize(); // total heap size
        info["freeHeap"] = ESP.getFreeHeap(); // available heap
        info["minFreeHeap"] = ESP.getMinFreeHeap(); // lowest level of free heap since boot
        info["maxAllocHeap"] = ESP.getMaxAllocHeap(); // largest block of heap that can be allocated at once

        // SPI RAM
        info["psramSize"] = ESP.getPsramSize();
        info["freePsram"] = ESP.getFreePsram();
        info["minFreePsram"] = ESP.getMinFreePsram();
        info["maxAllocPsram"] = ESP.getMaxAllocPsram();

        info["chipRevision"] = ESP.getChipRevision();
        info["chipModel"] = ESP.getChipModel();
        info["chipCores"] = ESP.getChipCores();
        info["cpuFreqMHz"] = ESP.getCpuFreqMHz();
        info["cycleCount"] = ESP.getCycleCount();
        info["sdkVersion"] = ESP.getSdkVersion();

        info["flashChipSize"] = ESP.getFlashChipSize();
        info["flashChipSpeed"] = ESP.getFlashChipSpeed();
        info["flashChipMode"] = ESP.getFlashChipMode();

        info["sketchSize"] = ESP.getSketchSize();
        info["sketchMD5"] = ESP.getSketchMD5();
        info["freeSketchSpace"] = ESP.getFreeSketchSpace();

        String infoJson;
        serializeJsonPretty(info, infoJson);

        return infoJson;
    }


    Grower(String id, std::list<Control> relays, std::list<Sensor> sensors) :
        id(std::move(id)),
        controls(std::move(relays)),
        sensors(std::move(sensors)) {
    }


    String getId() {
        return this->id;
    }


    std::list<Control> getControls() {
        return this->controls;
    }


    std::list<Sensor> getSensors() {
        return this->sensors;
    }


    Control getControl(ControlType type, int controlId) {
        for (Control control: this->controls) {
            if (control.getType() == type && control.getId() == controlId) {
                return control;
            }
        }
    }
};


#endif //GROW_DEVICE_GROWER_H
