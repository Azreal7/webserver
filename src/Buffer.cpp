#include "Buffer.h"
#include <string>
#include <iostream>

Buffer::Buffer() {
    buf.clear();
}

Buffer::~Buffer() {

}

void Buffer::append(const char* _str, int _size) {
    // printf("appending!\n");
    for(int i = 0; i < _size; ++i) {
        if(_str[i] == '\0') break;
        buf.push_back(_str[i]);
    }
}

ssize_t Buffer::size() {
    return buf.size();
}

const char* Buffer::c_str() {
    return buf.c_str();
}

void Buffer::clear() {
    buf.clear();
}

void Buffer::setBuf(const char* _buf){
    buf.clear();
    buf.append(_buf);
}