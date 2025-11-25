// ConfigParser.h
#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

struct Config2G {
    unsigned int users;
    unsigned int bandwidth;
    unsigned int channelBw;
    unsigned int usersPerChannel;
    unsigned int dataMessages;
    unsigned int voiceMessages;
    unsigned int overhead;
};

struct Config3G {
    unsigned int users;
    unsigned int bandwidth;
    unsigned int channelBw;
    unsigned int usersPerChannel;
    unsigned int messagesPerUser;
    unsigned int overhead;
};

struct Config4G {
    unsigned int users;
    unsigned int bandwidth;
    unsigned int channelBw;
    unsigned int usersPerChannel;
    unsigned int antennas;
    unsigned int messagesPerUser;
    unsigned int overhead;
};

struct Config5G {
    unsigned int users;
    unsigned int bandwidth;
    unsigned int channelBw;
    unsigned int usersPerMHz;
    unsigned int antennas;
    unsigned int messagesPerUser;
    unsigned int overhead;
};

struct SimulationConfig {
    Config2G config2G;
    Config3G config3G;
    Config4G config4G;
    Config5G config5G;
    unsigned int coreCapacity;
};

class ConfigParser {
public:
    static SimulationConfig parseFile(const char* filename);
    
private:
    static bool readLine(int fd, char* buffer, int maxLen);
    static void parseLine(const char* line, SimulationConfig& config);
    static unsigned int extractValue(const char* line);
    static const char* findColon(const char* str);
};

#endif