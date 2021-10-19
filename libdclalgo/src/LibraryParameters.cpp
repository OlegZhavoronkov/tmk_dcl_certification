//------------------------------------------------------------------------------
//  Created     : 04.07.2021
//  Author      : Golovchanskaya Julia
//  Description : This is class for parameters of devices. You need to start
//                working with method configure(...).
//  Example     : LibraryParameters::configure(
//                LibraryParameters::PolarizationSensorOrder::order90_45_135_0);
//------------------------------------------------------------------------------
#include "LibraryParameters.h"

#include <glog/logging.h>

#include <stdexcept>

std::optional<LibraryParameters::PolarizationSensorOrder>
    LibraryParameters::polarizationOrder = std::nullopt;

void LibraryParameters::configure(
    const PolarizationSensorOrder &polarizationOrder_) {
  polarizationOrder = polarizationOrder_;
}

std::array<double, LibraryParameters::polarizationMatrixSize>
LibraryParameters::getPolarMatrix() {
  if (!polarizationOrder.has_value()) {
    throw std::runtime_error(
        "LibraryParameters: Library parameters were not configured.");
  }

  switch (polarizationOrder.value()) {
  case PolarizationSensorOrder::order90_45_135_0: {
    return polarizationMatrix0;
  }
  default: {
    std::array<double, LibraryParameters::polarizationMatrixSize> zeros;
    zeros.fill(0.0);
    return zeros;
  }
  }
}