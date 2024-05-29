#include "H5.hpp"
#include "H5Group.hpp"
#include "highfive/H5DataType.hpp"
#include "highfive/H5File.hpp"
#include "highfive/H5Group.hpp"
#include "highfive/H5Object.hpp"
#include "highfive/bits/H5Iterables_misc.hpp"
#include "highfive/bits/H5Node_traits.hpp"
#include <cstddef>
#include <iostream>
#include <vector>

H5::H5() {}

H5::H5(std::string filename) {
  file_path = filename;
  file_name = file_path.substr(file_path.find_last_of("/") + 1);
  file_handler = new HighFive::File(file_path);
  filesize = file_handler->getFileSize();
  root = new H5Group(file_name);
  populate_sets(root, "/");

  type_names[Time]      = "Time";
  type_names[Integer]   = "Integer";
  type_names[Float]     = "Float";
  type_names[String]    = "String";
  type_names[BitField]  = "BitField";
  type_names[Opaque]    = "Opaque";
  type_names[Compound]  = "Compound";
  type_names[Reference] = "Reference";
  type_names[Enum]      = "Enum";
  type_names[VarLen]    = "VarLen";
  type_names[Array]     = "Array";
  type_names[Invalid]   = "Invalid";

  }
  std::vector<char const *> H5::type_name = {
      "Time",     "Integer",   "Float", "String", "BitField", "Opaque",
      "Compound", "Reference", "Enum",  "VarLen", "Array",    "Invalid"};

  void H5::select_dataset(std::string &path) {
  current_dataset = file_handler->getDataSet(path);
  auto space = current_dataset.getSpace();
  dim = space.getNumberDimensions();
  total_points = space.getElementCount();
  shape = space.getDimensions();
  auto true_type = current_dataset.getDataType().getClass();
  switch (true_type) {
  case HighFive::DataTypeClass::Integer:
    current_type = Integer;
    break;
  case HighFive::DataTypeClass::Float:
    current_type = Float;
    break;
  case HighFive::DataTypeClass::String:
    current_type = String;
    break;
  default:
    current_type = Invalid;
  }
}

void H5::populate_sets(H5Group *current_group, std::string path) {

  std::cout << "fetching path: " << path << std::endl;
  auto group = file_handler->getGroup(path);
  auto all_attr = group.listObjectNames();
  for (const auto &attr : all_attr) {
    auto current = path + "/" + attr;

    switch (file_handler->getObjectType(current)) {
    case HighFive::ObjectType::Group:
      current_group->add_group(attr);
      populate_sets(current_group->get_group(attr), current);
      break;
    case HighFive::ObjectType::Dataset:
      current_group->add_leaf(attr);
      leafs.push_back(current);
      break;
    default:
      std::cout << "unknown tybe h5" << std::endl;
    }
  }
}
int H5::number_of_groups() { return root->size(); }

std::string &H5::get_filename() { return file_name; }
H5Group *H5::get_groups() { return root; }
void H5Group::add_group(std::string name) {
  auto new_group = new H5Group(name);
  groups.push_back(new_group);
}
void H5::add_leaf(std::string name) {
  leafs.push_back(name);
}
H5Group * H5::get_group(const std::string & name){
  for(auto g : groups)
    if(g->get_name() == name)
      return g;
  return nullptr;
}

std::string H5::get_name(){
  return name;
}
int H5Group::size(){
  return groups.size();
}
std::vector<H5Group *> & H5::get_groups() {
  return groups;
}
std::vector<std::string> &H5::get_leafs() { return leafs; }

void H5::clear(){
  //groups.clear();
  for(auto &g: groups)
    g->clear();
  leafs.clear();
}
