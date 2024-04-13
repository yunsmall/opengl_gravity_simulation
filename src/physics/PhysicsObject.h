//
// Created by 91246 on 2024/4/11.
//

#ifndef PLANET3D_PHYSICSOBJECT_H
#define PLANET3D_PHYSICSOBJECT_H

#include <Eigen/Eigen>
#include "PhysicsCtrl.h"

class Simulation;
/**
 * @brief 只计入碰撞计算，也许可以移动，正在计算中不允许删除对象
 */
class PhysicsObject {
    friend class Simulation;
public:
    explicit PhysicsObject(Eigen::Vector3d position);
    PhysicsObject(Eigen::Vector3d position,Eigen::Vector3d last_postion);
    PhysicsObject(Eigen::Vector3d position,PhysicsCtrl& physicsCtrl,Eigen::Vector3d velocity=Eigen::Vector3d(0,0,0));
    PhysicsObject(const PhysicsObject&)=delete;
    virtual ~PhysicsObject()=default;

    PhysicsObject* copySelf();

    const Eigen::Vector3d &getMPosition() const;

    const Eigen::Vector3d &getMLastPosition() const;

    bool isMIsDestoried() const;

protected:
    Eigen::Vector3d m_position;
    Eigen::Vector3d m_last_position;

//    Simulation* simulation=nullptr;

    bool m_isDestoried=false;
};


#endif //PLANET3D_PHYSICSOBJECT_H
