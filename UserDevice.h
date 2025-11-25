// UserDevice.h
#ifndef USERDEVICE_H
#define USERDEVICE_H

class UserDevice {
private:
    unsigned int deviceId;
    char* deviceName;
    unsigned int messageCount;
    bool active;
    unsigned int assignedChannel;
    unsigned int frequency;
    
public:
    UserDevice(unsigned int id, const char* name, unsigned int msgCount);
    ~UserDevice();
    
    UserDevice(const UserDevice&) = delete;
    UserDevice& operator=(const UserDevice&) = delete;
    
    void assignToChannel(unsigned int channel, unsigned int freq);
    void activate();
    void deactivate();
    
    unsigned int getDeviceId() const { return deviceId; }
    const char* getDeviceName() const { return deviceName; }
    bool isActive() const { return active; }
    unsigned int getAssignedChannel() const { return assignedChannel; }
    unsigned int getFrequency() const { return frequency; }
    unsigned int getMessageCount() const { return messageCount; }
    
    void displayDeviceInfo() const;
};

#endif