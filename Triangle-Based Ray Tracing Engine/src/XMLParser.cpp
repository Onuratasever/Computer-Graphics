#include "XMLParser.h"

Vec3 parseVec3(const std::string& text) 
{
    std::istringstream ss(text);
    float x, y, z;
    ss >> x >> y >> z;
    return Vec3(x, y, z);
}

Vec2f parseVec2f(const std::string& text) 
{
    std::istringstream ss(text);

    float x, y;

    ss >> x >> y;

    Vec2f vec;

    vec.u = x;
    vec.v = y;
    
    return vec;
}

void XMLParser::parseCamera(XMLElement* camElem, Scene& scene) 
{
    float distance, left, right, bottom, top;
    int nx, ny;
    Vec3 origin, gaze, up;

    // <position>
    const char* posStr = camElem->FirstChildElement("position")->GetText();
    sscanf(posStr, "%f %f %f", &origin.x, &origin.y, &origin.z);

    // <gaze>
    const char* gazeStr = camElem->FirstChildElement("gaze")->GetText();
    sscanf(gazeStr, "%f %f %f", &gaze.x, &gaze.y, &gaze.z);

    // <up>
    const char* upStr = camElem->FirstChildElement("up")->GetText();
    sscanf(upStr, "%f %f %f", &up.x, &up.y, &up.z);

    // <nearPlane>
    const char* planeStr = camElem->FirstChildElement("nearPlane")->GetText();
    sscanf(planeStr, "%f %f %f %f", &left, &right, &bottom, &top);

    // <neardistance>
    camElem->FirstChildElement("neardistance")->QueryFloatText(&distance);

    // <imageresolution>
    const char* resStr = camElem->FirstChildElement("imageresolution")->GetText();
    sscanf(resStr, "%d %d", &nx, &ny);

    // Kamera nesnesini oluştur ve Scene'e ata
    scene.camera = Camera(distance, left, right, bottom, top, nx, ny, gaze, up, origin);
}

void XMLParser::parseLights(XMLElement* lightsElem, Scene& scene) {
    // Ambient Light
    XMLElement* ambientElem = lightsElem->FirstChildElement("ambientlight");
    if (ambientElem) 
    {
        Vec3 ambientIntensity = parseVec3(ambientElem->GetText());
        scene.lights.push_back(std::make_shared<AmbientLight>(ambientIntensity));
    }

    // Point Lights
    for (XMLElement* pointLightElem = lightsElem->FirstChildElement("pointlight");
         pointLightElem != nullptr;
         pointLightElem = pointLightElem->NextSiblingElement("pointlight")) 
    {
        int id = pointLightElem->IntAttribute("id");

        Vec3 position = parseVec3(pointLightElem->FirstChildElement("position")->GetText());
        Vec3 intensity = parseVec3(pointLightElem->FirstChildElement("intensity")->GetText());

        scene.lights.push_back(std::make_shared<PointLight>(id, position, intensity));
    }

    // Triangle Lights
    for (XMLElement* triLightElem = lightsElem->FirstChildElement("triangularlight");
         triLightElem != nullptr;
         triLightElem = triLightElem->NextSiblingElement("triangularlight")) 
    {
        int id = triLightElem->IntAttribute("id");

        Vec3 v0 = parseVec3(triLightElem->FirstChildElement("vertex1")->GetText());
        Vec3 v1 = parseVec3(triLightElem->FirstChildElement("vertex2")->GetText());
        Vec3 v2 = parseVec3(triLightElem->FirstChildElement("vertex3")->GetText());
        Vec3 intensity = parseVec3(triLightElem->FirstChildElement("intensity")->GetText());

        scene.lights.push_back(std::make_shared<TriangleLight>(id, v0, v1, v2, intensity));
    }
}

void XMLParser::parseMaterials(tinyxml2::XMLElement* materialsElem, Scene& scene)
{
    using namespace tinyxml2;

    for (XMLElement* materialElem = materialsElem->FirstChildElement("material");
         materialElem != nullptr;
         materialElem = materialElem->NextSiblingElement("material"))
    {
        Material material;
        material.id = materialElem->IntAttribute("id");

        material.ambient  = parseVec3(materialElem->FirstChildElement("ambient")->GetText());
        material.diffuse  = parseVec3(materialElem->FirstChildElement("diffuse")->GetText());
        material.specular = parseVec3(materialElem->FirstChildElement("specular")->GetText());
        material.mirrorReflectance = parseVec3(materialElem->FirstChildElement("mirrorreflactance")->GetText());

        materialElem->FirstChildElement("phongexponent")->QueryFloatText(&material.phongExponent);

        // texturefactor opsiyonel olabilir
        if (auto texFactorElem = materialElem->FirstChildElement("texturefactor")) {
            texFactorElem->QueryFloatText(&material.texturefactor); // veya float ise `QueryFloatText`
        }

        scene.materials.push_back(material);
    }
}

void XMLParser::parseGeometryData(XMLElement* root, Scene& scene)
{
    // VERTEX DATA
    if (auto vertexElem = root->FirstChildElement("vertexdata"))
    {
        std::istringstream ss(vertexElem->GetText());
        float x, y, z;
        while (ss >> x >> y >> z)
        {
            scene.vertexData.push_back(Vec3(x, y, z));
        }
    }

    // NORMAL DATA
    if (auto normalElem = root->FirstChildElement("normaldata"))
    {
        std::istringstream ss(normalElem->GetText());
        float x, y, z;
        while (ss >> x >> y >> z)
        {
            scene.normalData.push_back(Vec3(x, y, z));
        }
    }

    // TEXTURE DATA
    if (auto texElem = root->FirstChildElement("texturedata"))
    {
        std::istringstream ss(texElem->GetText());
        float u, v;
        while (ss >> u >> v)
        {
            scene.textureData.push_back(Vec2f{u, v});
        }
    }
}

void XMLParser::parseObjects(tinyxml2::XMLElement* root, Scene& scene)
{
    if (auto objectsElem = root->FirstChildElement("objects"))
    {
        for (auto meshElem = objectsElem->FirstChildElement("mesh"); meshElem != nullptr; meshElem = meshElem->NextSiblingElement("mesh"))
        {
            Mesh mesh;
            mesh.id = meshElem->IntAttribute("id");

            // materialid
            auto matElem = meshElem->FirstChildElement("materialid");
            if (matElem)
                mesh.materialId = std::stoi(matElem->GetText());

            // faces
            auto facesElem = meshElem->FirstChildElement("faces");
            if (facesElem)
            {
                std::istringstream faceStream(facesElem->GetText());
                std::string faceToken;

                while (faceStream >> faceToken)
                {
                    std::array<FaceIndex, 3> triangle;

                    // parse 3 vertices per triangle
                    for (int i = 0; i < 3; ++i)
                    {
                        if (i > 0) faceStream >> faceToken;

                        FaceIndex idx;
                        sscanf(faceToken.c_str(), "%d/%d/%d", &idx.vertexId, &idx.textureId, &idx.normalId);

                        // OBJ-like format: index starts at 1, so convert to 0-based
                        idx.vertexId--;
                        idx.textureId--;
                        idx.normalId--;

                        triangle[i] = idx;
                    }

                    mesh.faces.emplace_back(triangle);
                }
            }

            scene.objects.meshes.push_back(mesh);
        }
    }
}

Scene XMLParser::parseScene(const std::string& filename) 
{
    Scene scene;

    XMLDocument doc;
    if (doc.LoadFile(filename.c_str()) != XML_SUCCESS) {
        std::cerr << "Failed to load XML file: " << filename << std::endl;
        return scene;
    }

    XMLElement* root = doc.FirstChildElement("scene");

    // Parse background color
    if (auto bgElem = root->FirstChildElement("backgroundColor")) 
    {
        Vec3 bgColor = parseVec3(bgElem->GetText());
        scene.backgroundColor = Color(bgColor.x, bgColor.y, bgColor.z);
    }

    // Parse maxRayTraceDepth
    if (auto depthElem = root->FirstChildElement("maxraytracedepth")) 
    {
        scene.maxRayTraceDepth = std::stoi(depthElem->GetText());
    }

    // Parse camera
    if (auto camElem = root->FirstChildElement("camera")) 
    {
        parseCamera(camElem, scene);
    }

    // Parse lights
    if (auto lightsElem = root->FirstChildElement("lights"))
    {
        parseLights(lightsElem, scene);
    }

    // Parse materials
    if (auto materialsElem = root->FirstChildElement("materials"))
    {
        parseMaterials(materialsElem, scene);
    }

    // Parse geometry data
    if (root)
    {
        parseGeometryData(root, scene);
    }
    
    // Parse texture image name
    if (auto texElem = root->FirstChildElement("textureimage"))
    {
        const char* name = texElem->GetText();
        if (name)
            scene.textureImageName = std::string(name);
    }

    // Parse objects
    if (root)
    {
        parseObjects(root, scene);
    }
   
    return scene;
}
