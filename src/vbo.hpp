#pragma once

#include "struct.hpp"
#include "buffer.hpp"
#include "model.hpp"
#include <cstdint>
#include <vector>

struct Object;
struct Model;
struct File;

struct VBO : BufferOwner {

    static inline Struct *vertices_s = new Struct("Vertex", {"Position","UV","ID",});
    static inline Struct *indices_s = new Struct("Index",{"Vertex", "Vertex", "Vertex"});

    static inline Buffer buffer;
    uint32_t vao, vbo, ibo;
    
    Object *vertices;
    Object *indices;
    
    VBO();

    ~VBO();

    void upload();
    void update() override;

    void create();
    void destroy();

    void draw(int count = 1);
       
    Model* import(File *file);

    std::vector<Model> models;

};