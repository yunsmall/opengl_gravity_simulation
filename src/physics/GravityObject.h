//
// Created by 91246 on 2024/4/11.
//

#ifndef PLANET3D_GRAVITYOBJECT_H
#define PLANET3D_GRAVITYOBJECT_H

#include "PhysicsObject.h"

class GravityObject: public PhysicsObject {
    friend class Simulation;
public:
    explicit GravityObject(Eigen::Vector3d position,double mass,double charge);
    GravityObject(Eigen::Vector3d position,double mass,double charge,Eigen::Vector3d last_postion);
    GravityObject(Eigen::Vector3d position,double mass,double charge,PhysicsCtrl physicsCtrl,Eigen::Vector3d velocity=Eigen::Vector3d(0,0,0));
    GravityObject(const GravityObject&)=delete;
    virtual ~GravityObject()=default;
protected:
    double mass;
    double charge;

    bool forceMotionless=false;

};


#endif //PLANET3D_GRAVITYOBJECT_H
