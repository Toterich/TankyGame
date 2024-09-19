#include "raylib.hpp"

struct CarSettings {
    float accel;
    float decel;
    float drag;
    float steeringFw;
    float steeringBw;
};

struct ControlState {
    bool left : 1;
    bool right : 1;
    bool fw : 1;
    bool bw : 1;
};

struct Actor {
    CarSettings car;

//    BoundingRect bBox;
    rl::Rectangle bBox;

    float steerAngle;
    float vel;
};

constexpr int MAXCOINS = 5;

struct Coin {
    bool collected = false;

    rl::Vector2 center;

    float radius = 10;
};

struct SimContext {
    float trackTime;
    Actor player;
    Coin coins[MAXCOINS];
};
