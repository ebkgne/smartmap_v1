#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <vector>
#include <set>
#include <iostream>
#include <string.h>


#include "gui.hpp"


struct Attribute {

  static inline std::vector<float> UBO;

  int id = -1;

  const char* name;

  std::vector<Attribute*> childrens;

  float min_val, max_val, def_val, range_val;
 
  std::vector<float> local{1}; 

  std::vector<float>* buffer = &UBO; 

  Attribute(const char* name, float val, float min_val = 0, float max_val = 1, std::vector<float>* dst = nullptr) ;

  void gui();

  float* ptr();

  Attribute(const char* name, std::set<Attribute*> childrens, float min_val = 0, float max_val = 1);

  Attribute(std::set<Attribute*> childrens);

  Attribute* link_src = nullptr;

  std::set<Attribute*> links_dst;

  float get() ;

  virtual void set(float val) { (*buffer)[id] = val;  for (auto l:links_dst) l->set(val); } 
 
  void setNormalized(float val);
 
  Attribute* child(int id = 0);
 
  Attribute* child(const char* name);
 
  void linkFrom(Attribute* src);

  void linkTo(Attribute* dst);

  ~Attribute();

};




#endif