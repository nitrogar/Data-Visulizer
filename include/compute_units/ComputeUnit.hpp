#ifndef COMPUTEUNIT_H // If COMPUTEUNIT_H is not defined
#define COMPUTEUNIT_H // Define COMPUTEUNIT_H

#include "H5.hpp"
#include <boost/compute.hpp>
#include <boost/proto/detail/remove_typename.hpp>
#include <map>
#include <string>
#include <utility>
#include <variant>
#include <vector>

class App;
namespace compute = boost::compute;
class ComputeUnit {
public:
  typedef struct parameter_list {
    std::string dataset_name;
    std::variant<int, float> arg0;
  } parameter_list;
  ComputeUnit(App *app, std::string name, std::string descritpion)
      : app(app), name(name), description(descritpion){};
  virtual void compute(){};
  void load_parameter(std::string &param, parameter_list &pl) {
    parameters[param] = pl;
  };
  // H5 *get_dataset(std::string &param) {  };

  enum PARAM_TYPES { INT, FLOAT, DATASET };
  void add_parameter(std::string param_name, std::string param_description,
                     PARAM_TYPES t) {
    parameter_description[param_name] = std::make_pair(t, param_description);
  };

  std::string &get_unit_name() { return name; }
  std::string &get_unit_description() { return description; }
  auto &get_parameters() { return parameters; }
  auto &get_parameters_descriptions() { return parameter_description; }

protected:
  App *app;
  std::map<std::string, parameter_list> parameters; // performance
  std::map<std::string, std::pair<PARAM_TYPES, std::string>>
      parameter_description;
  std::string name;
  std::string description;
};

template <typename T> class ComputeUnitTyped : public ComputeUnit {
public:
  ComputeUnitTyped(App *app, std::string name, std::string descritpion)
      : ComputeUnit(app, name, descritpion){};
  void result_vector(std::vector<T> *res) { result = res; };

protected:
  std::vector<T> *result;
};

#endif // COMPUTEUNIT_H
