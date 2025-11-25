// CellTower.cpp
#include "CellTower.h"
#include "StringUtils.h"
#include "basicIO.h"

template<typename T>
void Channel<T>::displayChannelInfo() const {
    io.outputstring("  Channel ");
    io.outputint(channelId);
    io.outputstring(": ");
    io.outputint(frequencyStart);
    io.outputstring("-");
    io.outputint(frequencyStart + bandwidth);
    io.outputstring(" kHz (");
    io.outputint(currentUsers);
    io.outputstring("/");
    io.outputint(maxUsers);
    io.outputstring(" users)");
    io.terminate();
}

template class Channel<UserDevice>;

void CellTower::setupChannels(unsigned int bandwidth, unsigned int usersPerChannel, unsigned int antennas) {
    channelCount = totalBandwidth / bandwidth;
    channels = new Channel<UserDevice>*[channelCount];
    
    for (unsigned int i = 0; i < channelCount; ++i) {
        unsigned int freqStart = i * bandwidth;
        channels[i] = new Channel<UserDevice>(i, freqStart, bandwidth, usersPerChannel, antennas);
    }
}

CellTower::CellTower(unsigned int id, CellularCore* cellCore,
                     unsigned int bandwidth, unsigned int channelBandwidth, 
                     unsigned int usersPerChannel, unsigned int antennas) 
    : towerId(id), core(cellCore), channels(nullptr), channelCount(0),
      totalBandwidth(bandwidth), maxUsersPerChannel(usersPerChannel), antennaCount(antennas) {
    
    setupChannels(channelBandwidth, usersPerChannel, antennas);
}

CellTower::~CellTower() {
    if (channels) {
        for (unsigned int i = 0; i < channelCount; ++i) {
            delete channels[i];
        }
        delete[] channels;
    }
}

bool CellTower::assignDeviceToChannel(UserDevice* device, unsigned int channelIndex) {
    if (channelIndex >= channelCount || !device) {
        return false;
    }
    
    if (channels[channelIndex]->assignDevice(device)) {
        device->assignToChannel(channelIndex, channels[channelIndex]->getFrequencyStart());
        device->activate();
        return true;
    }
    return false;
}

bool CellTower::assignDeviceToFirstAvailable(UserDevice* device) {
    for (unsigned int i = 0; i < channelCount; ++i) {
        if (assignDeviceToChannel(device, i)) {
            return true;
        }
    }
    return false;
}

unsigned int CellTower::getTotalCapacity() const {
    return channelCount * maxUsersPerChannel * antennaCount;
}

unsigned int CellTower::getCurrentLoad() const {
    unsigned int total = 0;
    for (unsigned int i = 0; i < channelCount; ++i) {
        total += channels[i]->getCurrentUsers();
    }
    return total;
}

Channel<UserDevice>* CellTower::getChannel(unsigned int index) const {
    if (index < channelCount) {
        return channels[index];
    }
    return nullptr;
}

void CellTower::displayTowerInfo() const {
    io.outputstring("\n--- Tower Status ---");
    io.terminate();
    io.outputstring("Tower ID: ");
    io.outputint(towerId);
    io.terminate();
    io.outputstring("Active Users: ");
    io.outputint(getCurrentLoad());
    io.outputstring("/");
    io.outputint(getTotalCapacity());
    io.terminate();
    
    if (core) {
        io.outputstring("Cellular Core: ");
        io.outputstring(core->getProtocolName());
        io.terminate();
    }
}

void CellTower::displayFirstChannelUsers() const {
    Channel<UserDevice>* firstChannel = channels[0];
    unsigned int channelBw = firstChannel->getBandwidth();
    unsigned int totalUsers = firstChannel->getCurrentUsers();
    unsigned int antennas = firstChannel->getAntennaCount();
    
    io.outputstring("\n--- Users in First Channel (0-");
    io.outputint(channelBw);
    io.outputstring(" kHz) ---");
    io.terminate();
    
    io.outputstring("Total Users: ");
    io.outputint(totalUsers);
    
    if (antennas > 1) {
        io.outputstring(" across ");
        io.outputint(antennas);
        io.outputstring(" antennas (parallel)");
    }
    io.terminate();
    
    if (totalUsers == 0) {
        io.outputstring("Device IDs: None");
        io.terminate();
        return;
    }
    
    if (antennas == 1) {
        io.outputstring("Device IDs: ");
        for (unsigned int i = 0; i < totalUsers; ++i) {
            UserDevice* device = firstChannel->getDevice(i);
            if (device) {
                io.outputint(device->getDeviceId());
                if (i < totalUsers - 1) {
                    io.outputstring(", ");
                }
            }
        }
        io.terminate();
    } else {
        io.terminate();
        for (unsigned int ant = 1; ant <= antennas; ++ant) {
            io.outputstring("Antenna ");
            io.outputint(ant);
            io.outputstring(": ");
            
            unsigned int count = 0;
            for (unsigned int i = 0; i < totalUsers; ++i) {
                UserDevice* device = firstChannel->getDevice(i);
                if (device) {
                    unsigned int deviceId = device->getDeviceId();
                    unsigned int assignedAntenna = ((deviceId - 1) % antennas) + 1;
                    
                    if (assignedAntenna == ant) {
                        io.outputint(deviceId);
                        count++;
                        
                        bool hasMore = false;
                        for (unsigned int j = i + 1; j < totalUsers; ++j) {
                            UserDevice* nextDev = firstChannel->getDevice(j);
                            if (nextDev) {
                                unsigned int nextAnt = ((nextDev->getDeviceId() - 1) % antennas) + 1;
                                if (nextAnt == ant) {
                                    hasMore = true;
                                    break;
                                }
                            }
                        }
                        
                        if (hasMore) {
                            io.outputstring(", ");
                        }
                    }
                }
            }
            
            if (count == 0) {
                io.outputstring("None");
            }
            io.terminate();
        }
    }
}