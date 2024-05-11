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
#include "json.hpp"
#include "buffer.hpp"

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

    NODE<File>::oncreate([](Node* node, File *file){ node->name(file->name()); });

    NODE<File>::onrun([](Node* node, File *file){

        // if (file->last_modified != file->getTimeModified()) file->reload();
        if (file->hasChanged()) {
            file->reload();
            node->update();


            }

    });

    ////////// Struct.HPP

    NODE<Struct>::oncreate([](Node*node, Struct* s){ node->name(s->name()); });

    NODE<Struct>::onchange([&](Node*node, Struct* s){ s->name(node->name()); });

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

            node->addPtr<DMX>(uni.second)->name("universe "+std::to_string(uni.first));

        }

    });

    ////////// UBO.HPP

    NODE<UBO>::onrun([](Node* node, UBO *ubo){ ubo->upload(); });

    NODE<UBO>::oncreate([](Node* node, UBO *ubo){ node->name(ubo->name()); });

    ////////// STRUCT.HPP

    NODE<Struct>::oncreate([](Node* node, Struct *s){ node->name(s->name()); });

    ////////// ENGINE.HPP (and Stack)

    NODE<Stack>::onadd<File>([](Node*_this,Node*node){ auto x = _this->addOwnr<Layer>(); x->add(node); return x->node(); });

    ////////// DRAWCALL.HPP

    NODE<Layer>::oncreate([](Node* node, Layer *layer){ NODE<Struct>::oncreate_cb(node, &layer->s); node->referings.insert(nullptr); }); // for what ??????

    NODE<Layer>::onrun([](Node* node, Layer *layer){ layer->draw(); });

    NODE<Layer>::onchange([](Node* node, Layer *layer){ NODE<Struct>::onchange_cb(node, &layer->s);   layer->update(); });

    NODE<Layer>::onadd<File>([](Node*_this,Node*node){

        auto file = node->is_a<File>();

        if (file->extension == "glsl") return _this->addPtr<Effector>( _this->is_a<Layer>()->addEffector(file) )->node();

        return _this->addPtr<Model>(_this->is_a<Layer>()->add( file ))->node();

        });

    ////////// MODEL.HPP

    NODE<Model>::oncreate([](Node*node, Model* mod){ NODE<Struct>::oncreate_cb(node, &mod->s); });

    NODE<Model>::onchange([&](Node*node, Model* mod){ NODE<Struct>::onchange_cb(node, &mod->s); /*PLOGD << engine.dynamic_ubo.print_recurse();*/ });

    NODE<Model>::onadd<File>([](Node*_this,Node*node){

        auto file = node->is_a<File>();

        if (file->extension != "glsl") { PLOGW << "WARUM :GLSL only BB !!"; return _this;}

        auto x =  _this->is_a<Model>()->add( file )  ;

        return _this->addPtr<Effector>( x )->node();

    });

    NODE<Model>::ondelete([](Node* node, Model *model) {

        auto dc = node->parent()->is_a<DrawCall>();
        if (dc) dc->remove(model);

        auto layer = node->parent()->is_a<Layer>();
        if (layer) layer->remove(model);

    });

    ////////// Effector.HPP

    NODE<Effector>::oncreate([](Node*node, Effector* fx){ NODE<Struct>::oncreate_cb(node, &fx->ref); });

    NODE<Effector>::onchange([&](Node*node, Effector* fx){ NODE<Struct>::onchange_cb(node, &fx->ref); });

    NODE<Effector>::ondelete([](Node* node, Effector *effector) {

        auto model = node->parent()->is_a_nowarning<Model>();

        if (!model) return node->parent()->is_a<Layer>()->removeEffector(effector);

        model->remove(effector);

    });

    ////////// Atlas.HPP

    NODE<Atlas>::onchange([](Node* node, Atlas *atlas) { atlas->fromDir(atlas->path); });

    //////// Remap.HPP

    NODE<Remap>::onrun([](Node* node, Remap *remap) { remap->update(); });
    NODE<Remap>::onchange([](Node* node, Remap *remap) { remap->reset(); });
    NODE<DMX::Remap>::onrun([](Node* node, DMX::Remap *remap) { remap->update(); });
    NODE<DMX::Remap>::onchange([](Node* node, DMX::Remap *remap) { remap->reset(); });

    //////// Buffer.HPP

    NODE<Buffer>::onchange([](Node* node, Buffer *buffer) { PLOGD<<"ooo"; });

    ////////// Directory.HPP

    NODE<Directory>::oncreate([](Node* node, Directory *dir){ node->name(dir->path); });

    ////////// NDI.HPP

    NODE<NDI::Sender>::oncreate([](Node* node, NDI::Sender *sender){ sender->init(); });

    NODE<NDI::Sender>::onrun([](Node* node, NDI::Sender *sender){ sender->tick(); });

    ////////// JSON.HPP

    NODE<JSON>::oncreate([](Node* node, JSON *json){ if (json->file) node->name(json->file->name()); });

}
