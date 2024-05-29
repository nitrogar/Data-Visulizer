
#include "H5Group.hpp"
#include <vector>

H5Group::H5Group(){}

H5Group::H5Group(std::string name) {
  this->name = name;
}
void H5Group::add_group(std::string name) {
  auto new_group = new H5Group(name);
  groups.push_back(new_group);
}
void H5Group::add_leaf(std::string name) {
  leafs.push_back(name);
}
H5Group * H5Group::get_group(const std::string & name){
  for(auto g : groups)
    if(g->get_name() == name)
      return g;
  return nullptr;
}

std::string H5Group::get_name(){
  return name;
}
int H5Group::size(){
  return groups.size();
}
std::vector<H5Group *> & H5Group::get_groups() {
  return groups;
}

std::vector<std::string> &H5Group::get_leafs() { return leafs; }

void H5Group::clear(){
  //groups.clear();
  for(auto &g: groups)
    g->clear();
  leafs.clear();
}
