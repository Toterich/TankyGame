#include <vector>

#include "types.hpp"

struct Replay {
    Actor car; // includes initial position
    float finalTime;

    // Frame-by-frame history of the control parameters for the car
    std::vector<ControlState> controlHistory;
};
