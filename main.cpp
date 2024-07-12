/*

   SMARTMAP ( v 1.3.1.2 beta )

                                */

#include "log.hpp"
#include "src/engine.hpp"
#include "effector.hpp"
#include "instance.hpp"
#include "vbo.hpp"
#include "drawcall.hpp"
#include "layer.hpp"



int main() {

    engine.init();

    FileEffector fileEffector(File("prout.yolo", ""));

    engine.open("/home/user/cpp/smartmap/project.json");

    engine.run();

}
