#ifndef H5_H
#define H5_H
#include "H5Group.hpp"
#include "highfive/H5DataSet.hpp"
#include "highfive/H5DataSpace.hpp"
#include "highfive/H5DataType.hpp"
#include "highfive/highfive.hpp"
#include <algorithm>
#include <cstddef>
#include <format>
#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>
class H5 {
public:
  enum TYPES {
    Time = 1 << 1,
    Integer = 1 << 2,
    Float = 1 << 3,
    String = 1 << 4,
    BitField = 1 << 5,
    Opaque = 1 << 6,
    Compound = 1 << 7,
    Reference = 1 << 8,
    Enum = 1 << 9,
    VarLen = 1 << 10,
    Array = 1 << 11,
    Invalid = 0
  };

public:
  H5();
  H5(std::string file);
  H5Group *get_groups();
  void populate_sets(H5Group *current_group, std::string pos);
  int number_of_groups();
  std::string &get_filename();
  int get_dim() { return dim; };
  int get_total_points() { return total_points; };
  std::vector<size_t> &get_shape() { return shape; };
  void select_dataset(std::string &path);
  TYPES get_datatype() { return current_type; };
  template <typename T>
  void read_selection(size_t row_off, size_t col_off, size_t row_span,
                      size_t col_span, std::vector<T> &buffer) {

    current_dataset.select({row_off, col_off}, {row_span, col_span})
        .read(buffer);
  }
  template <typename T>
  void create_dataset(size_t dim_y, size_t dim_x, std::string &dataset_name) {
      std::vector<size_t> dims{dim_y,dim_x};
      file_handler->createDataSet<double>(dataset_name,HighFive::DataSpace(dims));
      root->clear();
      populate_sets(root, "/");
       
  }

  static auto & get_type_names() { return type_name; }
  auto get_file_handle() { return file_handler; }
  void add_group(std::string name);
  void add_leaf(std::string name);
  std::string get_name();
  H5Group *get_group(const std::string &name);
  std::vector<std::string> &get_leafs();
  int size();
  void clear();
private:
  std::vector<std::string> attribute;
  std::vector<std::string> leafs;
  int filesize;
  std::string file_name;
  std::string file_path;
  HighFive::File *file_handler;
  H5Group *root;
  std::vector<size_t> shape;
  size_t dim;
  size_t total_points;
  HighFive::DataSet current_dataset;
  TYPES current_type;
  std::map<int, std::string> type_names;
  static std::vector<char const *> type_name;
  std::vector<H5Group *> groups;
  std::string name;
///d Merge H5 with H5Group   


};

#endif // MYCLASS_H
