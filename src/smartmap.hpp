#pragma once

#include "window.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "framebuffer.hpp"
#include "vbo.hpp"
#include "atlas.hpp"
#include "ubo.hpp"
#include "gui.hpp"
#include "device/artnet.hpp"

struct SmartMap {

    Artnet *artnet;

    GL::Window *window;

    VBO *quadA, *quadB, *quadC;

    Texture *passBuf, *outBuf, *outBlur;

    FrameBuffer *outFB, *winFB;

    ShaderProgram *basic, *shader, *blur_x, *blur_y;

    GUI *gui;

    UBO *matriceUBO, *fixtureUBO; 

    uint16_t FW, FH, MAT_X, MAT_Y, MATS;

    Atlas *atlas;

    void createFixtures(int count);

    static SmartMap& getInstance();

    void render();

private:

    SmartMap();
    SmartMap(const SmartMap&) = delete;
    SmartMap& operator=(const SmartMap&) = delete;
};

extern SmartMap& sm;