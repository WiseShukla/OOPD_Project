// basicIO.h
#ifndef BASICIO_H
#define BASICIO_H

class BasicIO {
public:
    void outputstring(const char* str);
    void outputint(int num);
    void outputchar(char c);
    void terminate();
    void errorstring(const char* str);
    int inputint();
    char inputchar();
};

extern BasicIO io;

#endif