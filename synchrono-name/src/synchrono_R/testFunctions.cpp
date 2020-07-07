#include "testFunctions.h"

#include <cmath>

#include "chrono_models/vehicle/sedan/Sedan.h"

using namespace chrono;
using namespace chrono::vehicle;
using namespace chrono::vehicle::sedan;

int linearTestFunction(int a, int b, int x) {
  return a * x + b;
}

int stlTestFunction(int a, int b) {
  return std::pow(a, b);
}

double chronoVehicleTest() {
  auto sedan = std::make_shared<Sedan>();

  sedan->Initialize();

  return sedan->GetTotalMass();
}