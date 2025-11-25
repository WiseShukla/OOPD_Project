// Simulator.cpp
#include "CellTower.h"
#include "CellularCore.h"
#include "UserDevice.h"
#include "ConfigParser.h"
#include "basicIO.h"
#include "StringUtils.h"
#include <thread>
#include <mutex>

std::mutex outputMutex;

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
    
    // Fixed overhead calculation with proper rounding
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
    
    // Fixed overhead calculation with proper rounding
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
    
    // Fixed overhead calculation with proper rounding
    double effectiveMsgsFloat = (cfg.messagesPerUser * (100.0 + cfg.overhead)) / 100.0;
    unsigned int effectiveMsgs = (unsigned int)(effectiveMsgsFloat + 0.5);
    
    io.outputstring("  Messages per user with overhead: ");
    io.outputint(effectiveMsgs);
    io.terminate();
    
    unsigned int singleCoreLimit = coreCapacity / effectiveMsgs;
    io.outputstring("  Single core limit: ");
    io.outputint(singleCoreLimit);
    io.outputstring(" users");
    io.terminate();
    
    // Calculate required cores with proper ceiling
    unsigned int coresNeeded = (frequencyLimit * effectiveMsgs + coreCapacity - 1) / coreCapacity;
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
    
    // Fixed overhead calculation with proper rounding
    double effectiveMsgsFloat = (cfg.messagesPerUser * (100.0 + cfg.overhead)) / 100.0;
    unsigned int effectiveMsgs = (unsigned int)(effectiveMsgsFloat + 0.5);
    
    io.outputstring("  Messages per user with overhead: ");
    io.outputint(effectiveMsgs);
    io.terminate();
    
    unsigned int singleCoreLimit = coreCapacity / effectiveMsgs;
    io.outputstring("  Single core limit: ");
    io.outputint(singleCoreLimit);
    io.outputstring(" users");
    io.terminate();
    
    // Calculate required cores with proper ceiling
    unsigned int coresNeeded = (frequencyLimit * effectiveMsgs + coreCapacity - 1) / coreCapacity;
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
                    
                    std::thread thread2G, thread3G, thread4G, thread5G;
                    
                    if (config.config2G.users > 0) {
                        thread2G = std::thread(simulate2G, config.config2G, config.coreCapacity);
                    }
                    
                    if (config.config3G.users > 0) {
                        thread3G = std::thread(simulate3G, config.config3G, config.coreCapacity);
                    }
                    
                    if (config.config4G.users > 0) {
                        thread4G = std::thread(simulate4G, config.config4G, config.coreCapacity);
                    }
                    
                    if (config.config5G.users > 0) {
                        thread5G = std::thread(simulate5G, config.config5G, config.coreCapacity);
                    }
                    
                    if (thread2G.joinable()) thread2G.join();
                    if (thread3G.joinable()) thread3G.join();
                    if (thread4G.joinable()) thread4G.join();
                    if (thread5G.joinable()) thread5G.join();
                    
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