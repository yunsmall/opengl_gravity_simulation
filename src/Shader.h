//
// Created by 91246 on 2024/4/7.
//

#ifndef OPENGL_TEST_SHADER_H
#define OPENGL_TEST_SHADER_H
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

class Shader {
public:
    Shader(const std::string vertexPath,const std::string fragmentPath);
    Shader(const Shader&)=delete;
    ~Shader()=default;

    void use();

    unsigned int getValueID(std::string name);

    void setBool(std::string name, bool value);
    void setUInt(std::string name, unsigned int value);
    void setFloat(std::string name, float value);
    void setVecf2(std::string name, float v1, float v2);
    void setVecf3(std::string name, float v1, float v2, float v3);
    void setVecf4(std::string name, float v1, float v2, float v3, float v4);

    void setMat2(std::string name, glm::mat2& v);
    void setMat3(std::string name, glm::mat3& v);
    void setMat4(std::string name, glm::mat4& v);

    void setVec2(std::string name, glm::vec2& v);
    void setVec3(std::string name, glm::vec3& v);
    void setVec4(std::string name, glm::vec4& v);

private:
    enum CheckType{
        VERTEX,
        FRAGMENT,
        PROGRAM
    };

    unsigned int ID;
    void CheckError(unsigned int id,CheckType type);
};


#endif //OPENGL_TEST_SHADER_H
