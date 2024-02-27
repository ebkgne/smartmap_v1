#include "callbacks.hpp"

#include "node.hpp"
#include "file.hpp"
#include "ubo.hpp"
#include "struct.hpp"
#include "model.hpp"
#include "effector.hpp"
#include "directory.hpp"
#include "drawcall.hpp"
#include "engine.hpp"
#include "artnet.hpp"
#include "ndi.hpp"
#include "atlas.hpp"

void Callbacks::init() {
    
    // ////////// xxx.HPP 
    
    // NODE<xxx>::oncreate([](Node* node, xxx *x){ });
    // NODE<xxx>::onchange([](Node* node, xxx *x){ });
    // NODE<xxx>::onrun([](Node* node, xxx *x){ });

    // NODE<xxx>::onadd<yyy>([](Node*_this,Node*node){ 

    //     auto s = _this->is_a<xxx>();
    //     auto f = node->is_a<yyy>();

    //     return _this->UntypedNode::add(node);
        
    // });

    // NODE<Folder>::oncreate([](Node* node, Folder *folder){ 




    // });

    ////////// FILE.HPP 

    NODE<File>::onrun([](Node* node, File *file){ 

        if (file->hasChanged()) { 

            PLOGD <<  "file " << file->name << " changed";

            file->reload(); 

            node->update();
   
        }

     });

    ////////// Artnet.HPP 

    NODE<Artnet>::onrun([](Node* node, Artnet *an){ an->run(); 
    static int size = 0;

    if (an->universes.size() != size) {
        size = an->universes.size();
        // node->get<Artnet>()->trigchange();
    }
    });

    NODE<Artnet>::onchange([](Node* node, Artnet *an){ 

        for (auto c :node->childrens) { delete c; };

        for (auto &uni :an->universes) { 

            uni.second->id = uni.first;

            node->addPtr<DMX>(uni.second)->name = "universe "+std::to_string(uni.first);

        }

    });

    ////////// UBO.HPP 

    NODE<UBO>::onrun([](Node* node, UBO *ubo){ ubo->upload(); });
    NODE<UBO>::onchange([](Node* node, UBO *ubo){ ubo->update(); 
    node->updateRefs(node); 
    });
    
    NODE<UBO>::oncreate([](Node* node, UBO *ubo){ node->name = ubo->name; });

    ////////// STRUCT.HPP 

    NODE<Struct>::oncreate([](Node* node, Struct *s){ node->name = s->name; });

    ////////// ENGINE.HPP (and Stack)

    NODE<Stack>::onadd<DrawCall>([](Node*_this,Node*node){ 

        node->is_a<DrawCall>()->update();

        return node; // missind add() use case (still ? maybe not untyped)

        // maybe moved stored_type to UntypedNode and use if!=UntypedNode get name and so on

    });

    NODE<Stack>::onadd<Layer>([](Node*_this,Node*node){ 

        node->is_a<Layer>()->update();

        return node;

    });

    NODE<Stack>::onadd<UBO>([](Node*_this,Node*node){ return node; });

    NODE<Stack>::onadd<File>([](Node*_this,Node*node){ return node; });

    ////////// DRAWCALL.HPP 

    NODE<Layer>::onrun([](Node* node, Layer *layer){  if (layer->shader.loaded) layer->run(); });
    
    NODE<Layer>::onchange([](Node* node, Layer *layer){ layer->update(); });

    NODE<Layer>::onadd<Model>([](Node*_this,Node*node){

        auto layer = _this->is_a<Layer>();
        auto model = node->is_a<Model>();

        layer->vbo.import(model);  

        auto x = new Ptr<Model>(model);

        return x->node(); 
        
    });
    
    ////////// MODEL.HPP 

    NODE<Model>::oncreate([](Node* node, Model *model) { node->name = model->file->name; });

    NODE<Model>::onadd<Effector>([](Node*_this,Node*node){ return (new Ptr<Effector>(node->is_a<Effector>()))->node(); });

    NODE<Model>::onadd<Effector>([](Node*_this,Node*node){ 
        
        auto model = _this->is_a<Model>();
        auto effector = node->is_a<Effector>();

        model->addFX(effector);

        auto dc = _this->parent()->is_a<Layer>();
        if (dc) {

            dc->update();

            _this->referings.push_back(node);

        }

        return _this;

    });

    ////////// Effector.HPP 
    
    NODE<Effector>::oncreate([](Node* node, Effector *effector) { node->name = effector->file->name; });

    NODE<Effector>::onchange([](Node* node, Effector *effector) { PLOGD<<"update " << effector->file->name; });

    ////////// Atlas.HPP 

    // NODE<Atlas>::onchange([](Node* node, Atlas *atlas) { atlas->update(); });
    
    ////////// ShaderProgram.HPP 

    // NODE<ShaderProgram>::onchange([](Node* node, ShaderProgram *shader) { });

    ////////// Directory.HPP 

    NODE<Directory>::oncreate([](Node* node, Directory *dir){ 

        node->name = dir->path;

        for (auto f : dir->list) node->addOwnr<File>(f);
        
    });

    ////////// NDI.HPP 

    NODE<NDI::Sender>::oncreate([](Node* node, NDI::Sender *sender){ sender->init(); });
    NODE<NDI::Sender>::onrun([](Node* node, NDI::Sender *sender){ sender->tick(); });





}