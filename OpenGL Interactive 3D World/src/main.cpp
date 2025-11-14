/************************  main.cpp  ************************/
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "Shader.h"
#include "Camera.h"
#include "geometry.h"   // cubeVAO(), planeVAO()
#include "texture.h"    // loadTexture()

// --------------------------- globals
GLFWwindow* window;

const unsigned int SCR_WIDTH  = 1200;
const unsigned int SCR_HEIGHT =  800;

// FPS Counter
float fpsTimer = 0.0f;
int frameCount = 0;

// 10x10 room
const float ROOM_W = 10.0f;
const float ROOM_H = 10.0f;

bool dirOn = true;
bool pOn[2] = { true, true };
bool keyLatch[3] = { false, false, false };   // prevent multiple key presses 

Camera camera;                      // start point (0,0,3)

float lastX = SCR_WIDTH  / 2.0f; // Mouse position
float lastY = SCR_HEIGHT / 2.0f; // Mouse position
bool  firstMouse = true;

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f;

// --------------------------- prototypes
void framebuffer_size_callback(GLFWwindow*, int, int);
void mouse_callback(GLFWwindow*, double, double);
void scroll_callback(GLFWwindow*, double, double);
void processInput(GLFWwindow*);

int glfwStartup()
{
     /* ---------------- GLFW / GLAD ------------- */
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    window =
        glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT,
                         "CSE461 - Programming Assignment 2",
                         nullptr, nullptr);
    if(!window){ std::cerr<<"Failed to create GLFW window\n"; return -1; }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback      (window, mouse_callback);
    glfwSetScrollCallback         (window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cerr<<"Failed to init GLAD\n"; return -1; }
    glEnable(GL_DEPTH_TEST);

    return 0;
}

Object* createDynamicObject(const glm::vec3& pos, const glm::vec3& scale)
{
    Object* obj = new Object();
    obj->position = pos;
    obj->scale = scale;
    obj->updateBox();
    DynamicObjects.push_back(obj);
    return obj;
}

Object* createStaticObject(const glm::vec3& pos, const glm::vec3& scale)
{
    Object* obj = new Object();
    obj->position = pos;
    obj->scale = scale;
    obj->updateBox();
    staticAllColliders.push_back(&obj->box); // cadd static colliders
    return obj;
}

void DrawWalls(Shader& shader, unsigned int planeVAO, unsigned int texFloor) {
    /* ---- floor ---- */
    shader.setMat4("model", glm::mat4(1.0f));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texFloor);
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Back wall (Z = -ROOM_W/2)
    glm::mat4 back = glm::translate(glm::mat4(1.0f), glm::vec3(0, ROOM_H / 2 - 0.5f, -ROOM_W / 2 + 0.5f));
    back = glm::rotate(back, glm::radians(90.f), glm::vec3(1, 0, 0));
    shader.setMat4("model", back);
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Front wall (Z = +ROOM_W/2)
    glm::mat4 front = glm::translate(glm::mat4(1.0f), glm::vec3(0, ROOM_H / 2 - 0.5f, ROOM_W / 2 - 0.5f));
    front = glm::rotate(front, glm::radians(270.f), glm::vec3(1, 0, 0));
    shader.setMat4("model", front);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Left wall (X = -ROOM_W/2)
    glm::mat4 left = glm::translate(glm::mat4(1.0f), glm::vec3(-ROOM_W / 2 + 0.5f, ROOM_H / 2 - 0.5f, 0));
    left = glm::rotate(left, glm::radians(270.f), glm::vec3(0, 0, 1));
    shader.setMat4("model", left);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Right wall (X = +ROOM_W/2)
    glm::mat4 right = glm::translate(glm::mat4(1.0f), glm::vec3(ROOM_W / 2 - 0.5f, ROOM_H / 2 - 0.5f, 0));
    right = glm::rotate(right, glm::radians(90.f), glm::vec3(0, 0, 1));
    shader.setMat4("model", right);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void drawCubes(Object* obj, Shader& shader, unsigned int cubeVAO, unsigned int texCube, bool isRotating = false, float t = 0.0f)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, obj->position);
    model = glm::scale(model, obj->scale);
    if(isRotating) {
        model = glm::rotate(model, t*0.7f, glm::vec3(0.3f,1.0f,0.2f));
    }
    shader.setMat4("model", model);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texCube);
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void renderDynamicObjects(Shader& shader, unsigned int cubeVAO, unsigned int texCube)
{
    for (Object* obj : DynamicObjects) 
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, obj->position);
        model = glm::scale(model, obj->scale);
        shader.setMat4("model", model);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

void drawSphere(Shader& shader, Object* sphere, unsigned int sphereVAO, unsigned int texSphere)
{
    shader.use();

    shader.setVec3("material.ambient",  glm::vec3(0.1f, 0.1f, 0.1f));
    shader.setVec3("material.diffuse",  glm::vec3(0.8f, 0.8f, 0.8f));
    shader.setVec3("material.specular", glm::vec3(0.7f, 0.7f, 0.7f));
    shader.setFloat("material.shininess", 32.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texSphere);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, sphere->position);
    model = glm::scale(model, glm::vec3(0.8f));

    shader.setMat4("model", model);
    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLES, getSphereIndexCount(), GL_UNSIGNED_INT, 0);
}

void SetBlendShaderCommonUniforms(Shader& Shader, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& viewPos) 
{
    Shader.use();
    Shader.setMat4("view", view);
    Shader.setMat4("projection", projection);
    Shader.setVec3("viewPos", viewPos);
}

void SetLightUniforms(Shader& shader,
                      const glm::vec3& dirDir,
                      const glm::vec3& dirColor,
                      bool dirOn,
                      const glm::vec3 pointPos[2],
                      const glm::vec3 pointColor[2],
                      const bool pOn[2])
{
    shader.setVec3("dirLight.direction", dirDir);
    shader.setVec3("dirLight.color",     (dirOn ? dirColor : glm::vec3(0.0f)));

    for(int i = 0; i < 2; ++i)
    {
        std::string base = "pointLights[" + std::to_string(i) + "].";
        shader.setVec3(base + "position", pointPos[i]);
        shader.setVec3(base + "color",    (pOn[i] ? pointColor[i] : glm::vec3(0.0f)));
    }
}

void drawPyramid(Shader& blendShader, unsigned int texCube, unsigned int texFloor, unsigned int pyramidVAO, Object* pyramid)
{
    // Textures for the blend
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texCube);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texFloor);

    // Model matrix for the pyramid and draw call
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pyramid->position);
    model = glm::scale(model, pyramid->scale);
    blendShader.setMat4("model", model);

    glBindVertexArray(pyramidVAO);
    glDrawArrays(GL_TRIANGLES, 0, 18);
}

void SetMaterialUniforms(Shader& shader,
                        const glm::vec3& ambient,
                        const glm::vec3& diffuse,
                        const glm::vec3& specular,
                        float shininess)
{
    shader.setVec3("material.ambient",  ambient);
    shader.setVec3("material.diffuse",  diffuse);
    shader.setVec3("material.specular", specular);
    shader.setFloat("material.shininess", shininess);
}

void fpsCounter()
{
    // FPS calculation
    fpsTimer += deltaTime;
    frameCount++;

    if (fpsTimer >= 1.0f) {
        std::cout << "FPS: " << frameCount << std::endl;
        fpsTimer = 0.0f;
        frameCount = 0;
    }
}

int main()
{
   
    glfwStartup();

    /* ---------------- Assets ------------------- */
    Shader shader("shaders/lit_tex.vs", "shaders/lit_tex.fs");
    Shader blendShader("shaders/lit_tex.vs", "shaders/blend.fs");
   
    unsigned int texCube  = loadTexture("textures/indir.jpeg");
    unsigned int texFloor = loadTexture("textures/my_texture_only_texture.png");
    unsigned int texSphere   = loadTexture("textures/stars.jpg");

    unsigned int cubeVAO   = getCubeVAO();
    unsigned int planeVAO  = getPlaneVAO();
    unsigned int sphereVAO = getSphereVAO();
    unsigned int pyramidVAO= getPyramidVAO();

    shader.use();
    SetMaterialUniforms(shader, glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(1.0f, 0.5f, 0.3f),  glm::vec3(0.8f, 0.8f, 0.8f), 32.0f);                         // shininess
    shader.setInt("diffuseMap", 0);   // one sampler

    blendShader.use();
    blendShader.setInt("texture1", 0);
    blendShader.setInt("texture2", 1);

    /* --- Light Def ------------------------ */
    glm::vec3 dirDir   = glm::normalize(glm::vec3(-0.3f, -1.0f, -0.2f));
    glm::vec3 dirColor = glm::vec3(1.0f, 0.95f, 0.9f);

    glm::vec3 pointPos[2]   = { glm::vec3( 2,  2,  2),
                                glm::vec3(-2,  1, -2) };
    glm::vec3 pointColor[2] = { glm::vec3(1.0f,0.8f,0.6f),
                                glm::vec3(0.4f,0.6f,1.0f) };
    
    //Collider definitions
    camera.SetColliders(staticAllColliders, DynamicObjects);

    /*----------------Objects---------------------------------- */
    Object* movingCube = createDynamicObject(glm::vec3(3, 0.25f, -1), glm::vec3(0.3f));

    Object* mediumCube = createStaticObject(glm::vec3(-2, 0.5f, 2), glm::vec3(0.6f));

    Object* largeCube = createStaticObject(glm::vec3(1, 0.8f, -3), glm::vec3(1.5f));

    Object* rotatingCube = createStaticObject(glm::vec3(0, 1, 0), glm::vec3(1.0f));

    Object* sphere = createStaticObject(glm::vec3(-2, 0.5f, -2), glm::vec3(1.8f));

    Object* pyramid = createStaticObject(glm::vec3(1,0,-2), glm::vec3(1.0f));

    /* ---------------- Render Loop -------------- */
    while(!glfwWindowShouldClose(window))
    {
        float t = glfwGetTime();
        deltaTime = t - lastFrame;
        lastFrame = t;

        fpsCounter();

    /* ----------  Keyboard Control ------------- */
        auto edgeToggle = [&](int key, int idx, bool &flag){
            if(glfwGetKey(window, key) == GLFW_PRESS){
                if(!keyLatch[idx]) { flag = !flag; keyLatch[idx] = true; }
            } else keyLatch[idx] = false;
        };
        edgeToggle(GLFW_KEY_1, 0, dirOn);   // 1 → directional
        edgeToggle(GLFW_KEY_2, 1, pOn[0]);  // 2 → point-0
        edgeToggle(GLFW_KEY_3, 2, pOn[1]);  // 3 → point-1
    /* ------------------------------------------- */

        processInput(window);

        glClearColor(0.1f,0.15f,0.2f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* ---- view / proj ---- */
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 proj = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH/SCR_HEIGHT, 0.1f, 100.0f);

        SetBlendShaderCommonUniforms(shader, view, proj, camera.Position);
        
        SetLightUniforms(shader, dirDir, dirColor, dirOn, pointPos, pointColor, pOn);
        
        SetMaterialUniforms(shader,
                            glm::vec3(0.2f, 0.2f, 0.2f),    // Ambient
                            glm::vec3(0.8f, 0.8f, 0.8f),    // Diffuse
                            glm::vec3(0.1f, 0.1f, 0.1f),    // Specular
                            8.0f);                          // Shininess
        DrawWalls(shader, planeVAO, texFloor);

        // red bright
        SetMaterialUniforms(shader,
                            glm::vec3(0.17f, 0.02f, 0.02f), // Ambient
                            glm::vec3(0.6f, 0.07f, 0.07f),  // Diffuse
                            glm::vec3(0.7f, 0.6f, 0.6f),    // Specular
                            76.8f);                         // Shininess
        drawCubes(rotatingCube, shader, cubeVAO, texCube, true, t);

         // blue
        SetMaterialUniforms(shader,
                            glm::vec3(0.0f, 0.05f, 0.1f),   // Ambient
                            glm::vec3(0.1f, 0.3f, 0.7f),    // Diffuse
                            glm::vec3(0.1f, 0.1f, 0.2f),    // Specular
                            10.0f);                         // Shininess
        drawCubes(movingCube, shader, cubeVAO, texCube, false, t);     
        
        // green metallic non-bright
        SetMaterialUniforms(shader,
                            glm::vec3(0.02f, 0.1f, 0.02f),  // Ambient
                            glm::vec3(0.2f, 0.5f, 0.2f),    // Diffuse
                            glm::vec3(0.6f, 0.8f, 0.6f),    // Specular
                            100.0f);                        // Shininess
        drawCubes(mediumCube, shader, cubeVAO, texCube, false, t);

        SetMaterialUniforms(shader,
                            glm::vec3(0.1f, 0.1f, 0.1f),    // Ambient
                            glm::vec3(1.0f, 0.5f, 0.3f),    // Diffuse (orange)
                            glm::vec3(0.1f, 0.1f, 0.1f),    // Specular
                            64.0f);                         // Shininess
        drawCubes(largeCube, shader, cubeVAO, texCube, false, t);

        renderDynamicObjects(shader, cubeVAO, texCube);
        
        drawSphere(shader, sphere, sphereVAO, texSphere);

        SetBlendShaderCommonUniforms(blendShader, view, proj, camera.Position);
        SetLightUniforms(blendShader, dirDir, dirColor, dirOn, pointPos, pointColor, pOn);

        drawPyramid(blendShader, texCube, texFloor, pyramidVAO, pyramid);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

/* -------------- callbacks & input -------------- */
void framebuffer_size_callback(GLFWwindow*, int w, int h)
{
    glViewport(0, 0, w, h);
}

void mouse_callback(GLFWwindow*, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if(firstMouse){ lastX = xpos; lastY = ypos; firstMouse = false; }
    float xoff = xpos - lastX;
    float yoff = lastY - ypos;   // y axis is inverted in OpenGL
    lastX = xpos; lastY = ypos;
    camera.ProcessMouseMovement(xoff, yoff);
}

void scroll_callback(GLFWwindow*, double, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void processInput(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if(glfwGetKey(window, GLFW_KEY_W)==GLFW_PRESS) camera.ProcessKeyboard(FORWARD , deltaTime);
    if(glfwGetKey(window, GLFW_KEY_S)==GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_A)==GLFW_PRESS) camera.ProcessKeyboard(LEFT    , deltaTime);
    if(glfwGetKey(window, GLFW_KEY_D)==GLFW_PRESS) camera.ProcessKeyboard(RIGHT   , deltaTime);
}
