#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstddef>

#include "object.hpp"

struct Struct;
struct Buffer;

struct Buffer {
    
    std::vector<Object> objects;

    std::function<void()> callback;

    Buffer();

    Object* addObj(Struct* s, int reserved = 0);

    void update();

    void reset();
    
    void destroy();

    std::vector<char> data;  

    char* getEntry(Struct *s, int eq = 0);

    Object* operator[](int id) { return &objects[id]; }

};