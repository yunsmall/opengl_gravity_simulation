//
// Created by 91246 on 2024/4/11.
//

#include "Simulation.h"
#include <iostream>

using namespace std;

Simulation::State Simulation::getState() {
    return m_currentState;
}

double Simulation::getGap() {
    return m_simulateTimeGap;
}

const std::mutex &Simulation::getPublicMutex() {
    return m_publicMutex;
}

void Simulation::parseCommand(SimulationCommand command) {
    lock_guard lockGuard(m_publicMutex);
    if(command==SimulationCommand::PAUSE){
        if(m_currentState==State::RUNNING){
            m_currentState=State::PAUSED;
        }
    }
    else if(command==SimulationCommand::RUN){
        if(m_currentState==State::PAUSED){
            m_currentState=State::RUNNING;
        }
    }
    else if(command==SimulationCommand::STEP){
        if(m_currentState==State::PAUSED){
            m_stepOnce=true;
        }
    }
}

void Simulation::runOneSimulation() {
    lock_guard lockGuard(m_publicMutex);

    if(m_currentState==State::RUNNING||(m_currentState==State::PAUSED &&
                                                     m_stepOnce)) {

        //重置大小
        if (m_all_date.size() != 0) {
            if (std::get<0>(m_all_date.begin()->second).size() != m_all_date.size()) {
                for (auto &i: m_all_date) {
                    i.second.first.clear();//重置
                    for (auto &j: m_all_date) {//然后重新插入
                        i.second.first.insert(pair(j.first, tuple<Eigen::Vector3d, Eigen::Vector3d>()));
                    }
                }
            }
        } else {
            return;
        }

        //设置为最小值
        for (auto &i: m_all_date) {
            for (auto &j: get<0>(i.second)) {
                //清除万有引力
                get<0>(j.second) = Eigen::Vector3d (DBL_MIN,DBL_MIN,DBL_MIN);
                //清除电磁力
                get<1>(j.second) = Eigen::Vector3d (DBL_MIN,DBL_MIN,DBL_MIN);
            }
        }

//    int count_i=0;
        for (auto &i: m_all_date) {

            Eigen::Vector3d g_force(0, 0,0), e_force(0, 0,0);
            for (auto &j: m_all_date) {
                if (j.first != i.first) {
                    auto parallelism_mp = *i.second.first.find(j.first);

                    //是否要计算万有引力和电磁力
                    bool need_calc_gravity= typeid(i.first)== typeid(GravityObject)&&typeid(j.first)== typeid(GravityObject);
                    bool need_calc_electrocity=need_calc_gravity;

                    if (get<0>(parallelism_mp.second) == Eigen::Vector3d (DBL_MIN,DBL_MIN,DBL_MIN) &&
                        get<1>(parallelism_mp.second) == Eigen::Vector3d (DBL_MIN,DBL_MIN,DBL_MIN)) {//没计算过就算一遍
                        auto parallelism_mp_map = *m_all_date.find(parallelism_mp.first)->second.first.find(
                                i.first);
                        if (get<0>(parallelism_mp_map.second) == Eigen::Vector3d (DBL_MIN,DBL_MIN,DBL_MIN) &&
                            get<1>(parallelism_mp_map.second) == Eigen::Vector3d (DBL_MIN,DBL_MIN,DBL_MIN)) {//如果反着查找也没算过，这真正开始计算
                            auto dr = j.first->m_position - i.first->m_position;//计算坐标差
                            auto r_pow = dr.squaredNorm();//坐标差平方
                            auto r_abs = dr.norm();//坐标差长度
                            //计算碰撞


                            //先计算万有引力
                            double gmm_divide_r2;
                            if(need_calc_gravity){
                                auto i_as_gravity_obj=(GravityObject*)(&i.first);
                                auto j_as_gravity_obj=(GravityObject*)(&i.first);
                                gmm_divide_r2= m_phCtrl.G * i_as_gravity_obj->mass * j_as_gravity_obj->mass / r_pow;//GMm/r^2
                            }

                            double same_g_coefficient = gmm_divide_r2 / r_abs;//GMm/r^3
                            //再计算电磁力
                            double kqq_divide_r2;
                            if(need_calc_electrocity){
                                auto i_as_gravity_obj=(GravityObject*)(&i.first);
                                auto j_as_gravity_obj=(GravityObject*)(&i.first);
                                kqq_divide_r2= m_phCtrl.k * i_as_gravity_obj->charge * j_as_gravity_obj->charge / r_pow;//kQq/r^2
                            }
                            double same_k_coefficient = -kqq_divide_r2 / r_abs;//-kQq/r^3

                            get<0>(parallelism_mp.second) = same_g_coefficient * dr;

                            get<1>(parallelism_mp.second) = same_k_coefficient * dr;

                            get<0>(parallelism_mp_map.second) = -same_g_coefficient * dr;

                            get<1>(parallelism_mp_map.second) = -same_k_coefficient * dr;
                        } else {//如果发现算过了
                            get<0>(parallelism_mp.second) = -get<0>(parallelism_mp_map.second);

                            get<1>(parallelism_mp.second) = -get<1>(parallelism_mp_map.second);
                        }

                    }
                    g_force += get<0>(parallelism_mp.second);

                    e_force += get<1>(parallelism_mp.second);
                }
            }

            get<0>(i.second.second) = g_force;

            get<1>(i.second.second) = e_force;
        }
        //计算新的位置偏移
        for (auto &i: m_all_date) {
            //重力物体位置计算
            GravityObject* i_as_gravity_obj=dynamic_cast<GravityObject*>(i.first);
            if(i_as_gravity_obj){
                if(!i_as_gravity_obj->forceMotionless){
                    Eigen::Vector3d all_force(0, 0,0);
                    all_force = get<0>(i.second.second) + get<1>(i.second.second);

                    Eigen::Vector3d a(all_force/ i_as_gravity_obj->mass);
//                    Eigen::Vector3d a(0,0,0);
//                    cout<<"加速度"<<a<<endl;

                    double t2 = m_phCtrl.simulate_gap * m_phCtrl.simulate_gap;

                    Eigen::Vector3d new_position(a * t2 + 2 * i.first->m_position - i.first->m_last_position);

                    i_as_gravity_obj->m_last_position=i_as_gravity_obj->m_position;

                    i_as_gravity_obj->m_position=new_position;
                }

            }
            //其他位置计算
        }


        if(m_stepOnce){
            m_stepOnce=false;
        }
    }
}

void Simulation::addAnObject(PhysicsObject *newObject) {
    lock_guard lockGuard(m_publicMutex);
//    newObject->simulation=this;
    m_all_date.insert(pair<PhysicsObject*,pair<map<PhysicsObject*,tuple<Eigen::Vector3d,Eigen::Vector3d>>,tuple<Eigen::Vector3d,Eigen::Vector3d>>>(newObject,pair<map<PhysicsObject*,tuple<Eigen::Vector3d,Eigen::Vector3d>>,tuple<Eigen::Vector3d,Eigen::Vector3d>>()));
}

void Simulation::removeAnObject(PhysicsObject *newObject) {
    lock_guard lockGuard(m_publicMutex);
    removeAnObjectWithOutLock(newObject);
}

const PhysicsCtrl &Simulation::getPhyCtrl() {
    return m_phCtrl;
}

std::vector<PhysicsObject*> Simulation::getAllPhysicsObj() {
    lock_guard lockGuard(m_publicMutex);
    std::vector<PhysicsObject*> all_physics_obj;
    for(auto& i:m_all_date){
        all_physics_obj.push_back(i.first);
    }
    return all_physics_obj;
}

void Simulation::removeAnObjectWithOutLock(PhysicsObject *newObject) {
    newObject->m_isDestoried=true;
    m_all_date.erase(newObject);
}
