// StringUtils.cpp
#include "StringUtils.h"

unsigned int my_strlen(const char* str) {
    unsigned int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

char* my_strcpy(char* dest, const char* src) {
    char* original = dest;
    while (*src != '\0') {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
    return original;
}

int my_strcmp(const char* str1, const char* str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(const unsigned char*)str1 - *(const unsigned char*)str2;
}

const char* my_strstr(const char* haystack, const char* needle) {
    if (*needle == '\0') {
        return haystack;
    }
    
    while (*haystack) {
        const char* h = haystack;
        const char* n = needle;
        
        while (*h && *n && (*h == *n)) {
            h++;
            n++;
        }
        
        if (*n == '\0') {
            return haystack;
        }
        
        haystack++;
    }
    
    return nullptr;
}

char* my_strcat(char* dest, const char* src) {
    char* original = dest;
    
    while (*dest != '\0') {
        dest++;
    }
    
    while (*src != '\0') {
        *dest = *src;
        dest++;
        src++;
    }
    
    *dest = '\0';
    return original;
}