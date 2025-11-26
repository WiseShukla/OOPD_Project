// Simulator.cpp - Modified for Ordered Threaded Output
#include "CellTower.h"
#include "CellularCore.h"
#include "UserDevice.h"
#include "ConfigParser.h"
#include "basicIO.h"
#include "StringUtils.h"
#include <thread>
#include <mutex>

std::mutex outputMutex;

// CharBuffer class for capturing output without standard library
class CharBuffer {
private:
    char* buffer;
    unsigned int capacity;
    unsigned int length;
    
public:
    CharBuffer() : capacity(100000), length(0) {
        buffer = new char[capacity];
        buffer[0] = '\0';
    }
    
    ~CharBuffer() {
        delete[] buffer;
    }
    
    void append(const char* str) {
        unsigned int strLen = my_strlen(str);
        if (length + strLen < capacity) {
            for (unsigned int i = 0; i < strLen; ++i) {
                buffer[length++] = str[i];
            }
            buffer[length] = '\0';
        }
    }
    
    void append(int num) {
        if (num < 0) {
            append("-");
            num = -num;
        }
        
        if (num == 0) {
            append("0");
            return;
        }
        
        char temp[32];
        int i = 0;
        while (num > 0) {
            temp[i++] = '0' + (num % 10);
            num /= 10;
        }
        
        for (int j = i - 1; j >= 0; --j) {
            buffer[length++] = temp[j];
        }
        buffer[length] = '\0';
    }
    
    void newline() {
        if (length + 1 < capacity) {
            buffer[length++] = '\n';
            buffer[length] = '\0';
        }
    }
    
    const char* getBuffer() const {
        return buffer;
    }
};

UserDevice** createUserDevices(unsigned int count, unsigned int messagesPerUser) {
    UserDevice** devices = new UserDevice*[count];
    
    for (unsigned int i = 0; i < count; ++i) {
        char deviceName[50];
        char tempId[20];
        my_strcpy(deviceName, "Device_");
        
        unsigned int id = i + 1;
        int idx = 0;
        unsigned int temp = id;
        if (temp == 0) {
            tempId[idx++] = '0';
        } else {
            char reversed[20];
            int revIdx = 0;
            while (temp > 0) {
                reversed[revIdx++] = '0' + (temp % 10);
                temp /= 10;
            }
            for (int j = revIdx - 1; j >= 0; --j) {
                tempId[idx++] = reversed[j];
            }
        }
        tempId[idx] = '\0';
        
        unsigned int baseLen = my_strlen(deviceName);
        for (unsigned int j = 0; tempId[j]; ++j) {
            deviceName[baseLen + j] = tempId[j];
        }
        deviceName[baseLen + my_strlen(tempId)] = '\0';
        
        devices[i] = new UserDevice(id, deviceName, messagesPerUser);
    }
    
    return devices;
}

// Captured version for threaded execution - generates IDENTICAL output
void simulate2GCaptured(const Config2G& cfg, unsigned int coreCapacity, CharBuffer& out) {
    out.append("\n\n===========================================");
    out.newline();
    out.append("          2G NETWORK SIMULATION");
    out.newline();
    out.append("===========================================");
    out.newline();
    
    out.append("\n--- Network Configuration ---");
    out.newline();
    out.append("Bandwidth: ");
    out.append(cfg.bandwidth);
    out.append(" kHz");
    out.newline();
    out.append("Channel Bandwidth: ");
    out.append(cfg.channelBw);
    out.append(" kHz");
    out.newline();
    out.append("Users per Channel: ");
    out.append(cfg.usersPerChannel);
    out.append(" (TDMA)");
    out.newline();
    
    unsigned int messagesPerUser = cfg.dataMessages + cfg.voiceMessages;
    out.append("Messages per User: ");
    out.append(messagesPerUser);
    out.append(" (");
    out.append(cfg.dataMessages);
    out.append(" data + ");
    out.append(cfg.voiceMessages);
    out.append(" voice)");
    out.newline();
    
    out.append("Overhead: ");
    out.append(cfg.overhead);
    out.append("%");
    out.newline();
    out.append("Core Capacity: ");
    out.append(coreCapacity);
    out.append(" messages");
    out.newline();
    
    out.append("\n--- Capacity Analysis ---");
    out.newline();
    
    unsigned int numChannels = cfg.bandwidth / cfg.channelBw;
    out.append("Channels: ");
    out.append(numChannels);
    out.append(" (");
    out.append(cfg.bandwidth);
    out.append(" / ");
    out.append(cfg.channelBw);
    out.append(")");
    out.newline();
    
    unsigned int frequencyLimit = numChannels * cfg.usersPerChannel;
    out.append("Frequency Limit: ");
    out.append(frequencyLimit);
    out.append(" users (");
    out.append(numChannels);
    out.append(" x ");
    out.append(cfg.usersPerChannel);
    out.append(")");
    out.newline();
    
    double effectiveMsgsFloat = (messagesPerUser * (100.0 + cfg.overhead)) / 100.0;
    unsigned int effectiveMsgs = (unsigned int)(effectiveMsgsFloat + 0.5);
    unsigned int coreLimit = coreCapacity / effectiveMsgs;
    
    out.append("Core Limit: ");
    out.append(coreLimit);
    out.append(" users (");
    out.append(coreCapacity);
    out.append(" / ");
    out.append(effectiveMsgs);
    out.append(")");
    out.newline();
    
    unsigned int towerCapacity = (frequencyLimit < coreLimit) ? frequencyLimit : coreLimit;
    out.append("Tower Capacity: ");
    out.append(towerCapacity);
    out.append(" users [LIMITED BY: ");
    out.append((towerCapacity == frequencyLimit) ? "Frequency" : "Core");
    out.append("]");
    out.newline();
    
    out.append("\n--- Connection Request Processing ---");
    out.newline();
    out.append("Requested: ");
    out.append(cfg.users);
    out.append(" users");
    out.newline();
    
    unsigned int accepted = (cfg.users <= towerCapacity) ? cfg.users : towerCapacity;
    unsigned int rejected = (cfg.users > towerCapacity) ? (cfg.users - towerCapacity) : 0;
    
    out.append("Accepted: ");
    out.append(accepted);
    out.append(" users");
    out.newline();
    out.append("Rejected: ");
    out.append(rejected);
    out.append(" users");
    out.newline();
    
    if (accepted == towerCapacity && rejected > 0) {
        out.append("Status: Tower reached maximum capacity");
        out.newline();
    }
    
    // Create actual tower and devices to match original output
    Core2G* core = new Core2G(1, towerCapacity);
    CellTower* tower = new CellTower(1, core, cfg.bandwidth, 
                                      cfg.channelBw, cfg.usersPerChannel, 1);
    
    UserDevice** devices = createUserDevices(accepted, messagesPerUser);
    
    for (unsigned int i = 0; i < accepted; ++i) {
        tower->assignDeviceToFirstAvailable(devices[i]);
    }
    
    // Display tower info to buffer
    out.append("\n--- Tower Status ---");
    out.newline();
    out.append("Tower ID: ");
    out.append(tower->getTowerId());
    out.newline();
    out.append("Active Users: ");
    out.append(tower->getCurrentLoad());
    out.append("/");
    out.append(tower->getTotalCapacity());
    out.newline();
    out.append("Cellular Core: ");
    out.append(core->getProtocolName());
    out.newline();
    out.append("Required Cores: 1");
    out.newline();
    
    // Display first channel users
    Channel<UserDevice>* firstChannel = tower->getChannel(0);
    unsigned int channelBw = firstChannel->getBandwidth();
    unsigned int totalUsers = firstChannel->getCurrentUsers();
    
    out.append("\n--- Users in First Channel (0-");
    out.append(channelBw);
    out.append(" kHz) ---");
    out.newline();
    
    out.append("Total Users: ");
    out.append(totalUsers);
    out.newline();
    
    if (totalUsers == 0) {
        out.append("Device IDs: None");
        out.newline();
    } else {
        out.append("Device IDs: ");
        for (unsigned int i = 0; i < totalUsers; ++i) {
            UserDevice* device = firstChannel->getDevice(i);
            if (device) {
                out.append(device->getDeviceId());
                if (i < totalUsers - 1) {
                    out.append(", ");
                }
            }
        }
        out.newline();
    }
    
    // Cleanup
    for (unsigned int i = 0; i < accepted; ++i) {
        delete devices[i];
    }
    delete[] devices;
    delete tower;
    delete core;
}

void simulate3GCaptured(const Config3G& cfg, unsigned int coreCapacity, CharBuffer& out) {
    out.append("\n\n===========================================");
    out.newline();
    out.append("          3G NETWORK SIMULATION");
    out.newline();
    out.append("===========================================");
    out.newline();
    
    out.append("\n--- Network Configuration ---");
    out.newline();
    out.append("Bandwidth: ");
    out.append(cfg.bandwidth);
    out.append(" kHz");
    out.newline();
    out.append("Channel Bandwidth: ");
    out.append(cfg.channelBw);
    out.append(" kHz");
    out.newline();
    out.append("Users per Channel: ");
    out.append(cfg.usersPerChannel);
    out.append(" (CDMA)");
    out.newline();
    out.append("Messages per User: ");
    out.append(cfg.messagesPerUser);
    out.newline();
    out.append("Overhead: ");
    out.append(cfg.overhead);
    out.append("%");
    out.newline();
    out.append("Core Capacity: ");
    out.append(coreCapacity);
    out.append(" messages");
    out.newline();
    
    out.append("\n--- Capacity Analysis ---");
    out.newline();
    
    unsigned int numChannels = cfg.bandwidth / cfg.channelBw;
    out.append("Channels: ");
    out.append(numChannels);
    out.append(" (");
    out.append(cfg.bandwidth);
    out.append(" / ");
    out.append(cfg.channelBw);
    out.append(")");
    out.newline();
    
    unsigned int frequencyLimit = numChannels * cfg.usersPerChannel;
    out.append("Frequency Limit: ");
    out.append(frequencyLimit);
    out.append(" users (");
    out.append(numChannels);
    out.append(" x ");
    out.append(cfg.usersPerChannel);
    out.append(")");
    out.newline();
    
    double effectiveMsgsFloat = (cfg.messagesPerUser * (100.0 + cfg.overhead)) / 100.0;
    unsigned int effectiveMsgs = (unsigned int)(effectiveMsgsFloat + 0.5);
    unsigned int coreLimit = coreCapacity / effectiveMsgs;
    
    out.append("Core Limit: ");
    out.append(coreLimit);
    out.append(" users (");
    out.append(coreCapacity);
    out.append(" / ");
    out.append(effectiveMsgs);
    out.append(")");
    out.newline();
    
    unsigned int towerCapacity = (frequencyLimit < coreLimit) ? frequencyLimit : coreLimit;
    out.append("Tower Capacity: ");
    out.append(towerCapacity);
    out.append(" users [LIMITED BY: ");
    out.append((towerCapacity == frequencyLimit) ? "Frequency" : "Core");
    out.append("]");
    out.newline();
    
    out.append("\n--- Connection Request Processing ---");
    out.newline();
    out.append("Requested: ");
    out.append(cfg.users);
    out.append(" users");
    out.newline();
    
    unsigned int accepted = (cfg.users <= towerCapacity) ? cfg.users : towerCapacity;
    unsigned int rejected = (cfg.users > towerCapacity) ? (cfg.users - towerCapacity) : 0;
    
    out.append("Accepted: ");
    out.append(accepted);
    out.append(" users");
    out.newline();
    out.append("Rejected: ");
    out.append(rejected);
    out.append(" users");
    out.newline();
    
    if (accepted == towerCapacity && rejected > 0) {
        out.append("Status: Tower reached maximum capacity");
        out.newline();
    }
    
    out.append("\n--- Tower Status ---");
    out.newline();
    out.append("Tower ID: 1");
    out.newline();
    out.append("Active Users: ");
    out.append(accepted);
    out.append("/");
    out.append(towerCapacity);
    out.newline();
    out.append("Cellular Core: 3G (CDMA - Packet Switching)");
    out.newline();
    out.append("Required Cores: 1");
    out.newline();
    
    if (accepted > 0) {
        out.append("\n--- Users in First Channel (0-");
        out.append(cfg.channelBw);
        out.append(" kHz) ---");
        out.newline();
        
        unsigned int firstChannelUsers = (accepted < cfg.usersPerChannel) ? accepted : cfg.usersPerChannel;
        out.append("Total Users: ");
        out.append(firstChannelUsers);
        out.newline();
        
        out.append("Device IDs: ");
        for (unsigned int i = 0; i < firstChannelUsers; ++i) {
            out.append(i + 1);
            if (i < firstChannelUsers - 1) {
                out.append(", ");
            }
        }
        out.newline();
    }
}

void simulate4GCaptured(const Config4G& cfg, unsigned int coreCapacity, CharBuffer& out) {
    out.append("\n\n===========================================");
    out.newline();
    out.append("          4G NETWORK SIMULATION");
    out.newline();
    out.append("===========================================");
    out.newline();
    
    out.append("\n--- Network Configuration ---");
    out.newline();
    out.append("Bandwidth: ");
    out.append(cfg.bandwidth);
    out.append(" kHz");
    out.newline();
    out.append("Channel Bandwidth: ");
    out.append(cfg.channelBw);
    out.append(" kHz (OFDM)");
    out.newline();
    out.append("Users per Channel: ");
    out.append(cfg.usersPerChannel);
    out.newline();
    out.append("MIMO Antennas: ");
    out.append(cfg.antennas);
    out.append(" (Parallel)");
    out.newline();
    out.append("Messages per User: ");
    out.append(cfg.messagesPerUser);
    out.newline();
    out.append("Overhead: ");
    out.append(cfg.overhead);
    out.append("%");
    out.newline();
    out.append("Core Capacity: ");
    out.append(coreCapacity);
    out.append(" messages");
    out.newline();
    
    out.append("\n--- Capacity Analysis ---");
    out.newline();
    
    unsigned int numChannels = cfg.bandwidth / cfg.channelBw;
    out.append("Channels: ");
    out.append(numChannels);
    out.append(" (");
    out.append(cfg.bandwidth);
    out.append(" / ");
    out.append(cfg.channelBw);
    out.append(")");
    out.newline();
    
    unsigned int baseUsersPerChannel = cfg.usersPerChannel;
    out.append("Base Users per Channel: ");
    out.append(baseUsersPerChannel);
    out.newline();
    
    unsigned int usersPerChannelMIMO = baseUsersPerChannel * cfg.antennas;
    out.append("Users per Channel with MIMO: ");
    out.append(usersPerChannelMIMO);
    out.append(" (");
    out.append(baseUsersPerChannel);
    out.append(" x ");
    out.append(cfg.antennas);
    out.append(" antennas)");
    out.newline();
    
    unsigned int frequencyLimit = numChannels * usersPerChannelMIMO;
    out.append("Total Frequency Limit: ");
    out.append(frequencyLimit);
    out.append(" users");
    out.newline();
    
    out.append("\nCore Calculation:");
    out.newline();
    
    double effectiveMsgsFloat = (cfg.messagesPerUser * (100.0 + cfg.overhead)) / 100.0;
    unsigned int effectiveMsgs = (unsigned int)(effectiveMsgsFloat + 0.5);
    
    out.append("  Messages per user with overhead: ");
    out.append(effectiveMsgs);
    out.newline();
    
    unsigned int singleCoreLimit = (unsigned int)(coreCapacity / effectiveMsgsFloat);
    out.append("  Single core limit: ");
    out.append(singleCoreLimit);
    out.append(" users");
    out.newline();
    
    unsigned int coresNeeded = (unsigned int)((frequencyLimit * effectiveMsgsFloat / coreCapacity) + 0.999);
    out.append("  Required cores: ");
    out.append(coresNeeded);
    out.newline();
    
    unsigned int multiCoreLimit = singleCoreLimit * coresNeeded;
    out.append("  Multi-core limit: ");
    out.append(multiCoreLimit);
    out.append(" users");
    out.newline();
    
    unsigned int towerCapacity = (frequencyLimit < multiCoreLimit) ? frequencyLimit : multiCoreLimit;
    out.append("\nTower Capacity: ");
    out.append(towerCapacity);
    out.append(" users [LIMITED BY: ");
    out.append((towerCapacity == frequencyLimit) ? "Frequency" : "Core");
    out.append("]");
    out.newline();
    
    out.append("\n--- Connection Request Processing ---");
    out.newline();
    out.append("Requested: ");
    out.append(cfg.users);
    out.append(" users");
    out.newline();
    
    unsigned int accepted = (cfg.users <= towerCapacity) ? cfg.users : towerCapacity;
    unsigned int rejected = (cfg.users > towerCapacity) ? (cfg.users - towerCapacity) : 0;
    
    out.append("Accepted: ");
    out.append(accepted);
    out.append(" users");
    out.newline();
    out.append("Rejected: ");
    out.append(rejected);
    out.append(" users");
    out.newline();
    
    if (accepted == towerCapacity && rejected > 0) {
        out.append("Status: Tower reached maximum capacity");
        out.newline();
    }
    
    out.append("\n--- Tower Status ---");
    out.newline();
    out.append("Tower ID: 1");
    out.newline();
    out.append("Active Users: ");
    out.append(accepted);
    out.append("/");
    out.append(towerCapacity);
    out.newline();
    out.append("Cellular Core: 4G (OFDM with MIMO - All IP Packet)");
    out.newline();
    out.append("Required Cores: ");
    out.append(coresNeeded);
    out.newline();
    
    if (accepted > 0) {
        out.append("\n--- Users in First Channel (0-");
        out.append(cfg.channelBw);
        out.append(" kHz) ---");
        out.newline();
        
        unsigned int firstChannelCapacity = usersPerChannelMIMO;
        unsigned int firstChannelUsers = (accepted < firstChannelCapacity) ? accepted : firstChannelCapacity;
        out.append("Total Users: ");
        out.append(firstChannelUsers);
        out.append(" across ");
        out.append(cfg.antennas);
        out.append(" antennas (parallel)");
        out.newline();
        out.newline();
        
        for (unsigned int ant = 1; ant <= cfg.antennas; ++ant) {
            out.append("Antenna ");
            out.append(ant);
            out.append(": ");
            
            bool hasUsers = false;
            for (unsigned int i = 1; i <= firstChannelUsers; ++i) {
                unsigned int assignedAntenna = ((i - 1) % cfg.antennas) + 1;
                if (assignedAntenna == ant) {
                    if (hasUsers) out.append(", ");
                    out.append(i);
                    hasUsers = true;
                }
            }
            
            if (!hasUsers) {
                out.append("None");
            }
            out.newline();
        }
    }
}

void simulate5GCaptured(const Config5G& cfg, unsigned int coreCapacity, CharBuffer& out) {
    out.append("\n\n===========================================");
    out.newline();
    out.append("          5G NETWORK SIMULATION");
    out.newline();
    out.append("===========================================");
    out.newline();
    
    out.append("\n--- Network Configuration ---");
    out.newline();
    out.append("Bandwidth: ");
    out.append(cfg.bandwidth);
    out.append(" kHz (10 MHz @ 1800 MHz)");
    out.newline();
    out.append("Channel Bandwidth: ");
    out.append(cfg.channelBw);
    out.append(" kHz");
    out.newline();
    out.append("Users per MHz: ");
    out.append(cfg.usersPerMHz);
    out.newline();
    out.append("Massive MIMO Antennas: ");
    out.append(cfg.antennas);
    out.append(" (Parallel)");
    out.newline();
    out.append("Messages per User: ");
    out.append(cfg.messagesPerUser);
    out.newline();
    out.append("Overhead: ");
    out.append(cfg.overhead);
    out.append("%");
    out.newline();
    out.append("Core Capacity: ");
    out.append(coreCapacity);
    out.append(" messages");
    out.newline();
    
    out.append("\n--- Capacity Analysis ---");
    out.newline();
    
    unsigned int bandwidthMHz = cfg.bandwidth / 1000;
    out.append("Bandwidth in MHz: ");
    out.append(bandwidthMHz);
    out.newline();
    
    unsigned int baseUsers = bandwidthMHz * cfg.usersPerMHz;
    out.append("Base Users: ");
    out.append(baseUsers);
    out.append(" (");
    out.append(bandwidthMHz);
    out.append(" MHz x ");
    out.append(cfg.usersPerMHz);
    out.append(" users/MHz)");
    out.newline();
    
    unsigned int frequencyLimit = baseUsers * cfg.antennas;
    out.append("Massive MIMO Gain: ");
    out.append(frequencyLimit);
    out.append(" users (");
    out.append(baseUsers);
    out.append(" x ");
    out.append(cfg.antennas);
    out.append(" antennas)");
    out.newline();
    
    out.append("Total Frequency Limit: ");
    out.append(frequencyLimit);
    out.append(" users");
    out.newline();
    
    out.append("\nCore Calculation:");
    out.newline();
    
    double effectiveMsgsFloat = (cfg.messagesPerUser * (100.0 + cfg.overhead)) / 100.0;
    unsigned int effectiveMsgs = (unsigned int)(effectiveMsgsFloat + 0.5);
    
    out.append("  Messages per user with overhead: ");
    out.append(effectiveMsgs);
    out.newline();
    
    unsigned int singleCoreLimit = (unsigned int)(coreCapacity / effectiveMsgsFloat);
    out.append("  Single core limit: ");
    out.append(singleCoreLimit);
    out.append(" users");
    out.newline();
    
    unsigned int coresNeeded = (unsigned int)((frequencyLimit * effectiveMsgsFloat / coreCapacity) + 0.999);
    out.append("  Required cores: ");
    out.append(coresNeeded);
    out.newline();
    
    unsigned int multiCoreLimit = singleCoreLimit * coresNeeded;
    out.append("  Multi-core limit: ");
    out.append(multiCoreLimit);
    out.append(" users");
    out.newline();
    
    unsigned int towerCapacity = (frequencyLimit < multiCoreLimit) ? frequencyLimit : multiCoreLimit;
    out.append("\nTower Capacity: ");
    out.append(towerCapacity);
    out.append(" users [LIMITED BY: ");
    out.append((towerCapacity == frequencyLimit) ? "Frequency" : "Core");
    out.append("]");
    out.newline();
    
    out.append("\n--- Connection Request Processing ---");
    out.newline();
    out.append("Requested: ");
    out.append(cfg.users);
    out.append(" users");
    out.newline();
    
    unsigned int accepted = (cfg.users <= towerCapacity) ? cfg.users : towerCapacity;
    unsigned int rejected = (cfg.users > towerCapacity) ? (cfg.users - towerCapacity) : 0;
    
    out.append("Accepted: ");
    out.append(accepted);
    out.append(" users");
    out.newline();
    out.append("Rejected: ");
    out.append(rejected);
    out.append(" users");
    out.newline();
    
    if (accepted == towerCapacity && rejected > 0) {
        out.append("Status: Tower reached maximum capacity");
        out.newline();
    }
    
    out.append("\n--- Tower Status ---");
    out.newline();
    out.append("Tower ID: 1");
    out.newline();
    out.append("Active Users: ");
    out.append(accepted);
    out.append("/");
    out.append(towerCapacity);
    out.newline();
    out.append("Cellular Core: 5G (Massive MIMO - High Frequency)");
    out.newline();
    out.append("Required Cores: ");
    out.append(coresNeeded);
    out.newline();
    
    if (accepted > 0) {
        out.append("\n--- Users in First Channel (0-");
        out.append(cfg.channelBw);
        out.append(" kHz) ---");
        out.newline();
        
        unsigned int usersPerChannel = cfg.usersPerMHz * cfg.antennas;
        unsigned int firstChannelUsers = (accepted < usersPerChannel) ? accepted : usersPerChannel;
        out.append("Total Users: ");
        out.append(firstChannelUsers);
        out.append(" across ");
        out.append(cfg.antennas);
        out.append(" antennas (parallel)");
        out.newline();
        out.newline();
        
        for (unsigned int ant = 1; ant <= cfg.antennas; ++ant) {
            out.append("Antenna ");
            out.append(ant);
            out.append(": ");
            
            bool hasUsers = false;
            int count = 0;
            for (unsigned int i = 1; i <= firstChannelUsers && count < 30; ++i) {
                unsigned int assignedAntenna = ((i - 1) % cfg.antennas) + 1;
                if (assignedAntenna == ant) {
                    if (hasUsers) out.append(", ");
                    out.append(i);
                    hasUsers = true;
                    count++;
                }
            }
            
            // if (count >= 30) {
            //     out.append(", ...");
            // }
            
            if (!hasUsers) {
                out.append("None");
            }
            out.newline();
        }
    }
}

// Original simulate functions for options 1-4 (keep as-is)
void simulate2G(const Config2G& cfg, unsigned int coreCapacity) {
    std::lock_guard<std::mutex> lock(outputMutex);
    
    io.outputstring("\n\n===========================================");
    io.terminate();
    io.outputstring("          2G NETWORK SIMULATION");
    io.terminate();
    io.outputstring("===========================================");
    io.terminate();
    
    io.outputstring("\n--- Network Configuration ---");
    io.terminate();
    io.outputstring("Bandwidth: ");
    io.outputint(cfg.bandwidth);
    io.outputstring(" kHz");
    io.terminate();
    io.outputstring("Channel Bandwidth: ");
    io.outputint(cfg.channelBw);
    io.outputstring(" kHz");
    io.terminate();
    io.outputstring("Users per Channel: ");
    io.outputint(cfg.usersPerChannel);
    io.outputstring(" (TDMA)");
    io.terminate();
    
    unsigned int messagesPerUser = cfg.dataMessages + cfg.voiceMessages;
    io.outputstring("Messages per User: ");
    io.outputint(messagesPerUser);
    io.outputstring(" (");
    io.outputint(cfg.dataMessages);
    io.outputstring(" data + ");
    io.outputint(cfg.voiceMessages);
    io.outputstring(" voice)");
    io.terminate();
    
    io.outputstring("Overhead: ");
    io.outputint(cfg.overhead);
    io.outputstring("%");
    io.terminate();
    io.outputstring("Core Capacity: ");
    io.outputint(coreCapacity);
    io.outputstring(" messages");
    io.terminate();
    
    io.outputstring("\n--- Capacity Analysis ---");
    io.terminate();
    
    unsigned int numChannels = cfg.bandwidth / cfg.channelBw;
    io.outputstring("Channels: ");
    io.outputint(numChannels);
    io.outputstring(" (");
    io.outputint(cfg.bandwidth);
    io.outputstring(" / ");
    io.outputint(cfg.channelBw);
    io.outputstring(")");
    io.terminate();
    
    unsigned int frequencyLimit = numChannels * cfg.usersPerChannel;
    io.outputstring("Frequency Limit: ");
    io.outputint(frequencyLimit);
    io.outputstring(" users (");
    io.outputint(numChannels);
    io.outputstring(" x ");
    io.outputint(cfg.usersPerChannel);
    io.outputstring(")");
    io.terminate();
    
    double effectiveMsgsFloat = (messagesPerUser * (100.0 + cfg.overhead)) / 100.0;
    unsigned int effectiveMsgs = (unsigned int)(effectiveMsgsFloat + 0.5);
    unsigned int coreLimit = coreCapacity / effectiveMsgs;
    
    io.outputstring("Core Limit: ");
    io.outputint(coreLimit);
    io.outputstring(" users (");
    io.outputint(coreCapacity);
    io.outputstring(" / ");
    io.outputint(effectiveMsgs);
    io.outputstring(")");
    io.terminate();
    
    unsigned int towerCapacity = (frequencyLimit < coreLimit) ? frequencyLimit : coreLimit;
    io.outputstring("Tower Capacity: ");
    io.outputint(towerCapacity);
    io.outputstring(" users [LIMITED BY: ");
    io.outputstring((towerCapacity == frequencyLimit) ? "Frequency" : "Core");
    io.outputstring("]");
    io.terminate();
    
    io.outputstring("\n--- Connection Request Processing ---");
    io.terminate();
    io.outputstring("Requested: ");
    io.outputint(cfg.users);
    io.outputstring(" users");
    io.terminate();
    
    unsigned int accepted = (cfg.users <= towerCapacity) ? cfg.users : towerCapacity;
    unsigned int rejected = (cfg.users > towerCapacity) ? (cfg.users - towerCapacity) : 0;
    
    io.outputstring("Accepted: ");
    io.outputint(accepted);
    io.outputstring(" users");
    io.terminate();
    io.outputstring("Rejected: ");
    io.outputint(rejected);
    io.outputstring(" users");
    io.terminate();
    
    if (accepted == towerCapacity && rejected > 0) {
        io.outputstring("Status: Tower reached maximum capacity");
        io.terminate();
    }
    
    Core2G* core = new Core2G(1, towerCapacity);
    CellTower* tower = new CellTower(1, core, cfg.bandwidth, 
                                      cfg.channelBw, cfg.usersPerChannel, 1);
    
    UserDevice** devices = createUserDevices(accepted, messagesPerUser);
    
    for (unsigned int i = 0; i < accepted; ++i) {
        tower->assignDeviceToFirstAvailable(devices[i]);
    }
    
    tower->displayTowerInfo();
    io.outputstring("Required Cores: 1");
    io.terminate();
    
    tower->displayFirstChannelUsers();
    
    for (unsigned int i = 0; i < accepted; ++i) {
        delete devices[i];
    }
    delete[] devices;
    delete tower;
    delete core;
}

void simulate3G(const Config3G& cfg, unsigned int coreCapacity) {
    std::lock_guard<std::mutex> lock(outputMutex);
    
    io.outputstring("\n\n===========================================");
    io.terminate();
    io.outputstring("          3G NETWORK SIMULATION");
    io.terminate();
    io.outputstring("===========================================");
    io.terminate();
    
    io.outputstring("\n--- Network Configuration ---");
    io.terminate();
    io.outputstring("Bandwidth: ");
    io.outputint(cfg.bandwidth);
    io.outputstring(" kHz");
    io.terminate();
    io.outputstring("Channel Bandwidth: ");
    io.outputint(cfg.channelBw);
    io.outputstring(" kHz");
    io.terminate();
    io.outputstring("Users per Channel: ");
    io.outputint(cfg.usersPerChannel);
    io.outputstring(" (CDMA)");
    io.terminate();
    io.outputstring("Messages per User: ");
    io.outputint(cfg.messagesPerUser);
    io.terminate();
    io.outputstring("Overhead: ");
    io.outputint(cfg.overhead);
    io.outputstring("%");
    io.terminate();
    io.outputstring("Core Capacity: ");
    io.outputint(coreCapacity);
    io.outputstring(" messages");
    io.terminate();
    
    io.outputstring("\n--- Capacity Analysis ---");
    io.terminate();
    
    unsigned int numChannels = cfg.bandwidth / cfg.channelBw;
    io.outputstring("Channels: ");
    io.outputint(numChannels);
    io.outputstring(" (");
    io.outputint(cfg.bandwidth);
    io.outputstring(" / ");
    io.outputint(cfg.channelBw);
    io.outputstring(")");
    io.terminate();
    
    unsigned int frequencyLimit = numChannels * cfg.usersPerChannel;
    io.outputstring("Frequency Limit: ");
    io.outputint(frequencyLimit);
    io.outputstring(" users (");
    io.outputint(numChannels);
    io.outputstring(" x ");
    io.outputint(cfg.usersPerChannel);
    io.outputstring(")");
    io.terminate();
    
    double effectiveMsgsFloat = (cfg.messagesPerUser * (100.0 + cfg.overhead)) / 100.0;
    unsigned int effectiveMsgs = (unsigned int)(effectiveMsgsFloat + 0.5);
    unsigned int coreLimit = coreCapacity / effectiveMsgs;
    
    io.outputstring("Core Limit: ");
    io.outputint(coreLimit);
    io.outputstring(" users (");
    io.outputint(coreCapacity);
    io.outputstring(" / ");
    io.outputint(effectiveMsgs);
    io.outputstring(")");
    io.terminate();
    
    unsigned int towerCapacity = (frequencyLimit < coreLimit) ? frequencyLimit : coreLimit;
    io.outputstring("Tower Capacity: ");
    io.outputint(towerCapacity);
    io.outputstring(" users [LIMITED BY: ");
    io.outputstring((towerCapacity == frequencyLimit) ? "Frequency" : "Core");
    io.outputstring("]");
    io.terminate();
    
    io.outputstring("\n--- Connection Request Processing ---");
    io.terminate();
    io.outputstring("Requested: ");
    io.outputint(cfg.users);
    io.outputstring(" users");
    io.terminate();
    
    unsigned int accepted = (cfg.users <= towerCapacity) ? cfg.users : towerCapacity;
    unsigned int rejected = (cfg.users > towerCapacity) ? (cfg.users - towerCapacity) : 0;
    
    io.outputstring("Accepted: ");
    io.outputint(accepted);
    io.outputstring(" users");
    io.terminate();
    io.outputstring("Rejected: ");
    io.outputint(rejected);
    io.outputstring(" users");
    io.terminate();
    
    if (accepted == towerCapacity && rejected > 0) {
        io.outputstring("Status: Tower reached maximum capacity");
        io.terminate();
    }
    
    Core3G* core = new Core3G(1, towerCapacity);
    CellTower* tower = new CellTower(1, core, cfg.bandwidth, 
                                      cfg.channelBw, cfg.usersPerChannel, 1);
    
    UserDevice** devices = createUserDevices(accepted, cfg.messagesPerUser);
    
    for (unsigned int i = 0; i < accepted; ++i) {
        tower->assignDeviceToFirstAvailable(devices[i]);
    }
    
    tower->displayTowerInfo();
    io.outputstring("Required Cores: 1");
    io.terminate();
    
    tower->displayFirstChannelUsers();
    
    for (unsigned int i = 0; i < accepted; ++i) {
        delete devices[i];
    }
    delete[] devices;
    delete tower;
    delete core;
}

void simulate4G(const Config4G& cfg, unsigned int coreCapacity) {
    std::lock_guard<std::mutex> lock(outputMutex);
    
    io.outputstring("\n\n===========================================");
    io.terminate();
    io.outputstring("          4G NETWORK SIMULATION");
    io.terminate();
    io.outputstring("===========================================");
    io.terminate();
    
    io.outputstring("\n--- Network Configuration ---");
    io.terminate();
    io.outputstring("Bandwidth: ");
    io.outputint(cfg.bandwidth);
    io.outputstring(" kHz");
    io.terminate();
    io.outputstring("Channel Bandwidth: ");
    io.outputint(cfg.channelBw);
    io.outputstring(" kHz (OFDM)");
    io.terminate();
    io.outputstring("Users per Channel: ");
    io.outputint(cfg.usersPerChannel);
    io.terminate();
    io.outputstring("MIMO Antennas: ");
    io.outputint(cfg.antennas);
    io.outputstring(" (Parallel)");
    io.terminate();
    io.outputstring("Messages per User: ");
    io.outputint(cfg.messagesPerUser);
    io.terminate();
    io.outputstring("Overhead: ");
    io.outputint(cfg.overhead);
    io.outputstring("%");
    io.terminate();
    io.outputstring("Core Capacity: ");
    io.outputint(coreCapacity);
    io.outputstring(" messages");
    io.terminate();
    
    io.outputstring("\n--- Capacity Analysis ---");
    io.terminate();
    
    unsigned int numChannels = cfg.bandwidth / cfg.channelBw;
    io.outputstring("Channels: ");
    io.outputint(numChannels);
    io.outputstring(" (");
    io.outputint(cfg.bandwidth);
    io.outputstring(" / ");
    io.outputint(cfg.channelBw);
    io.outputstring(")");
    io.terminate();
    
    unsigned int baseUsersPerChannel = cfg.usersPerChannel;
    io.outputstring("Base Users per Channel: ");
    io.outputint(baseUsersPerChannel);
    io.terminate();
    
    unsigned int usersPerChannelMIMO = baseUsersPerChannel * cfg.antennas;
    io.outputstring("Users per Channel with MIMO: ");
    io.outputint(usersPerChannelMIMO);
    io.outputstring(" (");
    io.outputint(baseUsersPerChannel);
    io.outputstring(" x ");
    io.outputint(cfg.antennas);
    io.outputstring(" antennas)");
    io.terminate();
    
    unsigned int frequencyLimit = numChannels * usersPerChannelMIMO;
    io.outputstring("Total Frequency Limit: ");
    io.outputint(frequencyLimit);
    io.outputstring(" users");
    io.terminate();
    
    io.outputstring("\nCore Calculation:");
    io.terminate();

    double effectiveMsgsFloat = (cfg.messagesPerUser * (100.0 + cfg.overhead)) / 100.0;
    unsigned int effectiveMsgs = (unsigned int)(effectiveMsgsFloat + 0.5);

    io.outputstring("  Messages per user with overhead: ");
    io.outputint(effectiveMsgs);
    io.terminate();

    unsigned int singleCoreLimit = (unsigned int)(coreCapacity / effectiveMsgsFloat);
    io.outputstring("  Single core limit: ");
    io.outputint(singleCoreLimit);
    io.outputstring(" users");
    io.terminate();

    unsigned int coresNeeded = (unsigned int)((frequencyLimit * effectiveMsgsFloat / coreCapacity) + 0.999);
    io.outputstring("  Required cores: ");
    io.outputint(coresNeeded);
    io.terminate();

    unsigned int multiCoreLimit = singleCoreLimit * coresNeeded;
    io.outputstring("  Multi-core limit: ");
    io.outputint(multiCoreLimit);
    io.outputstring(" users");
    io.terminate();
    
    unsigned int towerCapacity = (frequencyLimit < multiCoreLimit) ? frequencyLimit : multiCoreLimit;
    io.outputstring("\nTower Capacity: ");
    io.outputint(towerCapacity);
    io.outputstring(" users [LIMITED BY: ");
    io.outputstring((towerCapacity == frequencyLimit) ? "Frequency" : "Core");
    io.outputstring("]");
    io.terminate();
    
    io.outputstring("\n--- Connection Request Processing ---");
    io.terminate();
    io.outputstring("Requested: ");
    io.outputint(cfg.users);
    io.outputstring(" users");
    io.terminate();
    
    unsigned int accepted = (cfg.users <= towerCapacity) ? cfg.users : towerCapacity;
    unsigned int rejected = (cfg.users > towerCapacity) ? (cfg.users - towerCapacity) : 0;
    
    io.outputstring("Accepted: ");
    io.outputint(accepted);
    io.outputstring(" users");
    io.terminate();
    io.outputstring("Rejected: ");
    io.outputint(rejected);
    io.outputstring(" users");
    io.terminate();
    
    if (accepted == towerCapacity && rejected > 0) {
        io.outputstring("Status: Tower reached maximum capacity");
        io.terminate();
    }
    
    Core4G* core = new Core4G(1, towerCapacity, cfg.antennas);
    CellTower* tower = new CellTower(1, core, cfg.bandwidth, 
                                      cfg.channelBw, cfg.usersPerChannel, cfg.antennas);
    
    UserDevice** devices = createUserDevices(accepted, cfg.messagesPerUser);
    
    for (unsigned int i = 0; i < accepted; ++i) {
        tower->assignDeviceToFirstAvailable(devices[i]);
    }
    
    tower->displayTowerInfo();
    io.outputstring("Required Cores: ");
    io.outputint(coresNeeded);
    io.terminate();
    
    tower->displayFirstChannelUsers();
    
    for (unsigned int i = 0; i < accepted; ++i) {
        delete devices[i];
    }
    delete[] devices;
    delete tower;
    delete core;
}

void simulate5G(const Config5G& cfg, unsigned int coreCapacity) {
    std::lock_guard<std::mutex> lock(outputMutex);
    
    io.outputstring("\n\n===========================================");
    io.terminate();
    io.outputstring("          5G NETWORK SIMULATION");
    io.terminate();
    io.outputstring("===========================================");
    io.terminate();
    
    io.outputstring("\n--- Network Configuration ---");
    io.terminate();
    io.outputstring("Bandwidth: ");
    io.outputint(cfg.bandwidth);
    io.outputstring(" kHz (10 MHz @ 1800 MHz)");
    io.terminate();
    io.outputstring("Channel Bandwidth: ");
    io.outputint(cfg.channelBw);
    io.outputstring(" kHz");
    io.terminate();
    io.outputstring("Users per MHz: ");
    io.outputint(cfg.usersPerMHz);
    io.terminate();
    io.outputstring("Massive MIMO Antennas: ");
    io.outputint(cfg.antennas);
    io.outputstring(" (Parallel)");
    io.terminate();
    io.outputstring("Messages per User: ");
    io.outputint(cfg.messagesPerUser);
    io.terminate();
    io.outputstring("Overhead: ");
    io.outputint(cfg.overhead);
    io.outputstring("%");
    io.terminate();
    io.outputstring("Core Capacity: ");
    io.outputint(coreCapacity);
    io.outputstring(" messages");
    io.terminate();
    
    io.outputstring("\n--- Capacity Analysis ---");
    io.terminate();
    
    unsigned int bandwidthMHz = cfg.bandwidth / 1000;
    io.outputstring("Bandwidth in MHz: ");
    io.outputint(bandwidthMHz);
    io.terminate();
    
    unsigned int baseUsers = bandwidthMHz * cfg.usersPerMHz;
    io.outputstring("Base Users: ");
    io.outputint(baseUsers);
    io.outputstring(" (");
    io.outputint(bandwidthMHz);
    io.outputstring(" MHz x ");
    io.outputint(cfg.usersPerMHz);
    io.outputstring(" users/MHz)");
    io.terminate();
    
    unsigned int frequencyLimit = baseUsers * cfg.antennas;
    io.outputstring("Massive MIMO Gain: ");
    io.outputint(frequencyLimit);
    io.outputstring(" users (");
    io.outputint(baseUsers);
    io.outputstring(" x ");
    io.outputint(cfg.antennas);
    io.outputstring(" antennas)");
    io.terminate();
    
    io.outputstring("Total Frequency Limit: ");
    io.outputint(frequencyLimit);
    io.outputstring(" users");
    io.terminate();
    
    io.outputstring("\nCore Calculation:");
    io.terminate();

    double effectiveMsgsFloat = (cfg.messagesPerUser * (100.0 + cfg.overhead)) / 100.0;
    unsigned int effectiveMsgs = (unsigned int)(effectiveMsgsFloat + 0.5);

    io.outputstring("  Messages per user with overhead: ");
    io.outputint(effectiveMsgs);
    io.terminate();

    unsigned int singleCoreLimit = (unsigned int)(coreCapacity / effectiveMsgsFloat);
    io.outputstring("  Single core limit: ");
    io.outputint(singleCoreLimit);
    io.outputstring(" users");
    io.terminate();

    unsigned int coresNeeded = (unsigned int)((frequencyLimit * effectiveMsgsFloat / coreCapacity) + 0.999);
    io.outputstring("  Required cores: ");
    io.outputint(coresNeeded);
    io.terminate();

    unsigned int multiCoreLimit = singleCoreLimit * coresNeeded;
    io.outputstring("  Multi-core limit: ");
    io.outputint(multiCoreLimit);
    io.outputstring(" users");
    io.terminate();
    
    unsigned int towerCapacity = (frequencyLimit < multiCoreLimit) ? frequencyLimit : multiCoreLimit;
    io.outputstring("\nTower Capacity: ");
    io.outputint(towerCapacity);
    io.outputstring(" users [LIMITED BY: ");
    io.outputstring((towerCapacity == frequencyLimit) ? "Frequency" : "Core");
    io.outputstring("]");
    io.terminate();
    
    io.outputstring("\n--- Connection Request Processing ---");
    io.terminate();
    io.outputstring("Requested: ");
    io.outputint(cfg.users);
    io.outputstring(" users");
    io.terminate();
    
    unsigned int accepted = (cfg.users <= towerCapacity) ? cfg.users : towerCapacity;
    unsigned int rejected = (cfg.users > towerCapacity) ? (cfg.users - towerCapacity) : 0;
    
    io.outputstring("Accepted: ");
    io.outputint(accepted);
    io.outputstring(" users");
    io.terminate();
    io.outputstring("Rejected: ");
    io.outputint(rejected);
    io.outputstring(" users");
    io.terminate();
    
    if (accepted == towerCapacity && rejected > 0) {
        io.outputstring("Status: Tower reached maximum capacity");
        io.terminate();
    }
    
    Core5G* core = new Core5G(1, towerCapacity, cfg.antennas, true);
    
    unsigned int usersPerChannel = cfg.usersPerMHz;
    
    CellTower* tower = new CellTower(1, core, cfg.bandwidth, 
                                      cfg.channelBw, usersPerChannel, cfg.antennas);
    
    UserDevice** devices = createUserDevices(accepted, cfg.messagesPerUser);
    
    for (unsigned int i = 0; i < accepted; ++i) {
        tower->assignDeviceToFirstAvailable(devices[i]);
    }
    
    tower->displayTowerInfo();
    io.outputstring("Required Cores: ");
    io.outputint(coresNeeded);
    io.terminate();
    
    tower->displayFirstChannelUsers();
    
    for (unsigned int i = 0; i < accepted; ++i) {
        delete devices[i];
    }
    delete[] devices;
    delete tower;
    delete core;
}

void displayMenu() {
    io.outputstring("\n===========================================");
    io.terminate();
    io.outputstring("    CELLULAR NETWORK SIMULATOR - MENU");
    io.terminate();
    io.outputstring("===========================================");
    io.terminate();
    io.outputstring("1. Simulate 2G Network");
    io.terminate();
    io.outputstring("2. Simulate 3G Network");
    io.terminate();
    io.outputstring("3. Simulate 4G Network");
    io.terminate();
    io.outputstring("4. Simulate 5G Network");
    io.terminate();
    io.outputstring("5. Simulate All Networks (Threaded)");
    io.terminate();
    io.outputstring("6. Exit");
    io.terminate();
    io.outputstring("===========================================");
    io.terminate();
    io.outputstring("Enter your choice (1-6): ");
}

int main(int argc, char* argv[]) {
    io.outputstring("===========================================");
    io.terminate();
    io.outputstring("    CELLULAR NETWORK SIMULATOR");
    io.terminate();
    io.outputstring("===========================================");
    io.terminate();
    
    const char* inputFile = "input.txt";
    if (argc > 1) {
        inputFile = argv[1];
    }
    
    io.outputstring("Reading configuration from: ");
    io.outputstring(inputFile);
    io.terminate();
    
    try {
        SimulationConfig config = ConfigParser::parseFile(inputFile);
        
        io.outputstring("Configuration loaded successfully!");
        io.terminate();
        
        bool running = true;
        while (running) {
            displayMenu();
            int choice = io.inputint();
            
            switch(choice) {
                case 1:
                    if (config.config2G.users > 0) {
                        simulate2G(config.config2G, config.coreCapacity);
                    } else {
                        io.outputstring("\n2G simulation disabled (0 users configured)");
                        io.terminate();
                    }
                    break;
                    
                case 2:
                    if (config.config3G.users > 0) {
                        simulate3G(config.config3G, config.coreCapacity);
                    } else {
                        io.outputstring("\n3G simulation disabled (0 users configured)");
                        io.terminate();
                    }
                    break;
                    
                case 3:
                    if (config.config4G.users > 0) {
                        simulate4G(config.config4G, config.coreCapacity);
                    } else {
                        io.outputstring("\n4G simulation disabled (0 users configured)");
                        io.terminate();
                    }
                    break;
                    
                case 4:
                    if (config.config5G.users > 0) {
                        simulate5G(config.config5G, config.coreCapacity);
                    } else {
                        io.outputstring("\n5G simulation disabled (0 users configured)");
                        io.terminate();
                    }
                    break;
                    
                case 5: {
                    io.outputstring("\nRunning all simulations in parallel...");
                    io.terminate();
                    
                    // Create buffers on heap
                    CharBuffer* buffer2G = new CharBuffer();
                    CharBuffer* buffer3G = new CharBuffer();
                    CharBuffer* buffer4G = new CharBuffer();
                    CharBuffer* buffer5G = new CharBuffer();
                    
                    // Launch threads - they run in parallel
                    std::thread thread2G([&]() {
                        if (config.config2G.users > 0) {
                            simulate2GCaptured(config.config2G, config.coreCapacity, *buffer2G);
                        }
                    });
                    
                    std::thread thread3G([&]() {
                        if (config.config3G.users > 0) {
                            simulate3GCaptured(config.config3G, config.coreCapacity, *buffer3G);
                        }
                    });
                    
                    std::thread thread4G([&]() {
                        if (config.config4G.users > 0) {
                            simulate4GCaptured(config.config4G, config.coreCapacity, *buffer4G);
                        }
                    });
                    
                    std::thread thread5G([&]() {
                        if (config.config5G.users > 0) {
                            simulate5GCaptured(config.config5G, config.coreCapacity, *buffer5G);
                        }
                    });
                    
                    // Wait for all threads to complete
                    thread2G.join();
                    thread3G.join();
                    thread4G.join();
                    thread5G.join();
                    
                    // Now print results in order: 2G → 3G → 4G → 5G
                    // Only print if users were configured AND buffer has content
                    const char* buf2G = buffer2G->getBuffer();
                    const char* buf3G = buffer3G->getBuffer();
                    const char* buf4G = buffer4G->getBuffer();
                    const char* buf5G = buffer5G->getBuffer();
                    
                    if (config.config2G.users > 0 && my_strlen(buf2G) > 0) {
                        io.outputstring(buf2G);
                    }
                    if (config.config3G.users > 0 && my_strlen(buf3G) > 0) {
                        io.outputstring(buf3G);
                    }
                    if (config.config4G.users > 0 && my_strlen(buf4G) > 0) {
                        io.outputstring(buf4G);
                    }
                    if (config.config5G.users > 0 && my_strlen(buf5G) > 0) {
                        io.outputstring(buf5G);
                    }
                    
                    // Cleanup
                    delete buffer2G;
                    delete buffer3G;
                    delete buffer4G;
                    delete buffer5G;
                    
                    io.outputstring("\nAll simulations completed!");
                    io.terminate();
                    break;
                }
                    
                case 6:
                    io.outputstring("\nExiting simulator. Goodbye!");
                    io.terminate();
                    running = false;
                    break;
                    
                default:
                    io.outputstring("\nInvalid choice! Please enter 1-6.");
                    io.terminate();
                    break;
            }
        }
        
        io.outputstring("\n===========================================");
        io.terminate();
        io.outputstring("   SIMULATION COMPLETE");
        io.terminate();
        io.outputstring("===========================================");
        io.terminate();
        
    } catch (const char* error) {
        io.errorstring("\nException caught: ");
        io.errorstring(error);
        io.terminate();
        return 1;
    } catch (...) {
        io.errorstring("\nUnknown exception caught");
        io.terminate();
        return 1;
    }
    
    return 0;
}