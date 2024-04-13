//
// Created by 91246 on 2024/4/11.
//

#ifndef PLANET3D_PLANETS_H
#define PLANET3D_PLANETS_H
#include "GravityObject.h"

class Planet:public GravityObject {
public:
    explicit Planet(Eigen::Vector3d position,double mass,double charge);
    Planet(Eigen::Vector3d position,double mass,double charge,Eigen::Vector3d last_postion);
    Planet(Eigen::Vector3d position,double mass,double charge,PhysicsCtrl physicsCtrl,Eigen::Vector3d velocity=Eigen::Vector3d(0,0,0));

    virtual ~Planet()=default;
protected:

};


#endif //PLANET3D_PLANETS_H
