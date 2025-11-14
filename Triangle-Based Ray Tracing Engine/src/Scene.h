#ifndef SCENE_H
#define SCENE_H
#include <vector>
#include <string>
#include "Camera.h"
#include "Vec3.h"
#include "Color.h"
#include <memory>
#include <vector>

// FaceIndex: 1 vertex için id'ler
struct FaceIndex {
    int vertexId;
    int textureId;
    int normalId;
};

// Mesh
class Mesh 
{
    public:
        int id;
        int materialId;
        std::vector<std::array<FaceIndex, 3>> faces; // Her üçgen için 3 adet FaceIndex
};

// Material
class Material 
{
    public:
        int id;
        Vec3 ambient, diffuse, specular, mirrorReflectance;
        float phongExponent;
        float texturefactor = -1;
};

// Light
enum LightType 
{
    AMBIENT,
    POINT,
    TRIANGLE
};

class Light 
{
    public:
        int id;
        Vec3 intensity;
        LightType type;
    
        Light(int id, const Vec3& intensity, LightType type): id(id), intensity(intensity), type(type) {}
    
        virtual ~Light() = default;
};

class AmbientLight : public Light 
{
    public:
        AmbientLight(const Vec3& intensity)
            : Light(-1, intensity, LightType::AMBIENT) {}
};

class PointLight : public Light 
{
    public:
        Vec3 position;

        PointLight(int id, const Vec3& position, const Vec3& intensity)
        : Light(id, intensity, LightType::POINT), position(position) {}
};

class TriangleLight : public Light 
{
    public:
        Vec3 v0, v1, v2;

        TriangleLight(int id, const Vec3& v0, const Vec3& v1, const Vec3& v2, const Vec3& intensity)
            : Light(id, intensity, LightType::TRIANGLE), v0(v0), v1(v1), v2(v2) {}
};

class Objects 
{
    public:
        std::vector<Mesh> meshes;
};

class Scene 
{
    public:
        int maxRayTraceDepth;
        Color backgroundColor;
        Camera camera;
        Objects objects;
        std::vector<Material> materials;
        std::vector<Vec3> vertexData;
        std::vector<Vec3> normalData;
        std::vector<Vec2f> textureData;
        std::string textureImageName;
        std::vector<std::shared_ptr<Light>> lights;
};


#endif // SCENE_H