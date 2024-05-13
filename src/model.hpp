#pragma once

struct File;
struct Effector;

#include <vector>
#include <string>

#include "struct.hpp"
#include "effector.hpp"

struct Model {

    File* file;

    Struct s;

    Model(File* f, std::string name);

    ~Model();

    Effector* addEffector(File* file);

    bool remove(Effector* effector);

    std::vector<std::shared_ptr<Effector>> effectors;

    static void fbx(File* model);

};
