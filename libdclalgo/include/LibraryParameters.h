//------------------------------------------------------------------------------
//  Created     : 04.07.2021
//  Author      : Golovchanskaya Julia
//  Description : This is class for parameters of devices. You need to start
//                working with method configure(...).
//  Example     : LibraryParameters::configure(
//                LibraryParameters::PolarizationSensorOrder::order90_45_135_0);
//------------------------------------------------------------------------------
#ifndef LIBRARYPARAMETERS_H
#define LIBRARYPARAMETERS_H

#include <array>
#include <optional>

class LibraryParameters {
public:
  LibraryParameters() = default;
  ~LibraryParameters() = default;

  enum class PolarizationSensorOrder {
    //  __________
    //  | 90 | 45 |
    //  __________
    //  |135 |  0 |
    //  __________
    // we haven’t come across any other configuration yet.
    order90_45_135_0 = 0 // Sony sensor
  };

  static void configure(const PolarizationSensorOrder &polarizationOrder_);
  static std::array<double, 12> getPolarMatrix();

private:
  static constexpr int polarizationMatrixSize = 12;
  static constexpr std::array<double, polarizationMatrixSize>
      polarizationMatrix0 = {0.2500,  0.2500, 0.2500, 0.2500,
                             0.5000, 0.0000, -0.5000, 0.0000,
                             0.0000, 0.5000, 0.0000, -0.5000};

private:
  static std::optional<PolarizationSensorOrder> polarizationOrder;
};
#endif // LIBRARYPARAMETERS_H
