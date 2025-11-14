#pragma once
#include <string>
#include <glm/glm.hpp>

class Shader {
public:
    unsigned int ID;
    Shader(const char* vertPath, const char* fragPath);

    void use() const;
    // Uniform helpers
    void setBool  (const std::string&, bool)   const;
    void setInt   (const std::string&, int)    const;
    void setFloat (const std::string&, float)  const;
    void setVec3  (const std::string&, const glm::vec3&) const;
    void setMat4  (const std::string&, const glm::mat4&) const;
private:
    static std::string readFile(const char*);
    static void checkCompile(unsigned int, const std::string&);
};
