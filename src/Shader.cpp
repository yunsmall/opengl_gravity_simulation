//
// Created by 91246 on 2024/4/7.
//

#include "Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <spdlog/spdlog.h>
#include <glm/gtc/type_ptr.hpp>

using namespace std;


Shader::Shader(const std::string vertexPath,const std::string fragmentPath) {
    ifstream vertexFile,fragmentFile;
    string vertexCode,fragmentCode;
    vertexFile.exceptions(ios::badbit|ios::failbit );
    fragmentFile.exceptions(ios::badbit|ios::failbit );
    try{
        vertexFile.open(vertexPath);
        fragmentFile.open(fragmentPath);

        spdlog::info("成功打开顶点着色器文件{}，片段着色器文件{}",vertexPath,fragmentPath);

        stringstream vertexCodeStream,fragmentCodeStream;

        vertexCodeStream<<vertexFile.rdbuf();
        fragmentCodeStream<<fragmentFile.rdbuf();

        vertexCode=vertexCodeStream.str();
        fragmentCode=fragmentCodeStream.str();
    }
    catch (ios::failure& failure){
        SPDLOG_ERROR("文件读取错误，{}",failure.what());
    }
    const char* p_vertexCodeStr=vertexCode.c_str();
    const char* p_fragmentCodeStr=fragmentCode.c_str();

    unsigned int vertexShader=glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1,&p_vertexCodeStr,nullptr);
    glCompileShader(vertexShader);
    CheckError(vertexShader,CheckType::VERTEX);

    unsigned int fragmentShader=glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader,1,&p_fragmentCodeStr,nullptr);
    glCompileShader(fragmentShader);
    CheckError(fragmentShader,CheckType::FRAGMENT);

    ID=glCreateProgram();
    glAttachShader(ID,vertexShader);
    glAttachShader(ID,fragmentShader);
    glLinkProgram(ID);
    CheckError(ID,CheckType::PROGRAM);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::use() {
    glUseProgram(ID);
}

void Shader::setBool(std::string name, bool value) {
    glUniform1i(glGetUniformLocation(ID,name.c_str()),(int)value);
}

void Shader::setUInt(std::string name, unsigned int value) {
    glUniform1i(glGetUniformLocation(ID,name.c_str()),value);
}

void Shader::setFloat(std::string name, float value) {
    glUniform1f(glGetUniformLocation(ID,name.c_str()),value);
}

void Shader::setVecf2(std::string name, float v1, float v2) {
    glUniform2f(glGetUniformLocation(ID,name.c_str()),v1,v2);
}

void Shader::setVecf3(std::string name, float v1, float v2, float v3) {
    glUniform3f(glGetUniformLocation(ID,name.c_str()),v1,v2,v3);
}
void Shader::setVecf4(std::string name, float v1, float v2, float v3, float v4) {
    glUniform4f(glGetUniformLocation(ID,name.c_str()),v1,v2,v3,v4);
}

void Shader::CheckError(unsigned int id,CheckType type){
    int success;
    char info[1024];
    if(type!=CheckType::PROGRAM){
        glGetShaderiv(id,GL_COMPILE_STATUS,&success);
        if(!success){
            glGetShaderInfoLog(id, 1024, NULL, info);
            SPDLOG_ERROR("着色器编译错误，编号：{}，信息：\n{}",ID,info);
        }
    }
    else{
        glGetProgramiv(id,GL_LINK_STATUS,&success);
        if(!success){
            glGetProgramInfoLog(id, 1024, NULL, info);
            SPDLOG_ERROR("程序连接错误，编号：{}，信息：\n{}",ID,info);
        }
    }
}

unsigned int Shader::getValueID(std::string name) {
    return glGetUniformLocation(ID,name.c_str());
}

void Shader::setMat2(std::string name, glm::mat2& v) {
    glUniformMatrix2fv(glGetUniformLocation(ID,name.c_str()),1,GL_FALSE,glm::value_ptr(v));
}

void Shader::setMat3(std::string name, glm::mat3& v) {
    glUniformMatrix3fv(glGetUniformLocation(ID,name.c_str()),1,GL_FALSE,glm::value_ptr(v));
}

void Shader::setMat4(std::string name, glm::mat4& v) {
    glUniformMatrix4fv(glGetUniformLocation(ID,name.c_str()),1,GL_FALSE,glm::value_ptr(v));
}

void Shader::setVec2(std::string name, glm::vec2& v) {
    glUniform2fv(glGetUniformLocation(ID,name.c_str()),1,glm::value_ptr(v));
}

void Shader::setVec3(std::string name, glm::vec3& v) {
    glUniform3fv(glGetUniformLocation(ID,name.c_str()),1,glm::value_ptr(v));
}

void Shader::setVec4(std::string name, glm::vec4& v) {
    glUniform4fv(glGetUniformLocation(ID,name.c_str()),1,glm::value_ptr(v));
}

