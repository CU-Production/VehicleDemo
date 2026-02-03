#pragma once

#include "threepp/threepp.hpp"
#include <vector>

enum class VehicleType {
    Kart,
    Sedan,
    Truck,
    Tank,
    Motorcycle
};

struct VehicleModel {
    std::shared_ptr<threepp::Group> group;
    std::vector<std::shared_ptr<threepp::Mesh>> wheels;
    std::vector<std::shared_ptr<threepp::Mesh>> steeringWheels;
};

class VehicleFactory {
public:
    static VehicleModel create(VehicleType type);
    static VehicleModel createKart();
    static VehicleModel createSedan();
    static VehicleModel createTruck();
    static VehicleModel createTank();
    static VehicleModel createMotorcycle();
};
