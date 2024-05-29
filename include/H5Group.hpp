#ifndef H5Group_H
#define H5Group_H

#include <vector>
#include <string>
class H5Group {
public:
  // Constructor
  H5Group();
  H5Group(std::string name);
  void add_group(std::string name);
  void add_leaf(std::string name);
  std::string get_name();
  H5Group *get_group(const std::string &name);
  std::vector<std::string> &get_leafs();
  std::vector<H5Group *> & get_groups();
  int size();
  void clear();
  // Member function
private:
  std::vector<std::string> leafs;
  std::vector<H5Group *> groups;
  std::string name;
};

#endif // H5Group_H
