//
// Created by mikestotik on 1.01.23.
//

#ifndef XLINK_GROWER_SYSTEMSENSORHANDLER_H
#define XLINK_GROWER_SYSTEMSENSORHANDLER_H


class SystemSensorHandler : public SensorHandler {
    bool status = true;

public:
    std::list<SensorValue> getMeta() override {
        return {
            *new SensorValue(0, "heapSize", 0, 1023),
            *new SensorValue(0, "freeHeap", 0, 1023),
            *new SensorValue(0, "minFreeHeap", 0, 1023),
            *new SensorValue(0, "maxAllocHeap", 0, 1023)
        };
    }


    bool getStatus() override {
        return this->status;
    }


    String getData() override {
        DynamicJsonDocument info(2048);
        info["heapSize"] = ESP.getHeapSize();
        info["freeHeap"] = ESP.getFreeHeap();
        info["minFreeHeap"] = ESP.getMinFreeHeap();
        info["maxAllocHeap"] = ESP.getMaxAllocHeap();

        String infoJson;
        serializeJsonPretty(info, infoJson);

        return infoJson;
    }
};


#endif //XLINK_GROWER_SYSTEMSENSORHANDLER_H
