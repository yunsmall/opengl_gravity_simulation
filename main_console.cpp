#include <iostream>
#include "Simulation.h"
#include <vector>

using namespace std;

int main(){
    Simulation simulation;
    vector<Planet*> all_planet;
    for(int i=0;i<2;i++){
        Planet* new_planet=new Planet(Eigen::Vector3d(0,10*i,0),100,0);
        all_planet.push_back(new_planet);
        simulation.addAnObject((PhysicsObject*)new_planet);
    }

    simulation.parseCommand(SimulationCommand::RUN);
    for(int i=0;i<3;i++){
        simulation.runOneSimulation();
        auto date=simulation.getAllPhysicsObj();
        for(auto& j:date){
            cout<<j->getMPosition()<<endl;
        }
    }

    for(auto i:all_planet){
        simulation.removeAnObject((PhysicsObject*)i);
        delete i;
    }
    all_planet.clear();



    return 0;
}