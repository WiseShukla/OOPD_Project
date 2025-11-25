// CellularCore.cpp
#include "CellularCore.h"
#include "basicIO.h"

void Core2G::displayCoreInfo() const {
    io.outputstring("2G Core - ID: ");
    io.outputint(coreId);
    io.outputstring(", Capacity: ");
    io.outputint(capacity);
    io.outputstring(", Load: ");
    io.outputint(currentLoad);
    io.outputstring(", Overhead: ");
    io.outputint((unsigned int)calculateOverhead());
    io.outputstring("%");
    io.terminate();
}

void Core3G::displayCoreInfo() const {
    io.outputstring("3G Core - ID: ");
    io.outputint(coreId);
    io.outputstring(", Capacity: ");
    io.outputint(capacity);
    io.outputstring(", Load: ");
    io.outputint(currentLoad);
    io.outputstring(", Overhead: ");
    io.outputint((unsigned int)calculateOverhead());
    io.outputstring("%");
    io.terminate();
}

void Core4G::displayCoreInfo() const {
    io.outputstring("4G Core - ID: ");
    io.outputint(coreId);
    io.outputstring(", Capacity: ");
    io.outputint(capacity);
    io.outputstring(", MIMO: ");
    io.outputint(mimoAntennas);
    io.outputstring(", Load: ");
    io.outputint(currentLoad);
    io.outputstring(", Overhead: ");
    io.outputint((unsigned int)calculateOverhead());
    io.outputstring("%");
    io.terminate();
}

void Core5G::displayCoreInfo() const {
    io.outputstring("5G Core - ID: ");
    io.outputint(coreId);
    io.outputstring(", Capacity: ");
    io.outputint(capacity);
    io.outputstring(", Massive MIMO: ");
    io.outputint(massiveMimoAntennas);
    io.outputstring(", Load: ");
    io.outputint(currentLoad);
    io.outputstring(", Overhead: ");
    io.outputint((unsigned int)calculateOverhead());
    io.outputstring("%");
    io.terminate();
}