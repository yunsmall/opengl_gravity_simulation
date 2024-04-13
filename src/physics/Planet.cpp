//
// Created by 91246 on 2024/4/11.
//

#include "Planet.h"

Planet::Planet(Eigen::Vector3d position, double mass, double charge) : GravityObject(position, mass, charge) {

}

Planet::Planet(Eigen::Vector3d position, double mass, double charge, Eigen::Vector3d last_postion) : GravityObject(
        position, mass, charge, last_postion) {

}

Planet::Planet(Eigen::Vector3d position, double mass, double charge, PhysicsCtrl physicsCtrl, Eigen::Vector3d velocity)
        : GravityObject(position, mass, charge,
                        physicsCtrl,
                        velocity) {

}
