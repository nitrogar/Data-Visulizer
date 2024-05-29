#ifndef AVERAGE_H // If AVERAGE_H is not defined
#define AVERAGE_H // Define AVERAGE_H

#include "ComputeUnit.hpp"
#include "App.hpp"
#include <string>

class Average : public ComputeUnitTyped<float> {
public:
  Average(App * app);
  void compute();
};

#endif // AVERAGE_H
