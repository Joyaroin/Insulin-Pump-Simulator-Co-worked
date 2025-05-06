#ifndef DEVICESTATES_H
#define DEVICESTATES_H


enum class DeviceState {
    IDLE,         // The pump is on but not currently delivering insulin
    REFILL,       // The cartridge is being refilled
    DELIVERING,   // The pump is actively delivering insulin (basal or bolus)
    ERROR         // The pump is in an error state (e.g. low battery, empty cartridge)
};

#endif // DEVICESTATES_H
