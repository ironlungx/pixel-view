#pragma once

#ifndef ARDUINO

#include <string>
#include <cstring>
#include <algorithm>
#include <cctype>
#include <sstream>

#include "StringUtils.h"

/**
 * @class String
 * @brief A lightweight wrapper around std::string that mimics Arduino's String class
 * 
 * This implementation provides the core functionality of Arduino's String class
 * while using std::string internally for better performance on Raspberry Pi.
 */
class String {
private:
    std::string data;

public:
    // Constructors
    String() = default;
    String(const char* str) : data(str ? str : "") {}
    String(const std::string& str) : data(str) {}
    String(const String& str) : data(str.data) {}
    String(char c) : data(1, c) {}
    String(unsigned char c) : data(1, static_cast<char>(c)) {}
    String(int value, unsigned char base = 10);
    String(unsigned int value, unsigned char base = 10);
    String(long value, unsigned char base = 10);
    String(unsigned long value, unsigned char base = 10);
    String(float value, unsigned char decimalPlaces = 2);
    String(double value, unsigned char decimalPlaces = 2);
    
    // Assignment
    String& operator=(const String& rhs) {
        if (this != &rhs) data = rhs.data;
        return *this;
    }
    String& operator=(const char* cstr) {
        data = cstr ? cstr : "";
        return *this;
    }
    
    // Access
    char charAt(unsigned int index) const {
        return (index < data.length()) ? data[index] : 0;
    }
    void setCharAt(unsigned int index, char c) {
        if (index < data.length()) data[index] = c;
    }
    char operator[](unsigned int index) const {
        return charAt(index);
    }
    char& operator[](unsigned int index) {
        static char dummy = 0;
        return (index < data.length()) ? data[index] : dummy;
    }
    
    // Comparison
    bool equals(const String& s) const { return data == s.data; }
    bool equals(const char* s) const { return data == s; }
    bool operator==(const String& rhs) const { return equals(rhs); }
    bool operator==(const char* rhs) const { return equals(rhs); }
    bool operator!=(const String& rhs) const { return !equals(rhs); }
    bool operator!=(const char* rhs) const { return !equals(rhs); }
    bool operator<(const String& rhs) const { return data < rhs.data; }
    bool operator>(const String& rhs) const { return data > rhs.data; }
    bool operator<=(const String& rhs) const { return data <= rhs.data; }
    bool operator>=(const String& rhs) const { return data >= rhs.data; }
    
    // Concatenation
    String& concat(const String& str) {
        data += str.data;
        return *this;
    }
    String& concat(const char* cstr) {
        if (cstr) data += cstr;
        return *this;
    }
    String& concat(char c) {
        data += c;
        return *this;
    }
    String& concat(unsigned char c) {
        data += static_cast<char>(c);
        return *this;
    }
    String& concat(int num) {
        data += std::to_string(num);
        return *this;
    }
    String& concat(unsigned int num) {
        data += std::to_string(num);
        return *this;
    }
    String& concat(long num) {
        data += std::to_string(num);
        return *this;
    }
    String& concat(unsigned long num) {
        data += std::to_string(num);
        return *this;
    }
    String& concat(float num) {
        std::ostringstream oss;
        oss << num;
        data += oss.str();
        return *this;
    }
    String& operator+=(const String& rhs) { return concat(rhs); }
    String& operator+=(const char* cstr) { return concat(cstr); }
    String& operator+=(char c) { return concat(c); }
    String& operator+=(unsigned char c) { return concat(c); }
    String& operator+=(int num) { return concat(num); }
    String& operator+=(unsigned int num) { return concat(num); }
    String& operator+=(long num) { return concat(num); }
    String& operator+=(unsigned long num) { return concat(num); }
    
    // Combination
    friend String operator+(const String& lhs, const String& rhs) {
        String result = lhs;
        result.concat(rhs);
        return result;
    }
    friend String operator+(const String& lhs, const char* rhs) {
        String result = lhs;
        result.concat(rhs);
        return result;
    }
    friend String operator+(const char* lhs, const String& rhs) {
        String result = lhs;
        result.concat(rhs);
        return result;
    }
    
    // Search
    int indexOf(char ch) const {
        size_t pos = data.find(ch);
        return (pos == std::string::npos) ? -1 : static_cast<int>(pos);
    }
    int indexOf(char ch, unsigned int fromIndex) const {
        size_t pos = data.find(ch, fromIndex);
        return (pos == std::string::npos) ? -1 : static_cast<int>(pos);
    }
    int indexOf(const String& str) const {
        size_t pos = data.find(str.data);
        return (pos == std::string::npos) ? -1 : static_cast<int>(pos);
    }
    int indexOf(const String& str, unsigned int fromIndex) const {
        size_t pos = data.find(str.data, fromIndex);
        return (pos == std::string::npos) ? -1 : static_cast<int>(pos);
    }
    int lastIndexOf(char ch) const {
        size_t pos = data.rfind(ch);
        return (pos == std::string::npos) ? -1 : static_cast<int>(pos);
    }
    int lastIndexOf(char ch, unsigned int fromIndex) const {
        if (fromIndex >= data.length()) fromIndex = data.length() - 1;
        size_t pos = data.rfind(ch, fromIndex);
        return (pos == std::string::npos) ? -1 : static_cast<int>(pos);
    }
    
    // Modification
    void replace(char find, char replace) {
        std::replace(data.begin(), data.end(), find, replace);
    }
    void replace(const String& find, const String& replace) {
        size_t pos = 0;
        while ((pos = data.find(find.data, pos)) != std::string::npos) {
            data.replace(pos, find.length(), replace.data);
            pos += replace.length();
        }
    }
    String substring(unsigned int beginIndex) const {
        if (beginIndex >= data.length()) return String();
        return String(data.substr(beginIndex));
    }
    String substring(unsigned int beginIndex, unsigned int endIndex) const {
        if (beginIndex >= data.length()) return String();
        return String(data.substr(beginIndex, endIndex - beginIndex));
    }
    void remove(unsigned int index) {
        if (index < data.length()) {
            data.erase(index);
        }
    }
    void remove(unsigned int index, unsigned int count) {
        if (index < data.length()) {
            data.erase(index, count);
        }
    }
    void toLowerCase() {
        std::transform(data.begin(), data.end(), data.begin(), 
                      [](unsigned char c){ return std::tolower(c); });
    }
    void toUpperCase() {
        std::transform(data.begin(), data.end(), data.begin(), 
                      [](unsigned char c){ return std::toupper(c); });
    }
    void trim() {
        // Trim left
        data.erase(data.begin(), std::find_if(data.begin(), data.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
        // Trim right
        data.erase(std::find_if(data.rbegin(), data.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), data.end());
    }
    
    // Conversion
    const char* c_str() const { return data.c_str(); }
    char* begin() { return &data[0]; }
    char* end() { return &data[0] + data.length(); }
    const char* begin() const { return data.c_str(); }
    const char* end() const { return data.c_str() + data.length(); }
    
    int toInt() const { return std::stoi(data); }
    float toFloat() const { return std::stof(data); }
    double toDouble() const { return std::stod(data); }
    
    // Properties
    unsigned int length() const { return static_cast<unsigned int>(data.length()); }
    bool isEmpty() const { return data.empty(); }
    void reserve(unsigned int size) { data.reserve(size); }
    
    // Conversion operators
    operator std::string() const { return data; }
    operator const char*() const { return c_str(); }
};

// Constructor implementations
inline String::String(int value, unsigned char base) {
    char buf[34];
    if (base == 10) {
        data = std::to_string(value);
    } else {
        // Implementation for non-base-10 conversions
        // (This is simplified; you may need to improve this for your needs)
        itoa(value, buf, base);
        data = buf;
    }
}

inline String::String(unsigned int value, unsigned char base) {
    char buf[34];
    if (base == 10) {
        data = std::to_string(value);
    } else {
        // Implementation for non-base-10 conversions
        utoa(value, buf, base);
        data = buf;
    }
}

inline String::String(long value, unsigned char base) {
    char buf[34];
    if (base == 10) {
        data = std::to_string(value);
    } else {
        // Implementation for non-base-10 conversions
        ltoa(value, buf, base);
        data = buf;
    }
}

inline String::String(unsigned long value, unsigned char base) {
    char buf[34];
    if (base == 10) {
        data = std::to_string(value);
    } else {
        // Implementation for non-base-10 conversions
        ultoa(value, buf, base);
        data = buf;
    }
}

inline String::String(float value, unsigned char decimalPlaces) {
    char buf[34];
    snprintf(buf, sizeof(buf), "%.*f", decimalPlaces, value);
    data = buf;
}

inline String::String(double value, unsigned char decimalPlaces) {
    char buf[34];
    snprintf(buf, sizeof(buf), "%.*f", decimalPlaces, value);
    data = buf;
}

#endif
