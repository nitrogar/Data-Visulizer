#include "compute_units/Average.hpp"
#include "compute_units/ComputeUnit.hpp"

Average::Average(App *app)
    : ComputeUnitTyped<float>(app, "Average", "Calculate Column Average") {
  add_parameter("in dataset", "The H5 dataset to calculate average for " , DATASET);
  add_parameter("out dataset", "The H5 dataset to store the averages values", DATASET);
}
void Average::compute(){
  std::cout << "inside compute average" << std::endl;
}





