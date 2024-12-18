#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <fstream>
#include "Shader.h"
#include "Camera.h"

struct ModelTransform
{
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    void setScale(float s)
    {
        scale.x = s;
        scale.y = s;
        scale.z = s;
    }
};

struct Color
{
    float r, g, b, a;
};

struct Material
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shiniess;
};

enum class LightType 
{
    Directional = 1,
    Point = 2,
    Spot =3
};

struct Light
{
    LightType type;

    glm::vec3 position;
    glm::vec3 direction;
    float cutOff;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

Color background = { 0.f, 0.f, 0.f, 1.f };

Camera camera(glm::vec3(0.f, 0.f, -5.f));

void OnResize(GLFWwindow* win, int width, int height)
{
    glViewport(0, 0, width, height);
    //std::cout << "��������������� �� " << width << "x" << height << std::endl;
}

void processInput(GLFWwindow* win, double dt)
{
    if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(win, true);
    if (glfwGetKey(win, GLFW_KEY_1) == GLFW_PRESS)
        background = { 1.0f, 0.0f, 0.0f, 1.0f };
    if (glfwGetKey(win, GLFW_KEY_2) == GLFW_PRESS)
        background = { 0.0f, 1.0f, 0.0f, 1.0f };
    if (glfwGetKey(win, GLFW_KEY_3) == GLFW_PRESS)
        background = { 0.0f, 0.0f, 1.0f, 1.0f };
    if (glfwGetKey(win, GLFW_KEY_4) == GLFW_PRESS)
        background = { 0.55f, 0.8f, 0.9f, 1.0f };
    if (glfwGetKey(win, GLFW_KEY_5) == GLFW_PRESS)
        background = { .0f, .0f, 0.0f, 1.0f };

    uint32_t dir = 0;

    if (glfwGetKey(win, GLFW_KEY_E) == GLFW_PRESS)
        dir |= CAM_UP;
    if (glfwGetKey(win, GLFW_KEY_Q) == GLFW_PRESS)
        dir |= CAM_DOWN;
    if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
        dir |= CAM_FORWARD;
    if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
        dir |= CAM_BACKWARD;
    if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS)
        dir |= CAM_LEFT;
    if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS)
        dir |= CAM_RIGHT;

    
    double newx = 0.f, newy = 0.f;
    glfwGetCursorPos(win, &newx, &newy);
    static double x = newx, y = newy;
    
    double xoffset = newx - x;
    double yoffset = newy - y;
    x = newx;
    y = newy;


    camera.Move(dir, dt);
    camera.Rotate(xoffset, -yoffset);
}

bool wireframeMode = false;

void UpdatePolygonMode()
{
    if (wireframeMode)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void OnKeyAction(GLFWwindow* win, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_SPACE:
            wireframeMode = !wireframeMode;
            UpdatePolygonMode();
            break;
        }
    }
}

void OnScroll(GLFWwindow* win, double x, double y)
{
    camera.ChangeFOV(y);
    std::cout << "FOV = " << camera.Fov << std::endl;
}

typedef unsigned char byte;

int main(void)
{
#pragma region WINDOW ITIALIZATION
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // ���������� �������� ������ OpenGL 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // ���������� �������������� ������ OpenGL 3.3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // ��������� ������� Opengl
    glfwWindowHint(GLFW_SAMPLES, 8);

    GLFWwindow* win = glfwCreateWindow(1280, 720, "OpenGL Window test", NULL, NULL);
    if (win == NULL)
    {
        std::cout << "������, �� ������� ������� ����" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(win);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "������, �� ������� ��������� GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(win, OnResize);
    glfwSetScrollCallback(win, OnScroll);
    glfwSetKeyCallback(win, OnKeyAction);

    glViewport(0, 0, 1280, 720);
    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    UpdatePolygonMode();
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    //glEnable(GL_MULTISAMPLE);

    int samples;
    glGetIntegerv(GL_SAMPLES, &samples);
    std::cout << "Samples: " << samples << std::endl;

#pragma endregion

    int box_width, box_height, channels;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    byte* data = stbi_load("images\\box.png", &box_width, &box_height, &channels, 0);
      
    const int verts = 36;

    float cube[] = {
        //position			normal					texture				color			
        -1.0f,-1.0f,-1.0f,	-1.0f,  0.0f,  0.0f,	0.0f, 0.0f,		0.0f, 1.0f, 0.0f,
        -1.0f,-1.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	1.0f, 0.0f,		0.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	1.0f, 1.0f,		0.0f, 1.0f, 0.0f,
        -1.0f,-1.0f,-1.0f,	-1.0f,  0.0f,  0.0f,	0.0f, 0.0f,		0.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	1.0f, 1.0f,		0.0f, 1.0f, 0.0f,
        -1.0f, 1.0f,-1.0f,	-1.0f,  0.0f,  0.0f,	0.0f, 1.0f,		0.0f, 1.0f, 0.0f,

        1.0f, 1.0f,-1.0f,	0.0f,  0.0f, -1.0f, 	0.0f, 1.0f,		1.0f, 0.0f, 0.0f,
        -1.0f,-1.0f,-1.0f,	0.0f,  0.0f, -1.0f, 	1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
        -1.0f, 1.0f,-1.0f,	0.0f,  0.0f, -1.0f, 	1.0f, 1.0f,		1.0f, 0.0f, 0.0f,
        1.0f, 1.0f,-1.0f,	0.0f,  0.0f, -1.0f,		0.0f, 1.0f,		1.0f, 0.0f, 0.0f,
        1.0f,-1.0f,-1.0f,	0.0f,  0.0f, -1.0f,		0.0f, 0.0f,		1.0f, 0.0f, 0.0f,
        -1.0f,-1.0f,-1.0f,	0.0f,  0.0f, -1.0f,		1.0f, 0.0f,		1.0f, 0.0f, 0.0f,

        1.0f,-1.0f, 1.0f,	0.0f, -1.0f,  0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,	0.0f, -1.0f,  0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 1.0f,
        1.0f,-1.0f,-1.0f,	0.0f, -1.0f,  0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 1.0f,
        1.0f,-1.0f, 1.0f,	0.0f, -1.0f,  0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,	0.0f, -1.0f,  0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,	0.0f, -1.0f,  0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 1.0f,

        -1.0f, 1.0f, 1.0f,	0.0f,  0.0f, 1.0f,		0.0f, 1.0f,		0.0f, 0.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,	0.0f,  0.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 1.0f,
        1.0f,-1.0f, 1.0f,	0.0f,  0.0f, 1.0f,		1.0f, 0.0f,		0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f,	0.0f,  0.0f, 1.0f,		1.0f, 1.0f,		0.0f, 0.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,	0.0f,  0.0f, 1.0f,		0.0f, 1.0f,		0.0f, 0.0f, 1.0f,
        1.0f,-1.0f, 1.0f,	0.0f,  0.0f, 1.0f,		1.0f, 0.0f,		0.0f, 0.0f, 1.0f,

        1.0f, 1.0f, 1.0f,	1.0f,  0.0f,  0.0f,		0.0f, 1.0f,		1.0f, 0.0f, 0.0f,
        1.0f,-1.0f,-1.0f,	1.0f,  0.0f,  0.0f,		1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
        1.0f, 1.0f,-1.0f,	1.0f,  0.0f,  0.0f,		1.0f, 1.0f,		1.0f, 0.0f, 0.0f,
        1.0f,-1.0f,-1.0f,	1.0f,  0.0f,  0.0f,		1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f,	1.0f,  0.0f,  0.0f,		0.0f, 1.0f,		1.0f, 0.0f, 0.0f,
        1.0f,-1.0f, 1.0f,	1.0f,  0.0f,  0.0f,		0.0f, 0.0f,		1.0f, 0.0f, 0.0f,

        1.0f, 1.0f, 1.0f,	0.0f,  1.0f,  0.0f,		1.0f, 0.0f,		0.0f, 1.0f, 0.0f,
        1.0f, 1.0f,-1.0f,	0.0f,  1.0f,  0.0f,		1.0f, 1.0f,		0.0f, 1.0f, 0.0f,
        -1.0f, 1.0f,-1.0f,	0.0f,  1.0f,  0.0f,		0.0f, 1.0f,		0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f,	0.0f,  1.0f,  0.0f,		1.0f, 0.0f,		0.0f, 1.0f, 0.0f,
        -1.0f, 1.0f,-1.0f,	0.0f,  1.0f,  0.0f,		0.0f, 1.0f,		0.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 1.0f,	0.0f,  1.0f,  0.0f,		0.0f, 0.0f,		0.0f, 1.0f, 0.0f
    };

    Material cubeMaterials[3] = {

    {   glm::vec3(0.25, 0.20725, 0.20725),
        glm::vec3(1, 0.829, 0.829),
        glm::vec3(0.296648, 0.296648, 0.296648),
        12.0f
    },

    {   glm::vec3(0.25, 0.25, 0.25),
        glm::vec3(0.4, 0.4, 0.4),
        glm::vec3(0.774597, 0.774597, 0.774597),
        77.0f
    },

    {   glm::vec3(0.1745, 0.01175, 0.01175),
        glm::vec3(0.61424, 0.04136, 0.04136),
        glm::vec3(0.727811, 0.626959, 0.626959),
        77.0f
    }
    };

    const int cube_count = 200;

    ModelTransform cubeTrans[cube_count];

    int cubeMat[cube_count];

    for (int i = 0; i < cube_count; i++)
    {
        float scale = (rand() % 6 + 1) / 20.0f;
        cubeTrans[i] = { 
            glm::vec3((rand() % 201 - 100) / 40.0f, (rand() % 201 - 100) / 40.0f, (rand() % 201 - 100) / 40.0f),
            glm::vec3(rand() / 10.0f, rand() / 10.0f, rand() / 10.0f),
            glm::vec3(scale, scale, scale)
        };
        cubeMat[i] = rand() % 3;

        if ((glm::vec3(0, 0, 0) - cubeTrans[i].position).length() < 0.7)
            i--;
    };


    ModelTransform cubeTrans2 = { glm::vec3(0.f, 0.f, 0.f),
                                   glm::vec3(0.f, 0.f, 0.f),
                                   glm::vec3(1.f, 1.f, 1.f) };

    ModelTransform cubeTrans3 = { glm::vec3(0.f, 0.f, 0.f),
                                   glm::vec3(0.f, 0.f, 0.f),
                                   glm::vec3(1.f, 1.f, 1.f) };

    ModelTransform lightTrans = { glm::vec3(0.f, 0.f, 0.f),
                                  glm::vec3(0.f, 0.f, 0.f),
                                  glm::vec3(0.2f, 0.2f, 0.2f) };

    Material cubeMat1 = {   glm::vec3(0.25, 0.20725, 0.20725),
                            glm::vec3(1, 0.829, 0.829),
                            glm::vec3(0.296648, 0.296648, 0.296648),
                            12.0f 
    };

    Material cubeMat2 = {   glm::vec3(0.25, 0.25, 0.25),
                            glm::vec3(0.4, 0.4, 0.4),
                            glm::vec3(0.774597, 0.774597, 0.774597),
                            77.0f
    };

    Material cubeMat3 = {   glm::vec3(0.1745, 0.01175, 0.01175),
                            glm::vec3(0.61424, 0.04136, 0.04136),
                            glm::vec3(0.727811, 0.626959, 0.626959),
                            77.0f
    };

#pragma region BUFFERS INITIALIZATION

    #pragma region SHADOW BUFFER INITIALIZATION
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    GLuint depthMap;

    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO); 
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0); 
    glDrawBuffer(GL_NONE); 
    glReadBuffer(GL_NONE); 
    glBindFramebuffer(GL_FRAMEBUFFER, 0); 

    #pragma endregion

    unsigned int box_texture;
    glGenTextures(1, &box_texture);

    glBindTexture(GL_TEXTURE_2D, box_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (channels == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, box_width, box_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    else if (channels == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, box_width, box_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    else {
        std::cerr << "Unsupported channel format!" << std::endl;
    }
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);


    unsigned int VBO_polygon, VAO_polygon;
    glGenBuffers(1, &VBO_polygon);
    glGenVertexArrays(1, &VAO_polygon);

    glBindVertexArray(VAO_polygon);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_polygon);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*) (3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // color
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

#pragma endregion

    Shader* polygon_shader = new Shader("shaders\\basic.vert", "shaders\\basic.frag");
    Shader* light_shader = new Shader("shaders\\light.vert", "shaders\\light.frag");
    Shader* shadow_shader = new Shader("shaders\\shadow.vert", "shaders\\shadow.frag");

    double oldTime = glfwGetTime(), newTime, deltaTime;

    Light lights[10];
    int lights_count = 3;
    /*lights[0].type = LightType::Point;
    lights[0].position = glm::vec3(0.0f, 0.0f, 0.0f);
    lights[0].ambient = glm::vec3(0.3f, 0.3f, 0.3f);
    lights[0].diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    lights[0].specular = glm::vec3(1.0f, 1.0f, 1.0f);
    lights[0].constant = 1.0f;
    lights[0].linear = 0.1f;
    lights[0].quadratic = 0.09f;*/

    lights[1].type = LightType::Directional;
    lights[1].direction = glm::vec3(-1.0f, -1.0f, -1.0f);
    lights[1].ambient = glm::vec3(0.3f, 0.3f, 0.3f);
    lights[1].diffuse = glm::vec3(0.6f, 0.85f, 1.0f);
    lights[1].specular = glm::vec3(0.0f, 0.0f, 0.0f);

    /*lights[2].type = LightType::Spot;
    lights[2].position = glm::vec3(-3.0f, -3.0f, -3.0f);
    lights[2].direction = glm::vec3(1.0f, 1.0f, 1.0f);
    lights[2].cutOff = glm::radians(10.0f);
    lights[2].ambient = glm::vec3(0.3f, 0.3f, 0.3f);
    lights[2].diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    lights[2].specular = glm::vec3(1.0f, 1.0f, 1.0f);
    lights[2].constant = 1.0f;
    lights[2].linear = 0.1f;
    lights[2].quadratic = 0.09f;*/

    while (!glfwWindowShouldClose(win))
    {
        newTime = glfwGetTime();
        deltaTime = newTime - oldTime;
        oldTime = newTime;

        processInput(win, deltaTime);

        lightTrans.position = lights[0].position;

        lights[2].position = camera.Position;
        lights[2].direction = camera.Front;

        glClearColor(background.r, background.g, background.b, background.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  


        glm::mat4 p = camera.GetProjectionMatrix();
        glm::mat4 v = camera.GetViewMatrix();
        glm::mat4 pv = p * v;

        glm::mat4 model;

        polygon_shader->use();
        

        polygon_shader->setMatrix4F("pv", pv);
        polygon_shader->setBool("wireframeMode", wireframeMode);
        polygon_shader->setVec3("viewPos", camera.Position);

        polygon_shader->setInt("lights_count", lights_count);

        Light directionalLight;

        for (int i = 0; i < lights_count; i++)
        {
            std::string num = std::to_string(i);
            switch (lights[i].type)
            {
            case LightType::Directional:
                directionalLight = lights[i];
                polygon_shader->setInt("light[" + num + "].type", int(lights[i].type));
                polygon_shader->setVec3("light[" + num + "].direction", lights[i].direction);
                polygon_shader->setVec3("light[" + num + "].ambient", lights[i].ambient);
                polygon_shader->setVec3("light[" + num + "].diffuse", lights[i].diffuse);
                polygon_shader->setVec3("light[" + num + "].specular", lights[i].specular);
                break;

            case LightType::Point:
                polygon_shader->setInt("light[" + num + "].type", int(lights[i].type));
                polygon_shader->setVec3("light[" + num + "].position", lights[i].position);
                polygon_shader->setVec3("light[" + num + "].ambient", lights[i].ambient);
                polygon_shader->setVec3("light[" + num + "].diffuse", lights[i].diffuse);
                polygon_shader->setVec3("light[" + num + "].specular", lights[i].specular);
                polygon_shader->setFloat("light[" + num + "].constant", lights[i].constant);
                polygon_shader->setFloat("light[" + num + "].linear", lights[i].linear);
                polygon_shader->setFloat("light[" + num + "].quadratic", lights[i].quadratic);
                break;

            case LightType::Spot:
                polygon_shader->setInt("light[" + num + "].type", int(lights[i].type));
                polygon_shader->setVec3("light[" + num + "].position", lights[i].position);
                polygon_shader->setVec3("light[" + num + "].direction", lights[i].direction);
                polygon_shader->setFloat("light[" + num + "].cutOff", lights[i].cutOff);
                polygon_shader->setVec3("light[" + num + "].ambient", lights[i].ambient);
                polygon_shader->setVec3("light[" + num + "].diffuse", lights[i].diffuse);
                polygon_shader->setVec3("light[" + num + "].specular", lights[i].specular);
                polygon_shader->setFloat("light[" + num + "].constant", lights[i].constant);
                polygon_shader->setFloat("light[" + num + "].linear", lights[i].linear);
                polygon_shader->setFloat("light[" + num + "].quadratic", lights[i].quadratic);
                break;
            }
        }

        glm::vec3 lightDir = glm::normalize(directionalLight.direction);
        glm::vec3 lightPos = directionalLight.position * 10.f;
        glm::mat4 DirectionProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, camera.zNear, camera.zFar);
        glm::mat4 DirectionView = glm::lookAt(lightPos, lightPos + lightDir, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 DirectionPv = DirectionProjection * DirectionView;
        shadow_shader->setMatrix4F("DirectionPv", DirectionPv);
        polygon_shader->setMatrix4F("FragPosLightSpace", DirectionPv);


        for (int i = 0; i < cube_count; i++)
        {
            model = glm::mat4(1.0f);

            model = glm::translate(model, cubeTrans[i].position);
            model = glm::rotate(model, glm::radians(cubeTrans[i].rotation.x), glm::vec3(1.f, 0.f, 0.f));
            model = glm::rotate(model, glm::radians(cubeTrans[i].rotation.y), glm::vec3(0.f, 1.f, 0.f));
            model = glm::rotate(model, glm::radians(cubeTrans[i].rotation.z), glm::vec3(0.f, 0.f, 1.f));
            model = glm::scale(model, cubeTrans[i].scale);

            polygon_shader->setMatrix4F("model", model);

            polygon_shader->setVec3("material.ambient", cubeMaterials[cubeMat[i]].ambient);
            polygon_shader->setVec3("material.diffuse", cubeMaterials[cubeMat[i]].diffuse);
            polygon_shader->setVec3("material.specular", cubeMaterials[cubeMat[i]].specular);
            polygon_shader->setFloat("material.shiniess", cubeMaterials[cubeMat[i]].shiniess);

            glBindTexture(GL_TEXTURE_2D, box_texture);
            glBindVertexArray(VAO_polygon);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }



        //LIGHT
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightTrans.position);
        model = glm::scale(model, lightTrans.scale);

        light_shader->setBool("wireframeMode", wireframeMode);

        light_shader->use();
        light_shader->setMatrix4F("pv", pv);
        light_shader->setMatrix4F("model", model);
        light_shader->setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

        glBindVertexArray(VAO_polygon);
        glDrawArrays(GL_TRIANGLES, 0, 36);
         
        /*glViewport(0, 0, 1280, 720); 
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO); 
        glClear(GL_DEPTH_BUFFER_BIT); 
        shadow_shader->use();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);*/


        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    delete polygon_shader;
    delete light_shader;

    glfwTerminate();
    return 0;
}