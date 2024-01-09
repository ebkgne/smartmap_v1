/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"

#include "smartmap.hpp"
#include "ndi.hpp"
#include "widgets/artnet_widget.hpp"
#include "widgets/file.hpp"
#include "widgets/stack.hpp"
#include "widgets/fps.hpp"

#include "shader_builder.hpp"
#include "model.hpp"
#include "tree.hpp"
#include "editor.hpp"

// shader sources
// clocks
// fix feedback blending

template <typename T>
struct Ptr : Node { T* ptr; Ptr(void* ptr) : ptr((T*)ptr) { name = ((Node*)ptr)->name + "ptr"; } };


int main() { 

    Log log;
    
    auto &engine = Engine::getInstance();
    engine.init();

    SmartMap::Base sm;

    auto *x = engine.dynamic_ubo.buffer.add("infos", {"int","int","int","int"},4);
    engine.specs = x->create();x->create();x->create();x->create();

    ShaderBuilder sb;
    
    TreeWidget tree;
    EditorWidget editor;

    struct DC : Node {

        struct ShaderFXPtr : Ptr<ShaderFX> {

            ShaderFXPtr(void* ptr) : Ptr<ShaderFX>(ptr) { }

            Node* add(Node *node) { return nullptr; }    

        };

        struct ModelPtr : Ptr<Model> {

            ModelPtr(void* ptr) : Ptr<Model>(ptr) { }
                
            Node* add(Node *node) {

                if (node->is_a<ShaderFX>()) return Node::add(new Ptr<ShaderFX>(node));

                return nullptr;

            }    

        };

        DC() {

            name = "drawcall";
        }
        
        Node* add(Node *node) {

            if (node->is_a<Model>()) return Node::add(new ModelPtr(node));

            return nullptr;

        }        
    };

    engine.tree.add(new DC());

    // sm.config.import("config.json");

    FPSWidget fpsw;

    Engine::Run();
 
}


