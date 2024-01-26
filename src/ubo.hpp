#pragma once

#include <vector>

#include "buffer.hpp"
#include "component.hpp"

struct ShaderProgram;
struct UBO : Node {

    uint32_t id = -1;

    uint32_t binding;

    static inline uint32_t binding_count = 0;

    static inline Components &comps = Components::getInstance();

    std::vector<ShaderProgram*> subscribers;

    Buffer buffer;

    ~UBO();
    
    UBO(std::string name, std::vector<ShaderProgram*> subscribers = {});

    void destroy();

    void update();

    void upload();

    void upload(GLvoid* data, size_t size, GLuint offset = 0);

    Node *add(Node* node) override;

    void editor() override;
    
    // static void toJSON();
    
    // static void fromJSON();

};