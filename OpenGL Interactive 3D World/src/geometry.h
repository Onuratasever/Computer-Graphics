#pragma once
#include <glad/glad.h>
#include <vector>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265359f
#endif

struct AABB 
{
    glm::vec3 min;
    glm::vec3 max;

    // center is the midpoint of the AABB
    glm::vec3 center() const 
    {
        return (min + max) * 0.5f;
    }
};

// Check if two AABBs overlap
bool AABBvsAABB(const AABB& a, const AABB& b) 
{
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
           (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
           (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

struct Object 
{
    glm::vec3 position;
    glm::vec3 scale;
    AABB box;
    
    // Updates the bounding box based on position and scale
    void updateBox() 
    {
        glm::vec3 half = 0.5f * scale;
        box.min = position - half;
        box.max = position + half;
    }

    void move(glm::vec3 delta, 
              const std::vector<AABB*>& staticColliders,
              const std::vector<Object*>& dynamicObjects)
    {
        if (glm::any(glm::isnan(position + delta))) 
        {
            std::cout << "NaN detected in move delta!" << std::endl;
            return;
        }
    
        glm::vec3 newPos = position + delta;
        glm::vec3 half = 0.5f * scale;
        AABB newBox { newPos - half, newPos + half };

        // Check for collisions with static colliders
        for (const auto& col : staticColliders) {
            if (AABBvsAABB(newBox, *col))
                return; // Do not move if collision detected
        }

        // Check for collisions with dynamic objects
        for (const auto& obj : dynamicObjects) {
            if (obj == this) continue;
            if (AABBvsAABB(newBox, obj->box))
                return; // Do not move if collision detected
        }
        position = newPos;
        updateBox();
    }
};

static AABB groundBox  = { glm::vec3(-5.0f, -0.51f, -5.0f), glm::vec3(5.0f, -0.49f, 5.0f) };
static AABB leftWall   = { glm::vec3(-5.0f, -0.5f, -5.0f), glm::vec3(-4.9f, 9.9f, 5.0f) };
static AABB rightWall  = { glm::vec3(4.9f, -0.5f, -5.0f),  glm::vec3(5.0f, 9.9f, 5.0f) };
static AABB backWall   = { glm::vec3(-5.0f, -0.5f, -5.0f), glm::vec3(5.0f, 9.9f, -4.9f) };
static AABB frontWall  = { glm::vec3(-5.0f, -0.5f, 4.9f),  glm::vec3(5.0f, 9.9f, 5.0f) };

static std::vector<AABB*> staticAllColliders = {
     &groundBox,
    &leftWall,
    &rightWall,
    &backWall,
    &frontWall
};

static std::vector<Object*> DynamicObjects = {};

// ================= CUBE =================
inline unsigned int getCubeVAO() {
    static unsigned int VAO = 0;
    if(VAO) return VAO;

    static const float cubeVertices[] = {
        // positions          // normals          // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    glGenVertexArrays(1, &VAO);
    unsigned int VBO; 
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Texture coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    return VAO;
}

// ================= PLANE =================
inline unsigned int getPlaneVAO() 
{
    static unsigned int VAO = 0;
    if(VAO) return VAO;

    static const float planeVertices[] = {
        // positions          // normals         // texture coords
         5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f,  0.0f, 2.0f,

         5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f,  2.0f, 2.0f
    };

    glGenVertexArrays(1, &VAO);
    unsigned int VBO; 
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    return VAO;
}

// ================= SPHERE =================
inline unsigned int getSphereVAO() 
{
    static unsigned int VAO = 0;
    //static int indexCount = 0;
    if(VAO) return VAO;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    const unsigned int X_SEGMENTS = 32;
    const unsigned int Y_SEGMENTS = 32;
    
    for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = std::cos(xSegment * 2.0f * M_PI) * std::sin(ySegment * M_PI);
            float yPos = std::cos(ySegment * M_PI);
            float zPos = std::sin(xSegment * 2.0f * M_PI) * std::sin(ySegment * M_PI);

            vertices.push_back(xPos);
            vertices.push_back(yPos);
            vertices.push_back(zPos);
            vertices.push_back(xPos);  // normal = position for unit sphere
            vertices.push_back(yPos);
            vertices.push_back(zPos);
            vertices.push_back(xSegment);  // texture coord
            vertices.push_back(ySegment);
        }
    }

    for (unsigned int y = 0; y < Y_SEGMENTS; ++y) {
        for (unsigned int x = 0; x < X_SEGMENTS; ++x) {
            indices.push_back(y * (X_SEGMENTS + 1) + x);
            indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            indices.push_back((y + 1) * (X_SEGMENTS + 1) + x + 1);

            indices.push_back(y * (X_SEGMENTS + 1) + x);
            indices.push_back((y + 1) * (X_SEGMENTS + 1) + x + 1);
            indices.push_back(y * (X_SEGMENTS + 1) + x + 1);
        }
    }
    //indexCount = indices.size();

    glGenVertexArrays(1, &VAO);
    unsigned int VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    return VAO;
}

// Helper function to get sphere index count
inline int getSphereIndexCount() 
{
    getSphereVAO(); // Initialize if not done
    return 32 * 32 * 6; // X_SEGMENTS * Y_SEGMENTS * 6
}

// ================= CYLINDER =================
inline unsigned int getCylinderVAO() 
{
    static unsigned int VAO = 0;
    //static int indexCount = 0;
    if(VAO) return VAO;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    const unsigned int SEGMENTS = 24;
    const float height = 2.0f;
    
    // Top center
    vertices.insert(vertices.end(), {0.0f, height/2, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f});
    
    // Bottom center  
    vertices.insert(vertices.end(), {0.0f, -height/2, 0.0f, 0.0f, -1.0f, 0.0f, 0.5f, 0.5f});

    // Side vertices
    for(unsigned int i = 0; i <= SEGMENTS; ++i) {
        float angle = 2.0f * M_PI * i / SEGMENTS;
        float x = cos(angle);
        float z = sin(angle);
        
        // Top ring
        vertices.insert(vertices.end(), {x, height/2, z, x, 0.0f, z, (float)i/SEGMENTS, 1.0f});
        // Bottom ring
        vertices.insert(vertices.end(), {x, -height/2, z, x, 0.0f, z, (float)i/SEGMENTS, 0.0f});
    }

    // Top face indices
    for(unsigned int i = 0; i < SEGMENTS; ++i) {
        indices.insert(indices.end(), {0, 2 + 2*i, 2 + 2*((i+1)%SEGMENTS)});
    }
    
    // Bottom face indices
    for(unsigned int i = 0; i < SEGMENTS; ++i) {
        indices.insert(indices.end(), {1, 3 + 2*((i+1)%SEGMENTS), 3 + 2*i});
    }
    
    // Side faces
    for(unsigned int i = 0; i < SEGMENTS; ++i) {
        unsigned int next = (i + 1) % SEGMENTS;
        indices.insert(indices.end(), {
            2 + 2*i, 3 + 2*i, 2 + 2*next,
            2 + 2*next, 3 + 2*i, 3 + 2*next
        });
    }
    
    //indexCount = indices.size();

    glGenVertexArrays(1, &VAO);
    unsigned int VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    return VAO;
}

inline int getCylinderIndexCount() 
{
    getCylinderVAO();
    return 24 * 4; // SEGMENTS * 4 triangles per segment (approx)
}

// ================= PYRAMID =================
inline unsigned int getPyramidVAO() 
{
    static unsigned int VAO = 0;
    if(VAO) return VAO;

    static const float pyramidVertices[] = {
        // Base (square)
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,

        // Front face
        -0.5f, -0.5f, 0.5f,   0.0f,  0.447f,  0.894f,  0.0f, 0.0f,
         0.5f, -0.5f, 0.5f,   0.0f,  0.447f,  0.894f,  1.0f, 0.0f,
         0.0f,  0.5f, 0.0f,   0.0f,  0.447f,  0.894f,  0.5f, 1.0f,

        // Right face
         0.5f, -0.5f,  0.5f,  0.894f,  0.447f,  0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.894f,  0.447f,  0.0f,  1.0f, 0.0f,
         0.0f,  0.5f,  0.0f,  0.894f,  0.447f,  0.0f,  0.5f, 1.0f,

        // Back face
         0.5f, -0.5f, -0.5f,  0.0f,  0.447f, -0.894f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.447f, -0.894f,  1.0f, 0.0f,
         0.0f,  0.5f,  0.0f,  0.0f,  0.447f, -0.894f,  0.5f, 1.0f,

        // Left face
        -0.5f, -0.5f, -0.5f, -0.894f,  0.447f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, -0.894f,  0.447f,  0.0f,  1.0f, 0.0f,
         0.0f,  0.5f,  0.0f, -0.894f,  0.447f,  0.0f,  0.5f, 1.0f
    };

    glGenVertexArrays(1, &VAO);
    unsigned int VBO; 
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    return VAO;
}