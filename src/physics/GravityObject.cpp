//
// Created by 91246 on 2024/4/11.
//

#include "GravityObject.h"

GravityObject::GravityObject(Eigen::Vector3d position,double mass,double charge, Eigen::Vector3d last_postion) : PhysicsObject(position,
                                                                                                     last_postion),mass(mass),charge(charge) {

}

GravityObject::GravityObject(Eigen::Vector3d position,double mass,double charge) : PhysicsObject(position),mass(mass),charge(charge) {

}

GravityObject::GravityObject(Eigen::Vector3d position,double mass,double charge, PhysicsCtrl physicsCtrl, Eigen::Vector3d velocity)
        : PhysicsObject(position, physicsCtrl, velocity),mass(mass),charge(charge) {
}
