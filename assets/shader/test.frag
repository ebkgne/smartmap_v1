#version 430 core

struct Framebuffer { int width, height, x, y;  };
struct Rect { vec2 size;vec2 pos;  };
struct Mat { vec2 size;vec2 norm;vec2 pos; vec2 xxxxalign; };

struct Fixture {

    float alpha;
    float r;
    float g;
    float b;
    vec2 pos;
    vec2 size;
    vec4 gobo;
    float orientation; 
    float feedback; 
    float strobe;  
    float ratio; // unused for now mostly for alignment

};
struct Layer {

    int framebuffer;
    int canva_first;
    int canva_count;
    int fixture_first;

};

layout (binding = 2, std140) uniform mediasCoords { Rect[16] mediaCoord;};

layout (binding = 0, std140) uniform dynamic_ubo { Fixture fix[50]; Fixture fix2[50];  int seq[4]; };
layout (binding = 1, std140) uniform static_ubo { Framebuffer framebuffer[100]; Mat mat[100]; Layer layer[10]; };


out vec4 color;

uniform sampler2D tex;
flat in int obj;
flat in int id;
in vec2 texcoord;

void main() { 

    color = vec4(1,1,0,1); return;
    
}

