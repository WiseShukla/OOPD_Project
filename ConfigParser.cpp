// ConfigParser.cpp
#include "ConfigParser.h"
#include "StringUtils.h"
#include "basicIO.h"

extern "C" long syscall3(long number, long arg1, long arg2, long arg3);

#define SYS_OPEN 2
#define SYS_READ 0
#define SYS_CLOSE 3
#define O_RDONLY 0

const char* ConfigParser::findColon(const char* str) {
    while (*str) {
        if (*str == ':') return str;
        str++;
    }
    return nullptr;
}

unsigned int ConfigParser::extractValue(const char* line) {
    const char* colon = findColon(line);
    if (!colon) return 0;
    
    colon++;
    while (*colon == ' ' || *colon == '\t') colon++;
    
    unsigned int result = 0;
    while (*colon >= '0' && *colon <= '9') {
        result = result * 10 + (*colon - '0');
        colon++;
    }
    
    return result;
}

void ConfigParser::parseLine(const char* line, SimulationConfig& config) {
    if (line[0] == '\0' || line[0] == '#') return;
    
    // 2G Configuration
    if (my_strstr(line, "2G_USERS:")) {
        config.config2G.users = extractValue(line);
    } else if (my_strstr(line, "2G_BANDWIDTH:")) {
        config.config2G.bandwidth = extractValue(line);
    } else if (my_strstr(line, "2G_CHANNEL_BW:")) {
        config.config2G.channelBw = extractValue(line);
    } else if (my_strstr(line, "2G_USERS_PER_CHANNEL:")) {
        config.config2G.usersPerChannel = extractValue(line);
    } else if (my_strstr(line, "2G_DATA_MESSAGES:")) {
        config.config2G.dataMessages = extractValue(line);
    } else if (my_strstr(line, "2G_VOICE_MESSAGES:")) {
        config.config2G.voiceMessages = extractValue(line);
    } else if (my_strstr(line, "2G_OVERHEAD")) {
        config.config2G.overhead = extractValue(line);
    }
    // 3G Configuration
    else if (my_strstr(line, "3G_USERS:")) {
        config.config3G.users = extractValue(line);
    } else if (my_strstr(line, "3G_BANDWIDTH:")) {
        config.config3G.bandwidth = extractValue(line);
    } else if (my_strstr(line, "3G_CHANNEL_BW:")) {
        config.config3G.channelBw = extractValue(line);
    } else if (my_strstr(line, "3G_USERS_PER_CHANNEL:")) {
        config.config3G.usersPerChannel = extractValue(line);
    } else if (my_strstr(line, "3G_MESSAGES_PER_USER:")) {
        config.config3G.messagesPerUser = extractValue(line);
    } else if (my_strstr(line, "3G_OVERHEAD")) {
        config.config3G.overhead = extractValue(line);
    }
    // 4G Configuration
    else if (my_strstr(line, "4G_USERS:")) {
        config.config4G.users = extractValue(line);
    } else if (my_strstr(line, "4G_BANDWIDTH:")) {
        config.config4G.bandwidth = extractValue(line);
    } else if (my_strstr(line, "4G_CHANNEL_BW:")) {
        config.config4G.channelBw = extractValue(line);
    } else if (my_strstr(line, "4G_USERS_PER_CHANNEL:")) {
        config.config4G.usersPerChannel = extractValue(line);
    } else if (my_strstr(line, "4G_ANTENNAS:")) {
        config.config4G.antennas = extractValue(line);
    } else if (my_strstr(line, "4G_MESSAGES_PER_USER:")) {
        config.config4G.messagesPerUser = extractValue(line);
    } else if (my_strstr(line, "4G_OVERHEAD")) {
        config.config4G.overhead = extractValue(line);
    }
    // 5G Configuration
    else if (my_strstr(line, "5G_USERS:")) {
        config.config5G.users = extractValue(line);
    } else if (my_strstr(line, "5G_BANDWIDTH:")) {
        config.config5G.bandwidth = extractValue(line);
    } else if (my_strstr(line, "5G_CHANNEL_BW:")) {
        config.config5G.channelBw = extractValue(line);
    } else if (my_strstr(line, "5G_USERS_PER_MHZ:")) {
        config.config5G.usersPerMHz = extractValue(line);
    } else if (my_strstr(line, "5G_ANTENNAS:")) {
        config.config5G.antennas = extractValue(line);
    } else if (my_strstr(line, "5G_MESSAGES_PER_USER:")) {
        config.config5G.messagesPerUser = extractValue(line);
    } else if (my_strstr(line, "5G_OVERHEAD")) {
        config.config5G.overhead = extractValue(line);
    }
    // Core Configuration
    else if (my_strstr(line, "CORE_CAPACITY:")) {
        config.coreCapacity = extractValue(line);
    }
}

bool ConfigParser::readLine(int fd, char* buffer, int maxLen) {
    int i = 0;
    char ch;
    
    while (i < maxLen - 1) {
        long bytes = syscall3(SYS_READ, fd, (long)&ch, 1);
        if (bytes <= 0) {
            buffer[i] = '\0';
            return (i > 0);
        }
        
        if (ch == '\n') {
            buffer[i] = '\0';
            return true;
        }
        
        buffer[i++] = ch;
    }
    
    buffer[i] = '\0';
    return true;
}

SimulationConfig ConfigParser::parseFile(const char* filename) {
    SimulationConfig config;
    
    config.config2G = {0, 0, 0, 0, 0, 0, 0};
    config.config3G = {0, 0, 0, 0, 0, 0};
    config.config4G = {0, 0, 0, 0, 0, 0, 0};
    config.config5G = {0, 0, 0, 0, 0, 0, 0};
    config.coreCapacity = 10000;
    
    long fd = syscall3(SYS_OPEN, (long)filename, O_RDONLY, 0);
    if (fd < 0) {
        throw "Cannot open configuration file";
    }
    
    char buffer[256];
    while (readLine(fd, buffer, 256)) {
        parseLine(buffer, config);
    }
    
    syscall3(SYS_CLOSE, fd, 0, 0);
    
    return config;
}