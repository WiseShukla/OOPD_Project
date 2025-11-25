// basicIO.cpp
#include "basicIO.h"

extern "C" long syscall3(long number, long arg1, long arg2, long arg3);

#define SYS_WRITE 1
#define SYS_READ 0
#define STDOUT 1
#define STDERR 2
#define STDIN 0

BasicIO io;

void BasicIO::outputstring(const char* str) {
    int len = 0;
    while (str[len] != '\0') len++;
    syscall3(SYS_WRITE, STDOUT, (long)str, len);
}

void BasicIO::outputint(int num) {
    if (num < 0) {
        outputchar('-');
        num = -num;
    }
    
    if (num == 0) {
        outputchar('0');
        return;
    }
    
    char buffer[32];
    int i = 0;
    
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    while (i > 0) {
        outputchar(buffer[--i]);
    }
}

void BasicIO::outputchar(char c) {
    syscall3(SYS_WRITE, STDOUT, (long)&c, 1);
}

void BasicIO::terminate() {
    outputchar('\n');
}

void BasicIO::errorstring(const char* str) {
    int len = 0;
    while (str[len] != '\0') len++;
    syscall3(SYS_WRITE, STDERR, (long)str, len);
}

int BasicIO::inputint() {
    char buffer[32];
    int i = 0;
    char c;
    bool negative = false;
    
    // Skip whitespace
    do {
        syscall3(SYS_READ, STDIN, (long)&c, 1);
    } while (c == ' ' || c == '\t' || c == '\n' || c == '\r');
    
    // Check for negative
    if (c == '-') {
        negative = true;
        syscall3(SYS_READ, STDIN, (long)&c, 1);
    }
    
    // Read digits
    while (c >= '0' && c <= '9' && i < 31) {
        buffer[i++] = c;
        syscall3(SYS_READ, STDIN, (long)&c, 1);
    }
    
    buffer[i] = '\0';
    
    // Convert to integer
    int result = 0;
    for (int j = 0; j < i; j++) {
        result = result * 10 + (buffer[j] - '0');
    }
    
    return negative ? -result : result;
}

char BasicIO::inputchar() {
    char c;
    syscall3(SYS_READ, STDIN, (long)&c, 1);
    return c;
}