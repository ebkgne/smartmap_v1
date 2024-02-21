#include "dmx.hpp"
#include "struct.hpp"
#include "component.hpp"

DMX::DMX() { memset(&data[0],0,512); } 


DMX::Fixture::Fixture(Struct* s) : s(s) {

    for (auto c : s->comps) c->each([this](Member &m){ attributes.push_back({1,m.range_from,m.range_to}); });

}

void DMX::update() {

    for (auto r : remaps) r.update();
    
    for (auto cb:callbacks) cb(this); 

}

uint16_t DMX::Remap::get16(void* data) { auto &ui8 = *(uint8_t*)data; return ((ui8 << 8) | ui8+1);  }
uint32_t DMX::Remap::get24(void* data) { auto &ui8 = *(uint8_t*)data; return ((ui8 << 16) | (ui8+1 << 8) | ui8+2);  }
uint32_t DMX::Remap::get32(void* data) { auto &ui8 = *(uint8_t*)data; return ((ui8 << 24) | (ui8+1 << 16) | (ui8+2 << 8) | ui8+3);  }

void DMX::Remap::update() {
    
    auto data = src;

    for (int offset = 0; offset < quantity; offset++) {  
        
        auto pos = (offset*fixture->attributes.size());

        for (int i = 0; i < fixture->attributes.size(); i++) { 

            float target = 0;
            auto c = fixture->attributes[i].combining;
            if (c==1) target      = GMAui2f[(uint8_t)*data];
            else if (c==2) target = get16(data)/65535.0f;
            else if (c==3) target = get24(data)/16777215.0f;
            else if (c==4) target = get32(data)/4294967295.0f;

            // range remap
            target = (target * (fixture->attributes[i].max - fixture->attributes[i].min)) + fixture->attributes[i].min;

            (*(((float*)dest)+i+pos)) = target;

            data += c;
            
        }

    } 

}

