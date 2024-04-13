#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <spdlog/spdlog.h>
#include "Shader.h"
#include "Camera.h"
#include "Simulation.h"
#include <vector>
#include <thread>
#include <atomic>
#include <Eigen/Dense>
#include <boost/lockfree/spsc_queue.hpp>

struct SystemSettings{
    Eigen::Vector3d set_v=Eigen::Vector3d(0,0,0);
    double set_mass=1e4,set_q=0;
    double set_radius=100;
    int tmp_set_simulate_time_gap=10000;
    bool place_mode=false;
    float camera_velocity=10.0;
};

struct RealTimeSettings{
    Eigen::Vector3d place_position=Eigen::Vector3d(0,0,0);
};

SystemSettings systemSettings;
SystemSettings systemSettings_copy;

RealTimeSettings realTimeSettings;


Simulation simulation;

std::atomic<bool> need_calc=true;

boost::lockfree::spsc_queue<std::vector<PhysicsObject*>,boost::lockfree::capacity<1>> physics_spscQueue;
boost::lockfree::spsc_queue<Simulation::State,boost::lockfree::capacity<1>> state_spscQueue;
boost::lockfree::spsc_queue<SimulationCommand,boost::lockfree::capacity<1>> simulate_command;

std::atomic<int> simulate_time_gap=10000;
std::chrono::microseconds REAL_GAP_TIME(10000);

glm::vec3 eigen_to_glm_vec3(Eigen::Vector3f& vector3F){
    return glm::vec3(vector3F.x(),vector3F.y(),vector3F.z());
}

Eigen::Vector3f glm_to_eigen3f(glm::vec3& vec3){
    return Eigen::Vector3f(vec3.x,vec3.y,vec3.z);
}
Eigen::Vector3d glm_to_eigen3d(glm::vec3& vec3){
    return Eigen::Vector3d(vec3.x,vec3.y,vec3.z);
}


void thread_func(){
    while(need_calc){
        auto start_time=std::chrono::high_resolution_clock::now();
        simulation.runOneSimulation();

        Simulation::State state=simulation.getState();
        if(state_spscQueue.empty()){
            state_spscQueue.push(state);
        }
        if(physics_spscQueue.empty()){
            physics_spscQueue.push(simulation.getAllPhysicsObj());
        }
        else{//主线程没取走数据，代表主线程慢的一批，所以强制睡眠一段时间

        }
//        std::cout<<"计算花了"<<std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()-start_time).count()<<"微秒"<<std::endl;

        while(std::chrono::high_resolution_clock::now()-start_time<REAL_GAP_TIME);
        if(REAL_GAP_TIME.count()!=simulate_time_gap){
            REAL_GAP_TIME=std::chrono::microseconds(simulate_time_gap);
        }

//        this_thread::sleep_for(chrono::microseconds(100));

        if(state==Simulation::State::PAUSED){
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        if(!simulate_command.empty()){
            SimulationCommand send_command;
            simulate_command.pop(send_command);
            simulation.parseCommand(send_command);
        }

    }

}


int main() {
    glfwSetErrorCallback([](int error, const char* description){
        spdlog::error("GLFW Error {}: {}",error,description);
    });
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    static int window_width=800,window_height=600;

    GLFWwindow* window= glfwCreateWindow(window_width,window_height,"opengl imgui测试",nullptr,nullptr);
    if(!window){
        spdlog::error("glfw窗口创建失败");
        glfwTerminate();
        return -1;
    }
    GLFWmonitor* monitor=glfwGetPrimaryMonitor();

    glfwMakeContextCurrent(window);
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)){
        spdlog::error("获取glad函数地址错误");
        glfwTerminate();
        return -1;
    }
    glEnable(GL_DEPTH_TEST);


    glViewport(0,0,800,600);
    glfwSetFramebufferSizeCallback(window,[](GLFWwindow* window, int width, int height){
        glViewport(0,0,width,height);
        window_width=width;
        window_height=height;

    });
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    ImGui::CreateContext();
    static ImGuiIO& io=ImGui::GetIO();
    ImGui::StyleColorsDark();
    io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msyh.ttc", 18.0f,nullptr,
                                 io.Fonts->GetGlyphRangesChineseFull());
    ImGui_ImplGlfw_InitForOpenGL(window,true);
    ImGui_ImplOpenGL3_Init("#version 330");

    static std::vector<Planet*> all_planet;

    for(int i=-2;i<=2;i++){
        for(int j=-2;j<=2;j++){
            for(int k=-3;k<=3;k++){
                Planet* new_planet=new Planet(Eigen::Vector3d(10*i,10*j,10*k),10000,0);
                all_planet.push_back(new_planet);
                simulation.addAnObject((PhysicsObject*)new_planet);
            }
        }
    }

    static Simulation::State stateCpoy=simulation.getState();
    std::thread simulation_thread(thread_func);
    simulate_command.push(SimulationCommand::RUN);

    unsigned int VAO;
    glGenVertexArrays(1,&VAO);
    unsigned int VBO;
    glGenBuffers(1,&VBO);
//
    float vertices[] = {
            // positions          // normals           // texture coords
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    Shader shader("../assets/shaders/trans_3d_without_texture.vert","../assets/shaders/blue.frag");
    Shader place_shader("../assets/shaders/trans_3d_without_texture.vert","../assets/shaders/orange.frag");

    static Camera camera(glm::vec3(0,0,0));
    static bool mouseNoMovement=true;

    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos){
//        spdlog::info("当前鼠标位置{},{}",xpos,ypos);
        static float lastX = 400, lastY = 300;
        if(stateCpoy==Simulation::State::RUNNING){

            if(mouseNoMovement)
            {
                lastX = xpos;
                lastY = ypos;
                mouseNoMovement = false;
            }
            float x_offset=xpos-lastX,y_offset=lastY-ypos;
            lastX = xpos;
            lastY = ypos;

            camera.ProcessMouseMovement(x_offset,y_offset);
//            spdlog::info("摄像头朝向：{}",glm::to_string(camera_front));
        }
//        if(io.WantCaptureMouse){
            ImGui_ImplGlfw_CursorPosCallback(window,xpos,ypos);
//        }
    });
//
    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset){
        if(!systemSettings.place_mode){
            camera.ProcessMouseScroll(yoffset);
        }
        else{
            realTimeSettings.place_position+=Eigen::Vector3d(0,0,-yoffset);
        }
        ImGui_ImplGlfw_ScrollCallback(window,xoffset,yoffset);
    });
//
    glfwSetKeyCallback(window,[](GLFWwindow* window, int key, int scancode, int action, int mods){
        if(key==GLFW_KEY_ESCAPE){
            if(action==GLFW_PRESS){
                if(stateCpoy==Simulation::State::RUNNING){
                    mouseNoMovement=true;
                    simulate_command.push(SimulationCommand::PAUSE);
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }
                else{
                    mouseNoMovement=true;
                    simulate_command.push(SimulationCommand::RUN);
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                }
            }
        }
        ImGui_ImplGlfw_KeyCallback(window,key,scancode,action,mods);
    });

    glfwSetMouseButtonCallback(window,[](GLFWwindow* window, int button, int action, int mods){
        if(!stateCpoy==Simulation::State::PAUSED){
            if(systemSettings.place_mode == true){
                if(button==GLFW_MOUSE_BUTTON_LEFT){
                    if(action==GLFW_PRESS){
                        glm::vec3 final_position=glm::mat3(camera.right,camera.up,-camera.front) * glm::vec3(realTimeSettings.place_position.x(), realTimeSettings.place_position.y(), realTimeSettings.place_position.z()) + camera.position;
                        Planet* planet=new Planet(glm_to_eigen3d(final_position), systemSettings.set_mass, systemSettings.set_q, simulation.getPhyCtrl(), systemSettings.set_v);
                        simulation.addAnObject(planet);
                        all_planet.push_back(planet);
                    }
                }
            }
        }


        ImGui_ImplGlfw_MouseButtonCallback(window,button,action,mods);
    });

//
    double lastFrame = glfwGetTime(),currentFrame;

    std::vector<PhysicsObject*> last_physics_objs;

    while(!glfwWindowShouldClose(window)){
        currentFrame = glfwGetTime();
        static float delta_time;
        delta_time=currentFrame-lastFrame;
        lastFrame=currentFrame;

        if(!state_spscQueue.empty()){
            state_spscQueue.pop(stateCpoy);
        }
        glfwPollEvents();

        bool is_running=stateCpoy==Simulation::State::RUNNING;
//
        if(is_running){
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                camera.ProcessKeyboard(Camera_Movement::LEFT,delta_time);
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                camera.ProcessKeyboard(Camera_Movement::RIGHT,delta_time);
            }
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                camera.ProcessKeyboard(Camera_Movement::FORWARD,delta_time);
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                camera.ProcessKeyboard(Camera_Movement::BACKWARD,delta_time);
            }
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                camera.ProcessKeyboard(Camera_Movement::UP,delta_time);
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
                camera.ProcessKeyboard(Camera_Movement::DOWN,delta_time);
            }

            if(glfwGetKey(window,GLFW_KEY_CAPS_LOCK)==GLFW_PRESS){
                camera.speed=systemSettings.camera_velocity*2;
            }
            else{
                camera.speed=systemSettings.camera_velocity/2;
            }
        }


        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if(stateCpoy==Simulation::State::PAUSED){
            ImGui::Begin((char*)u8"测试中文");
            ImGui::InputDouble((char*)u8"x轴速度",&systemSettings_copy.set_v.x());
            ImGui::InputDouble((char*)u8"y轴速度",&systemSettings_copy.set_v.y());
            ImGui::InputDouble((char*)u8"z轴速度",&systemSettings_copy.set_v.z());
            ImGui::InputDouble((char*)u8"质量",&systemSettings_copy.set_mass);
            ImGui::InputDouble((char*)u8"电荷",&systemSettings_copy.set_q);
            ImGui::InputDouble((char*)u8"半径",&systemSettings_copy.set_radius);
            ImGui::InputInt((char*)u8"毫秒数",&systemSettings_copy.tmp_set_simulate_time_gap);
            ImGui::InputFloat((char*)u8"移动速度",&systemSettings_copy.camera_velocity);
            ImGui::Checkbox((char*)u8"放置模式",&systemSettings_copy.place_mode);

            ImGui::InputDouble((char*)u8"放置x轴偏移",&realTimeSettings.place_position.x());
            ImGui::InputDouble((char*)u8"放置y轴偏移",&realTimeSettings.place_position.y());
            ImGui::InputDouble((char*)u8"放置z轴偏移",&realTimeSettings.place_position.z());

            if(ImGui::Button("确定")){
                systemSettings=systemSettings_copy;

                simulate_time_gap=systemSettings.tmp_set_simulate_time_gap;
            }

            if(ImGui::Button("清除")){
                for(auto&i:all_planet){
                    simulation.removeAnObject(i);
                }
                all_planet.clear();
            }

            if(ImGui::Button("退出")){
                glfwSetWindowShouldClose(window,true);
            }

            ImGui::End();
        }


        shader.use();
        shader.setVec3("viewPos", camera.position);

        glm::mat4 view=camera.GetViewMatrix();

        glm::mat4 projection=glm::mat4(1.0f);
        const GLFWvidmode* glfWvidmode=glfwGetVideoMode(monitor);

        projection=glm::perspective(glm::radians(camera.fov), (float)window_width/window_height,0.1f,1000.0f);
//
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        shader.use();
        glUniformMatrix4fv(shader.getValueID("view"),1,GL_FALSE,&view[0][0]);
        glUniformMatrix4fv(shader.getValueID("projection"),1,GL_FALSE,&projection[0][0]);

        place_shader.use();
        glUniformMatrix4fv(place_shader.getValueID("view"),1,GL_FALSE,&view[0][0]);
        glUniformMatrix4fv(place_shader.getValueID("projection"),1,GL_FALSE,&projection[0][0]);

        if(!physics_spscQueue.empty()){
            last_physics_objs.clear();
            physics_spscQueue.pop(last_physics_objs);
        }

        shader.use();
        glBindVertexArray(VAO);
        for(auto& j:last_physics_objs){
            auto position=j->getMPosition();
//            std::cout<<position<<std::endl;
            glm::mat4 model(1.0f);
            model=glm::translate(model,glm::vec3(position.x(),position.y(),position.z()));
            shader.setMat4("model",model);


            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        if(systemSettings.place_mode){

            place_shader.use();
            glm::vec3 offset(realTimeSettings.place_position.x(),realTimeSettings.place_position.y(),realTimeSettings.place_position.z());
//            glm::vec3 offset(sin(M_PI*2/2*glfwGetTime()),0,cos(M_PI*2/2*glfwGetTime()));

            glm::mat4 model(1.0f);
            model=glm::translate(model,glm::mat3(camera.right,camera.up,-camera.front)*offset+camera.position);
            place_shader.setMat4("model",model);

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
//
    need_calc=false;
    simulation_thread.join();
    for(auto i:all_planet){
        simulation.removeAnObject((PhysicsObject*)i);
        delete i;
    }
    all_planet.clear();
    return 0;
}
