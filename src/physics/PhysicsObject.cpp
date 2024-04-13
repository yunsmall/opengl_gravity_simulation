//
// Created by 91246 on 2024/4/11.
//

#include "PhysicsObject.h"

PhysicsObject::PhysicsObject(Eigen::Vector3d position) {
    m_position=position;
    m_last_position=position;
}

PhysicsObject::PhysicsObject(Eigen::Vector3d position, Eigen::Vector3d last_postion) {
    m_position=position;
    m_last_position=last_postion;
}

PhysicsObject::PhysicsObject(Eigen::Vector3d position,PhysicsCtrl &physicsCtrl, Eigen::Vector3d velocity) {
    m_position=position;
    m_last_position=position-velocity*physicsCtrl.simulate_gap;
}

PhysicsObject *PhysicsObject::copySelf() {
    return new PhysicsObject(m_position,m_last_position);
}

const Eigen::Vector3d &PhysicsObject::getMPosition() const {
    return m_position;
}

const Eigen::Vector3d &PhysicsObject::getMLastPosition() const {
    return m_last_position;
}

bool PhysicsObject::isMIsDestoried() const {
    return m_isDestoried;
}
