#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <string>
#include "Scene.h"
#include "tinyxml2.h"
#include <iostream>
#include <sstream>
#include "Vec3.h"
#include "Color.h"
#include "Camera.h"
#include <array>

using namespace tinyxml2;

class XMLParser 
{
    public:
        static Scene parseScene(const std::string& filename);
        static void parseCamera(tinyxml2::XMLElement* camElem, Scene& scene);
        static void parseLights(XMLElement* lightsElem, Scene& scene);
        static void parseMaterials(tinyxml2::XMLElement* materialsElem, Scene& scene);
        static void parseGeometryData(XMLElement* root, Scene& scene);
        static void parseObjects(tinyxml2::XMLElement* root, Scene& scene);
};

#endif // XMLPARSER_H
