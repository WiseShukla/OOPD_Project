// CellTower.h
#ifndef CELLTOWER_H
#define CELLTOWER_H

#include "UserDevice.h"
#include "CellularCore.h"

template<typename T>
class Channel {
private:
    unsigned int channelId;
    unsigned int frequencyStart;
    unsigned int bandwidth;
    T** assignedDevices;
    unsigned int maxUsers;
    unsigned int currentUsers;
    unsigned int antennaCount;
    
public:
    Channel(unsigned int id, unsigned int freqStart, unsigned int bw, 
            unsigned int maxUsr, unsigned int antennas = 1) 
        : channelId(id), frequencyStart(freqStart), bandwidth(bw), 
          maxUsers(maxUsr * antennas), currentUsers(0), antennaCount(antennas) {
        assignedDevices = new T*[maxUsers];
        for (unsigned int i = 0; i < maxUsers; ++i) {
            assignedDevices[i] = nullptr;
        }
    }
    
    ~Channel() {
        delete[] assignedDevices;
    }
    
    bool assignDevice(T* device) {
        if (currentUsers < maxUsers) {
            assignedDevices[currentUsers++] = device;
            return true;
        }
        return false;
    }
    
    unsigned int getChannelId() const { return channelId; }
    unsigned int getFrequencyStart() const { return frequencyStart; }
    unsigned int getBandwidth() const { return bandwidth; }
    unsigned int getCurrentUsers() const { return currentUsers; }
    unsigned int getMaxUsers() const { return maxUsers; }
    unsigned int getAntennaCount() const { return antennaCount; }
    unsigned int getUsersPerAntenna() const { 
        return (antennaCount > 0) ? (maxUsers / antennaCount) : maxUsers; 
    }
    
    T* getDevice(unsigned int index) const {
        if (index < currentUsers) return assignedDevices[index];
        return nullptr;
    }
    
    void displayChannelInfo() const;
};

class CellTower {
private:
    unsigned int towerId;
    CellularCore* core;
    Channel<UserDevice>** channels;
    unsigned int channelCount;
    unsigned int totalBandwidth;
    unsigned int maxUsersPerChannel;
    unsigned int antennaCount;
    
    void setupChannels(unsigned int bandwidth, unsigned int usersPerChannel, unsigned int antennas);
    
public:
    CellTower(unsigned int id, CellularCore* cellCore,
              unsigned int bandwidth, unsigned int channelBandwidth, 
              unsigned int usersPerChannel, unsigned int antennas = 1);
    ~CellTower();
    
    CellTower(const CellTower&) = delete;
    CellTower& operator=(const CellTower&) = delete;
    
    bool assignDeviceToChannel(UserDevice* device, unsigned int channelIndex);
    bool assignDeviceToFirstAvailable(UserDevice* device);
    
    unsigned int getTowerId() const { return towerId; }
    unsigned int getChannelCount() const { return channelCount; }
    unsigned int getTotalCapacity() const;
    unsigned int getCurrentLoad() const;
    unsigned int getAntennaCount() const { return antennaCount; }
    Channel<UserDevice>* getChannel(unsigned int index) const;
    
    void displayTowerInfo() const;
    void displayFirstChannelUsers() const;
};

#endif