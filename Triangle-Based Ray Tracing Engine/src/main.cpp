#include <iostream>
#include "ImageWriter.h"
#include "Image.h"
#include <string>
#include "Camera.h"
#include "Scene.h"
#include "XMLParser.h"
#define STB_IMAGE_IMPLEMENTATION
#include "./Include/stb_image.h"
#include <bits/algorithmfwd.h>
#include <thread>
#include <chrono>

using namespace std;

// define all the functions
const Light* getAmbientLight(const Scene& scene);
Color getTextureColor(const Scene& scene, const FaceIndex& f0, const FaceIndex& f1, const FaceIndex& f2,
    float alpha, float beta, float gamma);
bool isInShadow(const Scene& scene, const Vec3& origin, const Vec3& direction, float maxDistance);
Color computeLighting(const Scene& scene, const Vec3& hitPoint, const Vec3& normal,
    const Material& mat, const Ray& ray);
Color computeReflection(const Scene& scene, const Ray& ray, const Vec3& hitPoint,
    const Vec3& normal, const Material& mat, const Color& baseColor, int depth);
bool intersectRayWithTriangle(const Vec3& o, const Vec3& d,
    const Vec3& a, const Vec3& b, const Vec3& c,
    float& t, float& beta, float& gamma);
Vec3 computeColorTriangle(const Ray& ray, const Scene& scene, int depth);
Vec2f computeInterpolatedUV(
    const Scene& scene,
    const FaceIndex& f0,
    const FaceIndex& f1,
    const FaceIndex& f2,
    float beta,
    float gamma);


template <typename T>
T myClamp(T value, T minVal, T maxVal)
{
    if (value < minVal) return minVal;
    if (value > maxVal) return maxVal;
    return value;
}

struct TextureImage
{
    unsigned char* data = nullptr;
    int width, height, channels;

    // Returns the RGB float color of the (x,y) pixel
    Color getColor(int x, int y) const
    {
        // 1. Check if texture data is loaded
        if (data == nullptr)
        {
            return Color(1, 0, 1); // Magenta → error color
        }
    
        // 2. Clamp x and y to stay within valid range
        x = myClamp(x, 0, width - 1);
        y = myClamp(y, 0, height - 1);
    
        // 3. Compute the index in the data array
        int index = (y * width + x) * channels;
    
        int dataLength = width * height * channels;

        // 4. Check if index goes out of bounds
        int maxIndex = width * height * channels;

        if (index + 2 >= maxIndex)
        {
            return Color(1, 0, 1); // Error → return magenta
        }
    
        // 5. Read RGB values and normalize them from 0–255 to 0–1
        float r = data[index]     / 255.0f;
        float g = data[index + 1] / 255.0f;
        float b = data[index + 2] / 255.0f;

        return Color(r, g, b);
    }
};


TextureImage textureData; // Global texture data

Color computeAmbientComponent(const Light* ambientLight, const Material& mat)
{
    if (ambientLight == nullptr)
        return Color(0.0f, 0.0f, 0.0f);

    return Color(
        mat.ambient.x * ambientLight->intensity.x / 255.0f,
        mat.ambient.y * ambientLight->intensity.y / 255.0f,
        mat.ambient.z * ambientLight->intensity.z / 255.0f
    );
}

Vec2f computeInterpolatedUV(
    const Scene& scene,
    const FaceIndex& f0,
    const FaceIndex& f1,
    const FaceIndex& f2,
    float beta,
    float gamma)
{
    // Alpha
    float alpha = 1.0f - beta - gamma;

    // UV koordinatlarını al
    Vec2f uv0 = scene.textureData[f0.textureId];
    Vec2f uv1 = scene.textureData[f1.textureId];
    Vec2f uv2 = scene.textureData[f2.textureId];

    // Barycentrik interpolasyon
    return uv0 * alpha + uv1 * beta + uv2 * gamma;
}

// 1. Ambient light
const Light* getAmbientLight(const Scene& scene) 
{
    for (const auto& light : scene.lights) 
    {
        if (light->type == LightType::AMBIENT) 
        {
            return light.get();
        }
    }
    return nullptr;
}

// 2. UV interpolasyonu and texture color
Color getTextureColor(const Scene& scene, Vec2f& uv) 
{
    // 4. (u, v)
    int texX = static_cast<int>(uv.u * textureData.width);
    int texY = static_cast<int>((1.0f - uv.v) * textureData.height);
    // clamp (safety)
    texX = myClamp(texX, 0, textureData.width - 1);
    texY = myClamp(texY, 0, textureData.height - 1);

    return textureData.getColor(texX, texY);
}

// 3. Shadow check
bool isInShadow(const Scene& scene, const Vec3& origin, const Vec3& direction, float maxDistance) {
    
    Ray shadowRay(origin, direction);
    
    for (const auto& mesh : scene.objects.meshes) 
    {
        for (const auto& tri : mesh.faces) 
        {
            Vec3 a = scene.vertexData[tri[0].vertexId];
            Vec3 b = scene.vertexData[tri[1].vertexId];
            Vec3 c = scene.vertexData[tri[2].vertexId];
            
            float t, beta, gamma;
            
            if (intersectRayWithTriangle(shadowRay.getOrigin(), shadowRay.getDirection(), a, b, c, t, beta, gamma)) 
            {
                if (t > 1e-4f && t < maxDistance) return true;
            }
        }
    }
    return false;
}

// 4. Calculate lighting
Color computeLighting(const Scene& scene, const Vec3& hitPoint, const Vec3& normal,
                      const Material& mat, const Ray& ray) 
{
    Color result(0, 0, 0.0);

    Vec3 adjustedNormal = normal;

    if (ray.getDirection().dot(normal) > 0) 
    {
        adjustedNormal = Vec3(-normal.x, -normal.y, -normal.z);
    }
        
    for (const auto& lightPtr : scene.lights) 
    {
        if (lightPtr->type == LightType::AMBIENT) continue;

        float lightDistance;
        Vec3 lightDir;

        if (lightPtr->type == LightType::POINT) 
        {
            auto* pl = static_cast<const PointLight*>(lightPtr.get());
            lightDir = (pl->position - hitPoint);
            lightDistance = lightDir.length();
            lightDir = lightDir.normalized();
        }
        else if (lightPtr->type == LightType::TRIANGLE) 
        {
            auto* tl = static_cast<const TriangleLight*>(lightPtr.get());
            Vec3 edge1 = tl->v1 - tl->v0;
            Vec3 edge2 = tl->v2 - tl->v0;
            lightDir = (edge1.cross(edge2).normalized()) * -1.0f;
            lightDistance = 1e9f;

            // auto* tl = static_cast<const TriangleLight*>(lightPtr.get());

            // Vec3 edge1 = tl->v0 - tl->v1; // vertex1 - vertex2
            // Vec3 edge2 = tl->v0 - tl->v2; // vertex1 - vertex3

            // lightDir = (edge1.cross(edge2)).normalized();
            // lightDir = lightDir * -1.0f; // light dir

            // lightDistance = 1e9f; // infinite distance
        } 
        else 
            continue;

       if (isInShadow(scene, hitPoint + adjustedNormal * 0.001f, lightDir, lightDistance)) continue;

        float diff = std::max(0.0f, adjustedNormal.dot(lightDir));
        
        Color diffuse(
            mat.diffuse.x * lightPtr->intensity.x * diff / 255.0f,
            mat.diffuse.y * lightPtr->intensity.y * diff / 255.0f,
            mat.diffuse.z * lightPtr->intensity.z * diff / 255.0f
        );

        Vec3 viewDir = (ray.getOrigin() - hitPoint).normalized();
        Vec3 halfDir = (viewDir + lightDir).normalized();
        
        float specAngle = std::max(0.0f, adjustedNormal.dot(halfDir));
        float spec = pow(specAngle, mat.phongExponent);

        Color specular(
            mat.specular.x * lightPtr->intensity.x * spec / 255.0f,
            mat.specular.y * lightPtr->intensity.y * spec / 255.0f,
            mat.specular.z * lightPtr->intensity.z * spec / 255.0f
        );
        result += diffuse + specular;
    }

    
    result = Color(
        myClamp(result.getColorR(), 0.0f, 1.0f),
        myClamp(result.getColorG(), 0.0f, 1.0f),
        myClamp(result.getColorB(), 0.0f, 1.0f)
    );
    return result;
}

// 5. Reflection
Color computeReflection(const Scene& scene, const Ray& ray, const Vec3& hitPoint,
                        const Vec3& normal, const Material& mat, const Color& baseColor, int depth)
{
    Vec3 adjustedNormal = normal;

    if (ray.getDirection().dot(adjustedNormal) > 0)
        adjustedNormal = Vec3(-normal.x, -normal.y, -normal.z);

    Vec3 wo = ray.getDirection() * -1.0f;
    float dotProduct = adjustedNormal.dot(wo);
    Vec3 reflectDir = Vec3(
        -wo.x + 2.0f * adjustedNormal.x * dotProduct,
        -wo.y + 2.0f * adjustedNormal.y * dotProduct,
        -wo.z + 2.0f * adjustedNormal.z * dotProduct
    );

    Ray reflectedRay(
        Vec3(
            hitPoint.x + adjustedNormal.x * 0.001f,
            hitPoint.y + adjustedNormal.y * 0.001f,
            hitPoint.z + adjustedNormal.z * 0.001f
        ),
        reflectDir.normalized()
    );

    Vec3 reflectedColor = computeColorTriangle(reflectedRay, scene, depth - 1);

    return Color(
        baseColor.getColorR() * (1.0f - mat.mirrorReflectance.x) + reflectedColor.x * mat.mirrorReflectance.x,
        baseColor.getColorG() * (1.0f - mat.mirrorReflectance.y) + reflectedColor.y * mat.mirrorReflectance.y,
        baseColor.getColorB() * (1.0f - mat.mirrorReflectance.z) + reflectedColor.z * mat.mirrorReflectance.z
    );
}


bool intersectRayWithTriangle(const Vec3& o, const Vec3& d,
    const Vec3& a, const Vec3& b, const Vec3& c,
    float& t, float& beta, float& gamma)
{
    Vec3 e1 = b - a;
    Vec3 e2 = c - a;

    Vec3 s = o - a;

    // Matrisin columns:
    // A = [ -d | e1 | e2 ] → 3x3 matrix
    Vec3 col1 = d*-1; // -d
    Vec3 col2 = e1;
    Vec3 col3 = e2;

    // det(A) → determinant calc: |A| = col1 · (col2 × col3)
    float det = col1.dot(col2.cross(col3)); // cross product

    // if determinant too close to zero, return false
    if (fabs(det) < 1e-8) return false;

    // inverse det
    float invDet = 1.0f / det;

    // Cramer’s Rule:
    // t = |[s | e1 | e2]| / |A| = dot(s, (e1 × e2)) / det
    float tVal = s.dot(col2.cross(col3)) * invDet; // t = |[s | e1 | e2]| / |A|

    // β = |[-d | s | e2]| / |A| = dot(-d, (s × e2)) / det
    // float betaVal = dot(col1, cross(s, col3)) * invDet;
    float betaVal = col1.dot(s.cross(col3)) * invDet; // β = |[-d | s | e2]| / |A|

    // γ = |[-d | e1 | s]| / |A| = dot(-d, (e1 × s)) / det
    // float gammaVal = dot(col1, cross(col2, s)) * invDet;
    float gammaVal = col1.dot(col2.cross(s)) * invDet; // γ = |[-d | e1 | s]| / |A|
    // Barycentric conditions:
    if (betaVal < 0 || gammaVal < 0 || (betaVal + gammaVal) > 1) return false;

    // t < 0
    if (tVal < 0) return false;

    t = tVal;
    beta = betaVal;
    gamma = gammaVal;

    return true;
}

Vec3 computeColorTriangle(const Ray& ray, const Scene& scene, int depth)
{
    Color resultColor(scene.backgroundColor.getColorR(), scene.backgroundColor.getColorG(), scene.backgroundColor.getColorB());
    float minT = 1e9;

    const Light* ambientLight = getAmbientLight(scene);

    Vec3 hitPoint, normal;
    Color textureColor;
    float tFactor = 0.0f;
    const Material* hitMaterial = nullptr;

    FaceIndex f0, f1, f2;
    float beta = 0, gamma = 0;

    // --- Triangle intersection ---
    for (const auto& mesh : scene.objects.meshes)
    {
        const Material& mat = scene.materials[mesh.materialId - 1];

        for (const auto& triangle : mesh.faces)
        {
            FaceIndex tempF0 = triangle[0];
            FaceIndex tempF1 = triangle[1];
            FaceIndex tempF2 = triangle[2];

            Vec3 a = scene.vertexData[tempF0.vertexId];
            Vec3 b = scene.vertexData[tempF1.vertexId];
            Vec3 c = scene.vertexData[tempF2.vertexId];

            float t, tempBeta, tempGamma;

            if (intersectRayWithTriangle(ray.getOrigin(), ray.getDirection(), a, b, c, t, tempBeta, tempGamma))
            {
                if (t < minT)
                {
                    minT = t;

                    f0 = tempF0;
                    f1 = tempF1;
                    f2 = tempF2;
                    beta = tempBeta;
                    gamma = tempGamma;

                    hitPoint = ray.getOrigin() + ray.getDirection() * t;
                    normal = scene.normalData[f0.normalId].normalized();

                    Vec2f uv = computeInterpolatedUV(scene, f0, f1, f2, beta, gamma);
                    textureColor = getTextureColor(scene, uv);
                    tFactor = mat.texturefactor;
                    hitMaterial = &mat;
                }
            }
        }
    }

    if (minT < 1e9)
    {
        Color finalColor = computeAmbientComponent(ambientLight, *hitMaterial);
        finalColor += computeLighting(scene, hitPoint, normal, *hitMaterial, ray);

        Color baseColor = finalColor * (1.0f - tFactor) + textureColor * tFactor;

        Color reflectionComponent(0, 0, 0);

        if (depth > 0 && (hitMaterial->mirrorReflectance.x > 0 || hitMaterial->mirrorReflectance.y > 0 || hitMaterial->mirrorReflectance.z > 0))
        {
            Vec3 normalAdjusted = normal;
            if (ray.getDirection().dot(normalAdjusted) > 0)
            {
                normalAdjusted = Vec3(-normal.x, -normal.y, -normal.z);
            }

            Vec3 wo = ray.getDirection() * -1.0f;
            float dotProduct = normalAdjusted.dot(wo);

            Vec3 reflectDir = Vec3(
                -wo.x + 2.0f * normalAdjusted.x * dotProduct,
                -wo.y + 2.0f * normalAdjusted.y * dotProduct,
                -wo.z + 2.0f * normalAdjusted.z * dotProduct
            );

            Ray reflectedRay(
                Vec3(
                    hitPoint.x + normalAdjusted.x * 0.001f,
                    hitPoint.y + normalAdjusted.y * 0.001f,
                    hitPoint.z + normalAdjusted.z * 0.001f
                ),
                reflectDir.normalized()
            );

            Vec3 reflectedColor = computeColorTriangle(reflectedRay, scene, depth - 1);

            reflectionComponent = Color(
                reflectedColor.x * hitMaterial->mirrorReflectance.x,
                reflectedColor.y * hitMaterial->mirrorReflectance.y,
                reflectedColor.z * hitMaterial->mirrorReflectance.z
            );
        }

        Color finalCombined = baseColor + reflectionComponent;

        resultColor = Color(
            myClamp(finalCombined.getColorR(), 0.0f, 1.0f),
            myClamp(finalCombined.getColorG(), 0.0f, 1.0f),
            myClamp(finalCombined.getColorB(), 0.0f, 1.0f)
        );
    }

    return Vec3(resultColor.getColorR(), resultColor.getColorG(), resultColor.getColorB());
}


void renderRow(int startRow, int endRow, Image& image, const Scene& scene)
    {
        int width = image.getWidth();
        for (int i = startRow; i < endRow; ++i)
        {
            for (int j = 0; j < width; ++j)
            {
                Ray ray = scene.camera.getRay(j, i);
                Vec3 rayColor = computeColorTriangle(ray, scene, scene.maxRayTraceDepth);
                image.setPixel(j, i, Color(rayColor.x, rayColor.y, rayColor.z));
            }
        }
    }

int main()
{
    int width = 2000;
    int height = 2000;
    int i, j;
    
    Scene scene = XMLParser::parseScene("scene.xml");
    string fileName = "output.ppm";

    Image image(scene.camera.getNx(), scene.camera.getNy());
    ImageWriter imageWriter;

    int originalChannels = 0;
    textureData.data = stbi_load(scene.textureImageName.c_str(),
                                &textureData.width,
                                &textureData.height,
                                &originalChannels,
                                3);  // 3 kanal

    textureData.channels = 3;
    if (textureData.data == nullptr)
    {
        std::cerr << "Texture loading failed!" << std::endl;
        exit(1);
    }

    // Fill the image with a black color
    for(i = 0; i < height; i++)
    {
        for(j = 0; j < width; j++)
        {
            image.setPixel(i, j, scene.backgroundColor);
        }
    }

    int threadCount = std::thread::hardware_concurrency();
    vector<std::thread> threads;

    int rowsPerThread = height / threadCount;
    auto start = std::chrono::high_resolution_clock::now();
    //number of thread
    std::cout << "Number of threads: " << threadCount << std::endl;
    for (int t = 0; t < threadCount; ++t)
    {
        int startRow = t * rowsPerThread;
        int endRow = (t == threadCount - 1) ? height : (t + 1) * rowsPerThread;

        threads.emplace_back(renderRow, startRow, endRow, std::ref(image), std::cref(scene));
    }

    // Thread'lerin bitmesini bekle
    for (auto& th : threads)
    {
        th.join();
    }

    imageWriter.writePPM(fileName.c_str(), image); // convert string to const char*

    std::cout << "Image written to " << fileName << std::endl;
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Render time: " << duration.count() << " seconds" << std::endl;
    return 0;
}