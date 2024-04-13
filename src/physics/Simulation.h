//
// Created by 91246 on 2024/4/11.
//

#ifndef PLANET3D_SIMULATION_H
#define PLANET3D_SIMULATION_H

#include "PhysicsObject.h"
#include "GravityObject.h"
#include "Planet.h"
#include <mutex>
#include <vector>
#include <map>
#include "PhysicsCtrl.h"

enum SimulationCommand{
    PAUSE,
    STEP,
    RUN
};
class Simulation {

public:
    enum State{
        PAUSED,
        RUNNING
    };
    void parseCommand(SimulationCommand command);
    State getState();
    double getGap();
    const std::mutex& getPublicMutex();

    void runOneSimulation();

    void addAnObject(PhysicsObject* newObject);
    void removeAnObject(PhysicsObject* newObject);

    const PhysicsCtrl& getPhyCtrl();

    std::vector<PhysicsObject*> getAllPhysicsObj();


private:
    std::map<PhysicsObject*,std::pair<std::map<PhysicsObject*,std::tuple<Eigen::Vector3d ,Eigen::Vector3d>>,std::tuple<Eigen::Vector3d,Eigen::Vector3d>>> m_all_date;

    double m_simulateTimeGap;
    State m_currentState=PAUSED;
    std::mutex m_publicMutex;
    bool m_stepOnce=false;

    PhysicsCtrl m_phCtrl;

    void removeAnObjectWithOutLock(PhysicsObject *newObject);
};

#endif //PLANET3D_SIMULATION_H
