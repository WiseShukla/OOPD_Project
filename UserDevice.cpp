// UserDevice.cpp
#include "UserDevice.h"
#include "StringUtils.h"
#include "basicIO.h"

UserDevice::UserDevice(unsigned int id, const char* name, unsigned int msgCount)
    : deviceId(id), messageCount(msgCount), active(false), assignedChannel(0), frequency(0) {
    
    unsigned int nameLen = my_strlen(name);
    deviceName = new char[nameLen + 1];
    my_strcpy(deviceName, name);
}

UserDevice::~UserDevice() {
    delete[] deviceName;
}

void UserDevice::assignToChannel(unsigned int channel, unsigned int freq) {
    assignedChannel = channel;
    frequency = freq;
}

void UserDevice::activate() {
    active = true;
}

void UserDevice::deactivate() {
    active = false;
}

void UserDevice::displayDeviceInfo() const {
    io.outputstring("Device ID: ");
    io.outputint(deviceId);
    io.outputstring(", Name: ");
    io.outputstring(deviceName);
    io.outputstring(", Status: ");
    io.outputstring(active ? "Active" : "Inactive");
    io.outputstring(", Channel: ");
    io.outputint(assignedChannel);
    io.outputstring(", Frequency: ");
    io.outputint(frequency);
    io.outputstring(" kHz, Messages: ");
    io.outputint(messageCount);
    io.terminate();
}