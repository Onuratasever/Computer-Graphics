#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "geometry.h"  // AABB

enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT };

struct Camera 
{
    // settings
    float Yaw   = -90.0f;
    float Pitch =   0.0f;
    float Speed =   2.5f;
    float Sensitivity = 0.1f;
    float Zoom = 45.0f;
    float CollisionRadius = 0.2f; // it is used to determine the distance at which a collision is detected

    // state
    glm::vec3 Position;
    glm::vec3 Front { 0.0f, 0.0f, -1.0f };
    glm::vec3 Up, Right, WorldUp;

    // collision
    const std::vector<AABB*>* Colliders = nullptr;
    const std::vector<Object*>* DynamicObjects = nullptr;

    Camera(glm::vec3 startPos = {0.0f,0.0f,3.0f}, glm::vec3 up = {0.0f,1.0f,0.0f})
        : Position(startPos), WorldUp(up) { updateVectors(); }

    glm::mat4 GetViewMatrix() const 
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void ProcessKeyboard(Camera_Movement dir, float dt) 
    {
        float vel = Speed * dt;
        glm::vec3 offset;

        if(dir == FORWARD)  offset = Front * vel;
        if(dir == BACKWARD) offset = -Front * vel;
        if(dir == LEFT)     offset = -Right * vel;
        if(dir == RIGHT)    offset = Right * vel;

        glm::vec3 newPos = Position + offset;

        if (!Colliders || !isColliding(newPos)) 
        {
            Position = newPos;
        }
    }

    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch=true)
    {
        xoffset *= Sensitivity;
        yoffset *= Sensitivity;
        Yaw   += xoffset;
        Pitch += yoffset;
        if(constrainPitch)
        {
            if(Pitch>89.0f) Pitch=89.0f;
            if(Pitch<-89.0f) Pitch=-89.0f;
        }
        updateVectors();
    }

    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= yoffset;
        if(Zoom<1.0f)  Zoom=1.0f;
        if(Zoom>45.0f) Zoom=45.0f;
    }

    void SetColliders(const std::vector<AABB*>& staticColliders, 
                      const std::vector<Object*>& dynamicObjects = {})
    {
        Colliders = &staticColliders;
        DynamicObjects = &dynamicObjects;
    }

private:
    void updateVectors()
    {
        glm::vec3 f;
        f.x = cos(glm::radians(Yaw))*cos(glm::radians(Pitch));
        f.y = sin(glm::radians(Pitch));
        f.z = sin(glm::radians(Yaw))*cos(glm::radians(Pitch));
        Front = glm::normalize(f);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up    = glm::normalize(glm::cross(Right, Front));
    }

    bool isColliding(const glm::vec3& point) 
    {
        // Check static colliders for collision
        for (const auto& box : *Colliders) 
        {
            glm::vec3 closestPoint = glm::clamp(point, box->min, box->max);
            float distance = glm::length(point - closestPoint);
            if (distance < CollisionRadius)
                return true;
        }

        // Control dynamic objects for collision
        if (DynamicObjects) 
        {
            for (auto& obj : *DynamicObjects)
            {
                glm::vec3 closestPoint = glm::clamp(point, obj->box.min, obj->box.max);
                float distance = glm::length(point - closestPoint);
                if (distance < CollisionRadius) 
                {
                    // Calculate the direction to push the object away
                    glm::vec3 rawDir = obj->box.center() - this->Position;
                    glm::vec3 pushDir;

                    if (glm::length(rawDir) < 0.001f) 
                    {
                        if (glm::length(obj->position - point) > 0.001f) 
                        {
                            pushDir = glm::normalize(obj->position - point);
                        } 
                        else 
                        {
                            // very rare case where the object is exactly at the camera position
                            pushDir = glm::vec3(0.0f, 0.0f, 1.0f);
                        }
                    } 
                    else 
                    {
                        pushDir = glm::normalize(rawDir);
                    }

                    obj->move(pushDir * 0.05f, *Colliders, *DynamicObjects);
                    // std::cout << "Collision detected with dynamic object, pushing away." << std::endl;
                    return true; // Collusion detected, return true
                }
            }
        }
        
        return false;
    }
};
