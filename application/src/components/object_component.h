#pragma once
#include "../exlibs.h"
#include <string>

struct ObjectData
{
    std::string name;
    std::string obj_path;
    std::string texture_path;
};

struct objectInit
{
    std::string *obj_name;
    std::string *obj_path;
    std::string *texture_path;
    glm::vec3 *position;
    glm::vec3 *eulers;
    glm::vec3 *velocity;
    glm::vec3 *eulerVelocity;
    bool *onTexCoords;
    float *zoom_factor;
    float *scale;

    // Constructor with proper initialization
    objectInit(
        std::string *obj_name = nullptr,
        std::string *obj_path = nullptr,
        std::string *texture_path = nullptr,
        glm::vec3 *position = nullptr,
        glm::vec3 *eulers = nullptr,
        glm::vec3 *velocity = nullptr,
        glm::vec3 *eulerVelocity = nullptr,
        bool *onTexCoords = nullptr,
        float *zoom_factor = nullptr,
        float *scale = nullptr)
        : obj_name(obj_name),
          obj_path(obj_path),
          texture_path(texture_path),
          position(position ? position : new glm::vec3(0.0f, 0.0f, 0.0f)),
          eulers(eulers ? eulers : new glm::vec3(45.0f, 45.0f, 45.0f)),
          velocity(velocity ? velocity : new glm::vec3(0.0f, 0.0f, 0.0f)),
          eulerVelocity(eulerVelocity ? eulerVelocity : new glm::vec3(10.0f, 10.0f, 10.0f)),
          onTexCoords(onTexCoords ? onTexCoords : new bool(true)),
          zoom_factor(zoom_factor ? zoom_factor : new float(1.5f)),
          scale(scale ? scale : new float(160.0f))
    {
    }

    ~objectInit()
    {
        delete position;
        delete eulers;
        delete velocity;
        delete eulerVelocity;
        delete onTexCoords;
        delete zoom_factor;
        delete scale;
    }
};