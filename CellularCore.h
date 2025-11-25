// CellularCore.h
#ifndef CELLULARCORE_H
#define CELLULARCORE_H

class CellularCore {
protected:
    unsigned int coreId;
    unsigned int capacity;
    unsigned int currentLoad;
    
public:
    CellularCore(unsigned int id, unsigned int cap) 
        : coreId(id), capacity(cap), currentLoad(0) {}
    
    virtual ~CellularCore() {}
    
    virtual double calculateOverhead() const = 0;
    virtual const char* getProtocolName() const = 0;
    virtual void displayCoreInfo() const = 0;
    
    unsigned int getCoreId() const { return coreId; }
    unsigned int getCapacity() const { return capacity; }
    unsigned int getCurrentLoad() const { return currentLoad; }
    
    bool addLoad(unsigned int load) {
        if (currentLoad + load <= capacity) {
            currentLoad += load;
            return true;
        }
        return false;
    }
    
    void resetLoad() { currentLoad = 0; }
};

class Core2G : public CellularCore {
public:
    Core2G(unsigned int id, unsigned int cap) : CellularCore(id, cap) {}
    
    double calculateOverhead() const override {
        return 8.0;
    }
    
    const char* getProtocolName() const override {
        return "2G (TDMA - Circuit/Packet Switching)";
    }
    
    void displayCoreInfo() const override;
};

class Core3G : public CellularCore {
public:
    Core3G(unsigned int id, unsigned int cap) : CellularCore(id, cap) {}
    
    double calculateOverhead() const override {
        return 5.0;
    }
    
    const char* getProtocolName() const override {
        return "3G (CDMA - Packet Switching)";
    }
    
    void displayCoreInfo() const override;
};

class Core4G : public CellularCore {
private:
    unsigned int mimoAntennas;
    
public:
    Core4G(unsigned int id, unsigned int cap, unsigned int antennas = 4) 
        : CellularCore(id, cap), mimoAntennas(antennas) {}
    
    double calculateOverhead() const override {
        return 3.0;
    }
    
    const char* getProtocolName() const override {
        return "4G (OFDM with MIMO - All IP Packet)";
    }
    
    unsigned int getMimoAntennas() const { return mimoAntennas; }
    
    void displayCoreInfo() const override;
};

class Core5G : public CellularCore {
private:
    unsigned int massiveMimoAntennas;
    bool millimeterWave;
    
public:
    Core5G(unsigned int id, unsigned int cap, unsigned int antennas = 16, bool mmWave = true) 
        : CellularCore(id, cap), massiveMimoAntennas(antennas), millimeterWave(mmWave) {}
    
    double calculateOverhead() const override {
        return 2.0;
    }
    
    const char* getProtocolName() const override {
        return "5G (Massive MIMO - High Frequency)";
    }
    
    unsigned int getMassiveMimoAntennas() const { return massiveMimoAntennas; }
    bool isMillimeterWave() const { return millimeterWave; }
    
    void displayCoreInfo() const override;
};

#endif