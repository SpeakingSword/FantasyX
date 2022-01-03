//#define DEMO
#ifdef DEMO
// 标准库
#include <iostream>
#include <vector>
#include <random>
#include <direct.h>

// 引擎头文件
#include "camera.h"
#include "model.h"
#include "shader_old.h"

// 第三方库
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// 系统特定库
#include <Windows.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 渲染引擎内置API的定义（函数/方法）
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void initVAOWithTexCoods(unsigned int* VAO, unsigned* VBO, unsigned* EBO, float* vtx, int v_s, unsigned int* indx, int i_s);
Texture initTexture2D(const char* filePath, const char* type, bool gamma);
void initCubemap(unsigned int& cubemap, vector<std::string> faces);
unsigned int* initMultiTexture2D(const char* pathArray[], int count, bool gammas[]);
void initSampler2DLoaction(Shader_Old shader, const char* textureNames[], int texCount);
unsigned int loadHDRtexture(const char* filePath);

void drawSceneNormal();

std::string getFilePath(std::string directory, std::string filename);
float ssaoScaleLerp(float a, float b, float f);

void CreateSphere(unsigned int X_SEGMENT, unsigned int Y_SEGMENT, std::vector<float>& data, std::vector<unsigned int>& indices);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 渲染引擎内置常量、类型的定义
unsigned int SCR_WIDTH = 1920;
unsigned int SCR_HEIGHT = 1080;
unsigned int SHADOW_WIDTH = 1024;
unsigned int SHADOW_HEIGHT = 1024;
unsigned int boxAmount = 1000;

char buffer[100];
std::string currentDirectory = _getcwd(buffer, sizeof(buffer));
std::string ShaderDirectory = "\\Shaders\\";
std::string ImageDirectory = "\\Images\\";
std::string ProjectDirectory = currentDirectory;
std::string ShaderPath = currentDirectory + ShaderDirectory;
std::string ImagePath = currentDirectory + ImageDirectory;

Shader_Old* pShader;
Camera camera(glm::vec3(0.0f, 2.0f, 3.0f));

float exposure = 1.0f;
float height_scale = 0.01f;
float gamma = 1.40f;
float bias = 0.0005f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastTime = 0.0f;

bool firstMouse = true;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    // GLFW: 初始化与配置
    // ------------------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // __APPLE__

    // GLFW: 创建窗口
    // -------------------------------------------------------------------------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    HDC hDC = GetDC(HWND(NULL));
    int screenWidth = GetDeviceCaps(hDC, HORZRES);
    int screenHeight = GetDeviceCaps(hDC, VERTRES);
    glfwSetWindowPos(window, (screenWidth - SCR_WIDTH) / 2, (screenHeight - SCR_HEIGHT) / 2);
    if (window == NULL)
    {
        std::cout << "Failed to Create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_move_callback);
    glfwSetScrollCallback(window, mouse_scroll_callback);

    // GLAD: 加载所有的OpenGL函数
    // -----------------------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 设置ImGui上下文
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    // 设置ImGui风格
    ImGui::StyleColorsDark();

    // 为ImGui绑定渲染平台
    const char* glsl_version = "#version 330 core";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // 创建数据
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    // 箱子顶点
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    float cubeVertices[] = {
        // 顶点位置（逆时针）	  // 纹理坐标  // 顶点法向量
        // Back face
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f, -1.0f,// Bottom-left
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, -1.0f,// top-right
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 0.0f, -1.0f,// bottom-right         
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, -1.0f,// top-right
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f, -1.0f,// bottom-left
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f, -1.0f,// top-left
        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f,  1.0f,// bottom-left
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 0.0f,  1.0f,// bottom-right
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 0.0f,  1.0f,// top-right
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 0.0f,  1.0f,// top-right
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f,  1.0f,// top-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f,  1.0f,// bottom-left
        // Left face
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,-1.0f, 0.0f,  0.0f,// top-right
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,-1.0f, 0.0f,  0.0f, // top-left
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,-1.0f, 0.0f,  0.0f, // bottom-left
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,-1.0f, 0.0f,  0.0f, // bottom-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,-1.0f, 0.0f,  0.0f, // bottom-right
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,-1.0f, 0.0f,  0.0f, // top-right
        // Right face
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f, 0.0f,  0.0f, // top-left
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, 0.0f,  0.0f,// bottom-right
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f,  0.0f,// top-right         
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, 0.0f,  0.0f,// bottom-right
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f, 0.0f,  0.0f,// top-left
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f,// bottom-left     
        // Bottom face
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, -1.0f, 0.0f,// top-right
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f, -1.0f, 0.0f,// top-left
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f, 0.0f,// bottom-left
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f, 0.0f,// bottom-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, -1.0f, 0.0f,// bottom-right
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, -1.0f, 0.0f,// top-right
        // Top face
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,// top-left
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,// bottom-right
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  1.0f, 0.0f,// top-right     
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,// bottom-right
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,// top-left
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  1.0f, 0.0f // bottom-left        
    };

    // 箱子贴图
    const char* path1 = "\\Images\\container2.png";
    const char* path2 = "\\Images\\container2_specular.png";
    const char* pathArray[] = {
        path1,
        path2
    };
    const char* textureNames[] = {
        "material.texture_diffuse1",
        "material.texture_specular1"
    };
    bool gammas[2] = {
        true,
        false
    };

    // 天空盒顶点
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    // 屏幕矩形顶点数据
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    // 四个灯光的位置
    glm::vec3 pointLightPositions[] = {
        glm::vec3(4.0f,  4.0f,  4.0f),
        glm::vec3(4.0f, -4.0f,  4.0f),
        glm::vec3(-4.0f, -4.0f,  4.0f),
        glm::vec3(-4.0f,  4.0f,  4.0f)
    };

    // 四个灯光的颜色
    glm::vec3 pointLightColors[] = {
        glm::vec3(0.1f, 0.1f, 0.1f),	// 白色（上方）
        glm::vec3(0.0f, 0.9f, 1.0f),	// 蓝色
        glm::vec3(1.0f, 0.0f, 0.4f),	// 淡紫色
        glm::vec3(1.0f, 1.0f, 1.0f),	// 白色（前方）
    };

    // 加载编译着色器程序，并初始化一些着色器uniform变量
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    /*
    // 默认着色器
    Shader_Old shader(getFilePath(ShaderPath, "default.vert").c_str(), getFilePath(ShaderPath, "default.frag").c_str());
    pShader = &shader;
    // 加载纹理
    int texCount = sizeof(pathArray) / sizeof(unsigned int);
    unsigned int *texIDs = initMultiTexture2D(pathArray, texCount, gammas);
    initSampler2DLoaction(shader, textureNames, texCount);
    shader.use();
    shader.setFloat("material.shininess", 64.0f);
    shader.setFloat("bias", bias);
    shader.setBool("blinnLightOn", true);
    shader.setBool("diffuseOn", true);
    shader.setBool("specularOn", true);
    shader.setBool("discardEdgeOn", true);
    shader.setInt("skybox", 1);
    shader.setInt("material.texture_displacement1", 7);
    shader.setInt("pointLightShadowMap", 2);
    shader.setInt("dirLightShadowMap", 6);
    */


    // 光照处理阶段着色器
    Shader_Old lightingShader(getFilePath(ShaderPath, "lightingPass.vert").c_str(), getFilePath(ShaderPath, "lightingPass.frag").c_str());
    lightingShader.use();
    lightingShader.setInt("gPosition", 0);
    lightingShader.setInt("gNormal", 1);
    lightingShader.setInt("gAlbedoSpec", 2);
    lightingShader.setInt("ssao", 3);

    // SSAO着色器
    Shader_Old ssaoShader(getFilePath(ShaderPath, "ssaoPass.vert").c_str(), getFilePath(ShaderPath, "ssaoPass.frag").c_str());
    ssaoShader.use();
    ssaoShader.setInt("gPositionDepth", 0);
    ssaoShader.setInt("gNormal", 1);
    ssaoShader.setInt("texNoise", 2);

    // SSAO 模糊着色器
    Shader_Old ssaoBlurShader(getFilePath(ShaderPath, "ssaoBlur.vert").c_str(), getFilePath(ShaderPath, "ssaoBlur.frag").c_str());
    ssaoBlurShader.setInt("ssaoInput", 0);

    // 几何处理阶段着色器
    Shader_Old geomatryShader(getFilePath(ShaderPath, "geomatryPass.vert").c_str(), getFilePath(ShaderPath, "geomatryPass.frag").c_str());

    // 高斯模糊着色器
    Shader_Old gaussianBlur(getFilePath(ShaderPath, "gaussian_blur.vert").c_str(), getFilePath(ShaderPath, "gaussian_blur.frag").c_str());
    gaussianBlur.setInt("image", 0);

    // 灯光实体的着色器
    Shader_Old lightCubeShader(getFilePath(ShaderPath, "light.vert").c_str(), getFilePath(ShaderPath, "light.frag").c_str());

    // 屏幕矩形着色器
    Shader_Old screenShader(getFilePath(ShaderPath, "screen.vert").c_str(), getFilePath(ShaderPath, "screen.frag").c_str());
    screenShader.use();
    screenShader.setInt("screenTexture", 0);

    // PBR着色器
    int lightNum = 1;
    Shader_Old pbrShader(getFilePath(ShaderPath, "pbr_lighting.vert").c_str(), getFilePath(ShaderPath, "pbr_lighting.frag").c_str());
    pbrShader.use();
    pbrShader.setVec3("albedo", glm::vec3(0.5f, 0.0f, 0.0f));
    pbrShader.setFloat("ao", 1.0f);
    pbrShader.setInt("lightNum", lightNum);
    pbrShader.setInt("irradianceMap", 0);
    pbrShader.setInt("prefilterMap", 1);
    pbrShader.setInt("brdfLUT", 2);
    for (unsigned int i = 0; i < lightNum; ++i)
    {
        pbrShader.setVec3(("lights[" + std::to_string(i) + "].position").c_str(), pointLightPositions[i]);
        pbrShader.setVec3(("lights[" + std::to_string(i) + "].color").c_str(), glm::vec3(5.0f, 5.0f, 5.0f));
    }

    // HDR着色器
    Shader_Old hdrShader(getFilePath(ShaderPath, "hdrBox.vert").c_str(), getFilePath(ShaderPath, "hdrBox.frag").c_str());
    hdrShader.use();
    hdrShader.setInt("equirectangularMap", 0);

    // 辐照度着色器
    Shader_Old irradianceShader(getFilePath(ShaderPath, "irradianceBox.vert").c_str(), getFilePath(ShaderPath, "irradianceBox.frag").c_str());
    irradianceShader.use();
    irradianceShader.setInt("environmentMap", 0);

    // 预过滤着色器
    Shader_Old prefilterShader(getFilePath(ShaderPath, "prefilterMap.vert").c_str(), getFilePath(ShaderPath, "prefilterMap.frag").c_str());
    prefilterShader.use();
    prefilterShader.setInt("environmentMap", 0);

    // BRDF着色器
    Shader_Old brdfShader(getFilePath(ShaderPath, "BRDF.vert").c_str(), getFilePath(ShaderPath, "BRDF.frag").c_str());

    // 天空盒子着色器
    Shader_Old skyboxShader(getFilePath(ShaderPath, "skybox.vert").c_str(), getFilePath(ShaderPath, "skybox.frag").c_str());
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    /*
    // 定向光阴影贴图绘制着色器
    Shader_Old dirLightShadowShader(getFilePath(ShaderPath, "dirLightshadow.vert").c_str(), getFilePath(ShaderPath, "dirLightshadow.frag").c_str());
    // 点光阴影绘制着色器
    Shader_Old pointLightShadowShader(getFilePath(ShaderPath, "pointLightShadow.vert").c_str(),
                                    getFilePath(ShaderPath, "pointLightShadow.frag").c_str(),
                                    getFilePath(ShaderPath, "pointLightShadow.geom").c_str());
    */

    // 设置每个着色器uniform块的绑定点
    //unsigned int defaultUniformBlock = glGetUniformBlockIndex(shader.ID, "Matrices");
    unsigned int lightUniformBlock = glGetUniformBlockIndex(lightCubeShader.ID, "Matrices");
    unsigned int geomatryPassBlock = glGetUniformBlockIndex(geomatryShader.ID, "Matrices");
    unsigned int pbrShaderBlock = glGetUniformBlockIndex(pbrShader.ID, "Matrices");
    unsigned int skyboxShaderBlock = glGetUniformBlockIndex(skyboxShader.ID, "Matrices");



    // 全都绑定到0点
    //glUniformBlockBinding(shader.ID, defaultUniformBlock, 0);
    glUniformBlockBinding(lightCubeShader.ID, lightUniformBlock, 0);
    glUniformBlockBinding(geomatryShader.ID, geomatryPassBlock, 0);
    glUniformBlockBinding(pbrShader.ID, pbrShaderBlock, 0);
    glUniformBlockBinding(skyboxShader.ID, skyboxShaderBlock, 0);

    // 创建缓冲对象
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    // 创建箱子VAO、VBO、EBO

    unsigned int VBO = 0, VAO = 0, EBO = 0;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // 初始化VAO
    initVAOWithTexCoods(&VAO, &VBO, &EBO, cubeVertices, sizeof(cubeVertices), indices, sizeof(indices));
    // 声明箱子的模型矩阵和法线矩阵VBO
    /*
    glBindVertexArray(VAO);
    unsigned int boxInstanceVBO;
    glGenBuffers(1, &boxInstanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, boxInstanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * boxAmount * 2, NULL, GL_DYNAMIC_DRAW);
    for (unsigned int i = 5; i < 9; i++)
    {
        glVertexAttribPointer(i, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)((i - 5) * sizeof(glm::vec4)));
        glEnableVertexAttribArray(i);
        glVertexAttribDivisor(i, 1);
    }
    for (unsigned int i = 9; i < 13; i++)
    {
        glVertexAttribPointer(i, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)((i - 9) * sizeof(glm::vec4) + boxAmount * sizeof(glm::mat4)));
        glEnableVertexAttribArray(i);
        glVertexAttribDivisor(i, 1);
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // 各个木箱的位置(小行星带)
    glm::mat4 *boxIntanceMats;
    boxIntanceMats = new glm::mat4[2 * boxAmount];
    srand(glfwGetTime());
    float radius = 10.0f;
    float boxOffset = 5.0f;
    for (unsigned int i = 0; i < boxAmount; i++)
    {
        glm::mat4 model;
        // 位移
        float angle = (float)i / (float)boxAmount * 360.0f;
        float displacement = (rand() % (int)(2 * boxOffset * 100)) / 100.0f - boxOffset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * boxOffset * 100)) / 100.0f - boxOffset;
        float y = displacement * 0.4f;
        displacement = (rand() % (int)(2 * boxOffset * 100)) / 100.0f - boxOffset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));
        // 缩放
        float scale = (rand() % 20) / 100.0f + 0.05;
        model = glm::scale(model, glm::vec3(scale));
        // 旋转
        float rotateAngle = (rand() % 360);
        model = glm::rotate(model, rotateAngle, glm::vec3(0.4f, 0.6f, 0.8f));
        // 添加到模型矩阵数组中
        boxIntanceMats[i] = model;
        //  法线矩阵
        glm::mat4 normalMat;
        normalMat = glm::transpose(glm::inverse(model));
        boxIntanceMats[i + boxAmount] = normalMat;
    }
    // 更新箱子实例数组缓冲的数据
    glBindBuffer(GL_ARRAY_BUFFER, boxInstanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 2 * boxAmount * sizeof(glm::mat4), &boxIntanceMats[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    */

    // 屏幕矩形VAO
    unsigned int screenVAO, screenVBO;
    glGenVertexArrays(1, &screenVAO);
    glGenBuffers(1, &screenVBO);
    glBindVertexArray(screenVAO);
    glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // CUBE VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // 创建ShaderUniform缓冲对象并绑定到对应的绑定点
    unsigned int uboMatrices;
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);	// 仅仅开辟内存没有填充
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboMatrices);	// 绑定到0点

    // 生成球体数据
    /*
    unsigned int x_segment = 64;
    unsigned int y_segment = 64;
    std::vector<float> sphereData;
    std::vector<unsigned int> sphereIndices;
    CreateSphere(x_segment, y_segment, sphereData, sphereIndices);
    if (sphereData.size() > 0)
    {
        std::cout << "Sphere created success!" << std::endl;
    }
    // 球体VAO
    unsigned int sphereVAO, sphereVBO, sphereEBO;
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);
    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sphereData.size() * sizeof(float), &sphereData[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), &sphereIndices[0], GL_STATIC_DRAW);
    float stride = (3 + 2 + 3) * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    */


    // 创建一个多重采样帧缓冲
    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    /*
    unsigned int multisampFrameBuffer;
    glGenFramebuffers(1, &multisampFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, multisampFrameBuffer);
    // 添加两个颜色附件
    unsigned int multisamBuffers[2];
    glGenTextures(2, multisamBuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multisamBuffers[i]);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, GL_TRUE);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, multisamBuffers[i], 0);
    }
    // 添加渲染缓冲对象附件（包含深度缓冲和模板缓冲）
    unsigned int multiRenderBuffer;
    glGenRenderbuffers(1, &multiRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, multiRenderBuffer);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, multiRenderBuffer);
    //显示告诉帧缓冲有几个附件
    unsigned int multiSampAttachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, multiSampAttachments);
    // 检查帧缓冲的完整性
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    */

    // 创建一个后期处理帧缓冲
    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    /*
    unsigned int postprocessFrameBuffer;
    glGenFramebuffers(1, &postprocessFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, postprocessFrameBuffer);
    // 创建用作后期处理的纹理帧缓冲附件
    unsigned int screenTexture;
    glGenTextures(1, &screenTexture);

    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);
    // 检查帧缓冲的完整性
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    */

    // 创建一个定向光阴影贴图绘制帧缓冲
    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    /*
    unsigned int dirLightFramebuffer;
    glGenFramebuffers(1, &dirLightFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, dirLightFramebuffer);
    // 创建定向光阴影贴图
    unsigned int dirLightShadowMap;
    glGenTextures(1, &dirLightShadowMap);
    glBindTexture(GL_TEXTURE_2D, dirLightShadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dirLightShadowMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    // 检查帧缓冲的完整性
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // 阴影立方体贴图
    unsigned int shadowCubeMap;
    glGenTextures(1, &shadowCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, shadowCubeMap);
    for (unsigned int i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    */

    // 点光源阴影绘制缓冲
    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------
    /*
    unsigned int pointLightFramebuffer;
    glGenFramebuffers(1, &pointLightFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, pointLightFramebuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowCubeMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    // 检查帧缓冲的完整性
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // 乒乓帧缓冲
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    unsigned int pingpongFBO[2];
    unsigned int pingpongBuffer[2];
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongBuffer);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    */

    // G-缓冲
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    enum GBufferTex
    {
        gPosition,
        gNormal,
        gAlbedoSpec,
        gBufferTextureNum
    };
    unsigned int gBuffer;
    unsigned int gBufferTextures[gBufferTextureNum];
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    // 位置颜色缓冲附件
    glGenTextures(1, &gBufferTextures[gPosition]);
    glBindTexture(GL_TEXTURE_2D, gBufferTextures[gPosition]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBufferTextures[gPosition], 0);
    // 法线颜色缓冲附件
    glGenTextures(1, &gBufferTextures[gNormal]);
    glBindTexture(GL_TEXTURE_2D, gBufferTextures[gNormal]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gBufferTextures[gNormal], 0);
    // 漫反射颜色和镜面颜色缓冲附件
    glGenTextures(1, &gBufferTextures[gAlbedoSpec]);
    glBindTexture(GL_TEXTURE_2D, gBufferTextures[gAlbedoSpec]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gBufferTextures[gAlbedoSpec], 0);
    // 声明帧缓冲使用的颜色附件
    unsigned int gBufferAttachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, gBufferAttachments);
    // 添加渲染缓冲对象附件（包含深度缓冲和模板缓冲)
    unsigned int gRenderBuffer;
    glGenRenderbuffers(1, &gRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, gRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gRenderBuffer);
    // 检查帧缓冲的完整性
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // SSAO 帧缓冲对象
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    unsigned int ssaoFBO;
    glGenFramebuffers(1, &ssaoFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    // SSAO帧缓冲颜色附件
    unsigned int ssaoColorBuffer;
    glGenTextures(1, &ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
    // 检查帧缓冲的完整性
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // SSAO-模糊 帧缓冲对象
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    unsigned int ssaoBlurFBO, ssaoBlurColorBuffer;
    glGenFramebuffers(1, &ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glGenTextures(1, &ssaoBlurColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoBlurColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoBlurColorBuffer, 0);
    // 检查帧缓冲的完整性
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 光照处理帧缓冲
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    unsigned int lightingFBO, lightingTexture;
    glGenFramebuffers(1, &lightingFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, lightingFBO);
    glGenTextures(1, &lightingTexture);
    glBindTexture(GL_TEXTURE_2D, lightingTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightingTexture, 0);
    // 检查帧缓冲的完整性
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 正向渲染帧缓冲
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    unsigned int forwardFBO;
    glGenFramebuffers(1, &forwardFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, forwardFBO);
    // 帧缓冲颜色纹理
    unsigned int forwardFboColor;
    glGenTextures(1, &forwardFboColor);
    glBindTexture(GL_TEXTURE_2D, forwardFboColor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, forwardFboColor, 0);
    // 帧缓冲渲染缓冲对象
    unsigned int forwardRBO;
    glGenRenderbuffers(1, &forwardRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, forwardRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, forwardRBO);
    // 检查帧缓冲的完整性
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // HDR立方体贴图生成帧缓冲
    unsigned int hdrWidth = 1024;
    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);
    // 添加渲染缓冲对象
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, hdrWidth, hdrWidth);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 初始化灯光
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    /*
    shader.use();
    glm::vec3 dirLightPos(10.0f, 10.0f, -10.0f);
    // 定向光
    shader.setFloat("dirLight.strength", 0.5f);
    shader.setVec3("dirLight.direction", (glm::vec3(0.0f) - dirLightPos));
    shader.setVec3("dirLight.ambient", glm::vec3(0.4f));
    shader.setVec3("dirLight.diffuse", glm::vec3(0.8f));
    shader.setVec3("dirLight.specular", glm::vec3(0.5f));
    // 点光
    for (unsigned int i = 0; i < sizeof(pointLightPositions) / sizeof(glm::vec3); i++)
    {
        std::string num;
        std::stringstream ss;
        ss << i;
        num = ss.str();
        shader.setFloat("pointLights[" + num + "].strength", 4.0f);
        shader.setVec3("pointLights[" + num + "].position", pointLightPositions[i]);
        shader.setVec3("pointLights[" + num + "].ambient", pointLightColors[i]);
        shader.setVec3("pointLights[" + num + "].diffuse", pointLightColors[i]);
        shader.setVec3("pointLights[" + num + "].specular", pointLightColors[i]);
        shader.setFloat("pointLights[" + num + "].constant", 1.0f);
        shader.setFloat("pointLights[" + num + "].linear", 0.22f);
        shader.setFloat("pointLights[" + num + "].quadratic", 0.20f);
    }
    // 聚光
    shader.setFloat("spotLight.strength", 1.0f);
    shader.setVec3("spotLight.position", camera.Position);
    shader.setVec3("spotLight.direction", camera.Front);
    shader.setVec3("spotLight.ambient", glm::vec3(0.0f));
    shader.setVec3("spotLight.diffuse", glm::vec3(1.6f));
    shader.setVec3("spotLight.specular", glm::vec3(1.6f));
    shader.setFloat("spotLight.constant", 1.0f);
    shader.setFloat("spotLight.linear", 0.09f);
    shader.setFloat("spotLight.quadratic", 0.032f);
    shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(30.0f)));
    */

    // 初始化纹理贴图
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    // 天空盒立方体六面贴图
    /*
    vector<std::string> skyboxFaces
    {
        "D:\\Visual Studio 2017 Projects\\OpenGLdemo\\OpenGLdemo\\Images\\Skybox\\mountain_and_lake\\right.jpg",
        "D:\\Visual Studio 2017 Projects\\OpenGLdemo\\OpenGLdemo\\Images\\Skybox\\mountain_and_lake\\left.jpg",
        "D:\\Visual Studio 2017 Projects\\OpenGLdemo\\OpenGLdemo\\Images\\Skybox\\mountain_and_lake\\top.jpg",
        "D:\\Visual Studio 2017 Projects\\OpenGLdemo\\OpenGLdemo\\Images\\Skybox\\mountain_and_lake\\bottom.jpg",
        "D:\\Visual Studio 2017 Projects\\OpenGLdemo\\OpenGLdemo\\Images\\Skybox\\mountain_and_lake\\front.jpg",
        "D:\\Visual Studio 2017 Projects\\OpenGLdemo\\OpenGLdemo\\Images\\Skybox\\mountain_and_lake\\back.jpg"
    };
    //加载天空盒纹理
    unsigned int skyboxTexture;
    initCubemap(skyboxTexture, skyboxFaces);
    */

    // 加载地板的置换贴图
    //unsigned int planeDepthMap;
    //initTexture2D(planeDepthMap, "D:\\OpenGL3DModels\\ground\\wood_disp.png", false);

    // 加载材质球贴图
    Texture albedoMap, normalMap, metallicMap, roughnessMap, aoMap;
    albedoMap = initTexture2D("D:/OpenGL3DModels/Cerberus/Textures/Cerberus_A.tga", "texture_diffuse", true);
    normalMap = initTexture2D("D:/OpenGL3DModels/Cerberus/Textures/Cerberus_N.tga", "texture_normal", false);
    metallicMap = initTexture2D("D:/OpenGL3DModels/Cerberus/Textures/Cerberus_M.tga", "texture_metallic", false);
    roughnessMap = initTexture2D("D:/OpenGL3DModels/Cerberus/Textures/Cerberus_R.tga", "texture_roughness", false);
    aoMap = initTexture2D("D:/OpenGL3DModels/Cerberus/Textures/Cerberus_AO.tga", "texture_ao", true);

    // 加载HDR格式贴图
    unsigned int hdrTexture = loadHDRtexture("D:/Visual Studio 2017 Projects/FantasyX/FantasyX/Images/HDR/Factory_Catwalk/Factory_Catwalk_2k.hdr");

    // HDR 立方体贴图
    unsigned int envCubeMap;
    glGenTextures(1, &envCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, hdrWidth, hdrWidth, 0, GL_RGB, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    // 卷积后生成的辐照度贴图
    unsigned int irradianceMap;
    unsigned int irradianceMapWidth = 128;
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, irradianceMapWidth, irradianceMapWidth, 0, GL_RGB, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    // 预滤波镜面环境贴图
    unsigned int prefilterMap;
    unsigned int prefilterMapWidth = 128;
    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, prefilterMapWidth, prefilterMapWidth, 0, GL_RGB, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 生成多级渐远纹理
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    // BRDF贴图
    unsigned int brdfLUTTexture;
    glGenTextures(1, &brdfLUTTexture);
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // 加载模型
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    //Model man("D:/OpenGL3DModels/nanosuit/nanosuit.obj");
    //Model matilda("D:/OpenGL3DModels/matilda/matilda.obj");
    //Model plane("D:/OpenGL3DModels/ground/plane.obj");
    //plane.AddTexture(planeDepthMap, "texture_displacement", "D:\\OpenGL3DModels\\ground\\wood_disp.png");
    Model mModel("D:/OpenGL3DModels/Cerberus/fire_gun.obj");
    mModel.AddTexture(albedoMap);
    mModel.AddTexture(normalMap);
    mModel.AddTexture(metallicMap);
    mModel.AddTexture(roughnessMap);
    mModel.AddTexture(aoMap);

    // 辅助数据和操作
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    // 声明计时器
    float timer = 1.0f;
    camera.MovementSpeed = 5.0f;
    camera.ZoomSpeed = 1.0f;
    camera.Near_Plane = 0.1f;
    camera.Far_Plane = 1000.0f;

    // 生成光的位置
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
    float lightMax = 1.0f;
    float radius = (-linear + std::sqrtf(linear * linear - 4 * quadratic * (constant - (256.0 / 5.0) * lightMax))) / (2 * quadratic);
    vector<glm::vec3> lightPositions;
    lightingShader.use();
    int mCount = 0;
    for (int i = -6; i <= 6; i += 4)
    {
        if (i != 0)
        {
            lightPositions.push_back(glm::vec3(i, 10.0f, i));
            lightingShader.setVec3("lights[" + std::to_string(mCount) + "].position", glm::vec3(i, 10.0f, i));
            lightingShader.setFloat("lights[" + std::to_string(mCount) + "].radius", radius);
            lightingShader.setVec3("lights[" + std::to_string(mCount) + "].color", glm::vec3(1.0f, 1.0f, 1.0f));
            lightingShader.setFloat("lights[" + std::to_string(mCount) + "].constant", constant);
            lightingShader.setFloat("lights[" + std::to_string(mCount) + "].linear", linear);
            lightingShader.setFloat("lights[" + std::to_string(mCount) + "].quadratic", quadratic);
            mCount += 1;
            lightPositions.push_back(glm::vec3(i, 10.0f, -i));
            lightingShader.setVec3("lights[" + std::to_string(mCount) + "].position", glm::vec3(i, 10.0f, -i));
            lightingShader.setFloat("lights[" + std::to_string(mCount) + "].radius", radius);
            lightingShader.setVec3("lights[" + std::to_string(mCount) + "].color", glm::vec3(1.0f, 1.0f, 1.0f));
            lightingShader.setFloat("lights[" + std::to_string(mCount) + "].constant", constant);
            lightingShader.setFloat("lights[" + std::to_string(mCount) + "].linear", linear);
            lightingShader.setFloat("lights[" + std::to_string(mCount) + "].quadratic", quadratic);
            mCount += 1;
        }
    }
    std::cout << "light radius: " << radius << std::endl;

    // 生成SSAO半球形样本
    int kernelSize = 24;
    float kernelRadius = 0.5f;
    int occlusionPower = 2.0f;
    ssaoShader.use();
    ssaoShader.setInt("kernelSize", kernelSize);
    ssaoShader.setFloat("radius", kernelRadius);
    ssaoShader.setInt("power", occlusionPower);
    std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);		// (0.0, 1.0)之间的随机浮点数
    std::default_random_engine generator;
    std::vector<glm::vec3> ssacoKernel;
    for (unsigned int i = 0; i < kernelSize; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = (float)i / kernelSize;
        scale = ssaoScaleLerp(0.1f, 1.0f, scale * scale);		// 对样本位置作二次曲线缩放
        ssacoKernel.push_back(sample);
        // 传送数据到着色器
        ssaoShader.setVec3(("samples[" + std::to_string(i) + "]").c_str(), sample);
    }
    // 生成SSAO噪声图像（保存片段的旋转向量）
    std::vector<glm::vec3> ssaoNoise;
    for (unsigned int i = 0; i < 16; ++i)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator) * 2.0f - 1.0f, 0.0f);
        ssaoNoise.push_back(noise);
    }
    // 将向量保存到纹理中
    unsigned int noiseTexture;
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);

    // 创建定向光光源空间变换矩阵
    /*
    float dirLightNear = 0.1f;
    float dirLightFar = 100.0f;
    glm::mat4 dirLightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, dirLightNear, dirLightFar);
    glm::mat4 dirLightView = glm::lookAt(dirLightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 dirLightSpaceMatrix = dirLightProjection * dirLightView;
    dirLightShadowShader.use();
    dirLightShadowShader.setMat4("lightSpaceMatrix", dirLightSpaceMatrix);
    shader.use();
    shader.setMat4("dirlightSpaceMatrix", dirLightSpaceMatrix);
    */

    // 点光源空间变换矩阵
    /*
    glm::vec3 pointLighPos = pointLightPositions[1];
    float pointLightAspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
    float pointLightNear = 0.1f;
    float pointLightFar = 100.0f;
    glm::mat4 pointLightShadowProj = glm::perspective(glm::radians(90.0f), pointLightAspect, pointLightNear, pointLightFar);
    std::vector<glm::mat4> shadowTransforms;
    shadowTransforms.push_back(pointLightShadowProj *
        glm::lookAt(pointLighPos, pointLighPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(pointLightShadowProj *
        glm::lookAt(pointLighPos, pointLighPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(pointLightShadowProj *
        glm::lookAt(pointLighPos, pointLighPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
    shadowTransforms.push_back(pointLightShadowProj *
        glm::lookAt(pointLighPos, pointLighPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
    shadowTransforms.push_back(pointLightShadowProj *
        glm::lookAt(pointLighPos, pointLighPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(pointLightShadowProj *
        glm::lookAt(pointLighPos, pointLighPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
    pointLightShadowShader.use();
    for (unsigned int i = 0; i < 6; i++)
    {
        pointLightShadowShader.setMat4(("shadowMatrices[" + std::to_string(i) + "]").c_str(), shadowTransforms[i]);
    }
    */

    // HDR 立方图 渲染空间变换矩阵
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    // 渲染HDR环境立方体贴图(真正的HDR)
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    hdrShader.use();
    hdrShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glViewport(0, 0, hdrWidth, hdrWidth);
    glEnable(GL_DEPTH_TEST);


    glBindVertexArray(cubeVAO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        hdrShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubeMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // 生成多级渐远纹理
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    // 利用HDR贴图卷积生成辐照度贴图
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, irradianceMapWidth, irradianceMapWidth);

    irradianceShader.use();
    irradianceShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);

    glViewport(0, 0, irradianceMapWidth, irradianceMapWidth);
    glEnable(GL_DEPTH_TEST);

    glBindVertexArray(cubeVAO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        irradianceShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // 生成预过滤HDR环境贴图
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);

    prefilterShader.use();
    prefilterShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);

    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(cubeVAO);

    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        // 根据mipmap等级重设帧缓冲的大小
        unsigned int mipWidth = prefilterMapWidth * std::pow(0.5f, mip);
        unsigned int mipHeight = prefilterMapWidth * std::pow(0.5f, mip);

        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilterShader.setFloat("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            prefilterShader.setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // 生成BRDF贴图
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

    glViewport(0, 0, 512, 512);
    glEnable(GL_DEPTH_TEST);

    brdfShader.use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(screenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // OpenGL模式
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    //glEnable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // 只生成一次阴影贴图标记
    bool oneceDirShadow = false;
    bool onecePointShadow = false;
    bool ssaoOn = false;

    // 渲染循环
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        // 计算每一帧的渲染时间(deltaTime)
        float currenFrame = glfwGetTime();
        deltaTime = currenFrame - lastFrame;
        lastFrame = currenFrame;

        if (timer >= 1.0f)
        {
            std::cout << "FPS: " << 1 / deltaTime << std::endl;
            timer = 0.0f;
        }
        else
            timer += deltaTime;

        // 检测输入
        processInput(window);

        // 更新着色器变量的值(单独设置)
        /*
        shader.use();
        shader.setFloat("bias", bias);
        shader.setBool("containTexture", true);
        shader.setVec3("spotLight.position", camera.Position);
        shader.setVec3("spotLight.direction", camera.Front);
        shader.setVec3("viewPos", camera.Position);	// 更新相机位置
        shader.setVec3("lightPos", pointLighPos);
        shader.setFloat("pointLight_far_plane", pointLightFar);
        */

        // 计算模型、观察、投影矩阵
        glm::mat4 view, model, projection, normalMat;
        view = camera.lookAt(camera.Position, camera.Front + camera.Position, camera.Up);
        projection = glm::perspective(glm::radians(camera.Fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, camera.Near_Plane, camera.Far_Plane);

        // 使用Uniform块更新着色器变量的值
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        /*
        // 渲染到G缓冲（也称几何渲染阶段）
        // ----------------------------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        geomatryShader.use();
        geomatryShader.setVec3("material.color", glm::vec3(0.6f, 0.6f, 0.6f));
        geomatryShader.setFloat("NEAR", camera.Near_Plane);
        geomatryShader.setFloat("FAR", camera.Far_Plane);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // 绘制Matilda
        geomatryShader.setBool("diffuseMapOn", true);
        //geomatryShader.setBool("normalMapOn", true);
        //geomatryShader.setBool("specularMapOn", true);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.025f));
        normalMat = glm::transpose(glm::inverse(view * model));
        geomatryShader.setMat4("model", model);
        geomatryShader.setMat4("normalMat", normalMat);
        matilda.Draw(geomatryShader);
        model = glm::mat4(1.0f);
        geomatryShader.setBool("diffuseMapOn", false);
        //geomatryShader.setBool("normalMapOn", false);
        //geomatryShader.setBool("specularMapOn", false);
        // 绘制地板
        geomatryShader.setBool("diffuseMapOn", true);
        geomatryShader.setBool("normalMapOn", true);
        geomatryShader.setBool("specularMapOn", true);
        model = glm::translate(model, glm::vec3(0.0f, -0.02f, 0.0f));
        model = glm::scale(model, glm::vec3(15.0f));
        normalMat = glm::transpose(glm::inverse(view * model));
        geomatryShader.setMat4("model", model);
        geomatryShader.setMat4("normalMat", normalMat);
        plane.Draw(geomatryShader);
        model = glm::mat4(1.0f);
        geomatryShader.setBool("diffuseMapOn", false);
        geomatryShader.setBool("normalMapOn", false);
        geomatryShader.setBool("specularMapOn", false);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // SSAO阶段
        // ----------------------------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------------------------
        if (ssaoOn)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
            glClear(GL_COLOR_BUFFER_BIT);
            ssaoShader.use();
            ssaoShader.setMat4("projection", projection);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, gBufferTextures[gPosition]);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, gBufferTextures[gNormal]);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, noiseTexture);
            glBindVertexArray(screenVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        // SSAO-模糊阶段
        // ----------------------------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------------------------
        if (ssaoOn)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
            glClear(GL_COLOR_BUFFER_BIT);
            ssaoBlurShader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
            glBindVertexArray(screenVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        */
        /*
        // 使用点光源深度贴图绘制帧缓冲
        // ----------------------------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------------------------
        // 只渲染点光阴影贴图一次
        if (!oneceDirShadow)
        {
            std::cout << "Point lightmap rendering ..." << std::endl;
            glBindFramebuffer(GL_FRAMEBUFFER, pointLightFramebuffer);
            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glEnable(GL_DEPTH_TEST);
            glClear(GL_DEPTH_BUFFER_BIT);
            pointLightShadowShader.use();
            pointLightShadowShader.setVec3("lightPos", pointLighPos);
            pointLightShadowShader.setFloat("far_plane", pointLightFar);
            // 绘制人
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
            model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.4f));
            pointLightShadowShader.setMat4("model", model);
            man.Draw(pointLightShadowShader);
            model = glm::mat4(1.0f);
            // 绘制玛蒂尔达
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, glm::vec3(0.025f));
            pointLightShadowShader.setMat4("model", model);
            matilda.Draw(pointLightShadowShader);
            model = glm::mat4(1.0f);
            // 绘制地板
            model = glm::translate(model, glm::vec3(0.0f, -0.02f, 0.0f));
            //model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(8.0f));
            pointLightShadowShader.setMat4("model", model);
            plane.Draw(pointLightShadowShader);
            model = glm::mat4(1.0f);
            oneceDirShadow = true;
            std::cout << "Point lightmap render finished ..." << std::endl;
        }
        // 使用定向光深度贴图绘制帧缓冲
        // ----------------------------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------------------------

        // 只渲染定向光阴影贴图一次
        if (!onecePointShadow)
        {
            std::cout << "Directional lightmap rendering ..." << std::endl;
            glBindFramebuffer(GL_FRAMEBUFFER, dirLightFramebuffer);
            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glEnable(GL_DEPTH_TEST);
            glClear(GL_DEPTH_BUFFER_BIT);
            dirLightShadowShader.use();
            // 绘制人
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
            model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.4f));
            dirLightShadowShader.setMat4("model", model);
            man.Draw(dirLightShadowShader);
            model = glm::mat4(1.0f);
            // 绘制玛蒂尔达
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, glm::vec3(0.025f));
            dirLightShadowShader.setMat4("model", model);
            matilda.Draw(dirLightShadowShader);
            model = glm::mat4(1.0f);
            // 绘制地板
            model = glm::translate(model, glm::vec3(0.0f, -0.02f, 0.0f));
            //model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(8.0f));
            dirLightShadowShader.setMat4("model", model);
            plane.Draw(dirLightShadowShader);
            model = glm::mat4(1.0f);
            onecePointShadow = true;
            std::cout << "Directional lightmap render finished ..." << std::endl;
        }
        // 使用多种采样帧缓冲
        // ----------------------------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, multisampFrameBuffer);
        // 设置当前缓冲必须OpenGL状态
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_CUBE_MAP, shadowCubeMap);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, dirLightShadowMap);
        // 渲染
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();
        shader.setBool("dirLightShadowOn", true);
        shader.setBool("pointLightShadowOn", true);
        // 绘制箱子
        // 使用箱子纹理
        for (int i = 0; i < texCount; i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, texIDs[i]);
        }
        glBindVertexArray(VAO);
        shader.setBool("instancingDraw", true);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, boxAmount);
        shader.setBool("instancingDraw", false);
        // 绘制人
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.4f));
        shader.setMat4("model", model);
        normalMat = glm::transpose(glm::inverse(model));
        shader.setMat4("normalMat", normalMat);
        shader.setBool("Reflection", true);
        shader.setBool("normalMapOn", true);
        man.Draw(shader);
        shader.setBool("Reflection", false);
        shader.setBool("normalMapOn", false);
        model = glm::mat4(1.0f);
        // 绘制玛蒂尔达
        glDisable(GL_CULL_FACE);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.025f));
        shader.setMat4("model", model);
        normalMat = glm::transpose(glm::inverse(model));
        shader.setMat4("normalMat", normalMat);
        shader.setBool("specularOn", false);
        matilda.Draw(shader);
        shader.setBool("specularOn", true);
        model = glm::mat4(1.0f);
        glEnable(GL_CULL_FACE);
        // 绘制灯光
        glBindVertexArray(lightVAO);
        lightShader.use();
        for (unsigned int i = 0; i < 4; i++)
        {
            glm::mat4 lightModel;
            lightModel = glm::translate(lightModel, pointLightPositions[i]);
            lightModel = glm::scale(lightModel, glm::vec3(0.2f));
            lightShader.setMat4("model", lightModel);
            lightShader.setVec3("lightColor", pointLightColors[i]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        shader.use();
        // 绘制地板
        model = glm::translate(model, glm::vec3(0.0f, -0.02f, 0.0f));
        model = glm::scale(model, glm::vec3(8.0f));
        shader.setMat4("model", model);
        normalMat = glm::transpose(glm::inverse(model));
        shader.setBool("normalMapOn", true);
        shader.setBool("displacementMapOn", true);
        shader.setBool("discardEdgeOn", false);
        shader.setFloat("height_scale", height_scale);
        plane.Draw(shader);
        shader.setBool("discardEdgeOn", true);
        shader.setBool("displacementMapOn", false);
        shader.setBool("normalMapOn", false);
        model = glm::mat4(1.0f);
        */



        /*
        // 将多重采样帧缓冲的绘制结果还原到后期处理缓冲里
        glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampFrameBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postprocessFrameBuffer);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glReadBuffer(GL_COLOR_ATTACHMENT1);
        glDrawBuffer(GL_COLOR_ATTACHMENT1);
        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        // 使用乒乓缓冲对高亮图像做高斯模糊处理
        // ----------------------------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------------------------
        bool horizontal = true, first_blur = true;
        int blur_time = 10;
        gaussianBlur.use();
        for (unsigned int i = 0; i < blur_time; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            gaussianBlur.setBool("horizontal", horizontal);
            glActiveTexture(GL_TEXTURE0);
            // 第一次使用生成的高亮图
            glBindTexture(GL_TEXTURE_2D, first_blur ? screenTextures[1] : pingpongBuffer[!horizontal]);
            // 开始处理
            glBindVertexArray(screenVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
            horizontal = !horizontal;
            if (first_blur)
                first_blur = false;
        }
        */
        /*
        // 光照处理阶段
        // ----------------------------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, lightingFBO);
        glClearColor(0.3f, 0.3f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        lightingShader.use();
        lightingShader.setBool("ssaoOn", ssaoOn);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gBufferTextures[gPosition]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gBufferTextures[gNormal]);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gBufferTextures[gAlbedoSpec]);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, ssaoBlurColorBuffer);
        glBindVertexArray(screenVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        */
        // 正向渲染部分
        // ----------------------------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, forwardFBO);

        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 帧缓冲之间的数据传送
        // ----------------------------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------------------------
        /*
        glBindFramebuffer(GL_READ_FRAMEBUFFER, lightingFBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, forwardFBO);
        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, forwardFBO);
        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        */

        /*
        lightCubeShader.use();
        glBindVertexArray(cubeVAO);
        for (unsigned int i = 0; i < lightPositions.size(); i++)
        {
            glm::mat4 lightModel;
            lightModel = glm::translate(lightModel, lightPositions[i]);
            lightModel = glm::scale(lightModel, glm::vec3(0.2f));
            lightCubeShader.setMat4("model", lightModel);
            lightCubeShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        */


        // 使用PBR渲染器渲染
        pbrShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
        pbrShader.setVec3("viewPos", camera.Position);

        model = glm::translate(model, glm::vec3(0.0f));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(4.0f));
        normalMat = glm::transpose(glm::inverse(model));
        pbrShader.setMat4("model", model);
        pbrShader.setMat4("normalMat", normalMat);
        mModel.Draw(pbrShader);
        model = glm::mat4(1.0f);

        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS);


        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        // 后期处理阶段
        // ----------------------------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glDisable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 将多缓冲的绘制结果呈现出来
        screenShader.use();
        screenShader.setFloat("exposure", exposure);		// 更新曝光值
        screenShader.setFloat("gamma", gamma);				// 更新GAMMA值
        glBindVertexArray(screenVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, forwardFboColor);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // GLFW：检测IO事件 (键盘的按下与释放、鼠标移动等等)
        glfwPollEvents();

        // 开始绘制ImGui内容
        // ----------------------------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------------------------
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 显示DEMO窗口
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        {
            static float f = 0.0f;
            static int imgui_counter = 0;

            ImGui::Begin("Hello World!");

            ImGui::Text("This is some useful text.");
            ImGui::Checkbox("Demo Window", &show_demo_window);
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
            ImGui::ColorEdit3("clear color", (float*)&clear_color);

            if (ImGui::Button("Button"))
                imgui_counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", imgui_counter);

            ImGui::Text("Aplication average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 显示另一个窗口
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // 渲染ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // GLFW: 交换前帧缓冲与后帧缓冲、
        glfwSwapBuffers(window);
    }

    // 可选: 释放掉不再需要的OpenGL对象
    //glDeleteVertexArrays(1, &VAO);
    //glDeleteBuffers(1, &VBO);
    //glDeleteBuffers(1, &EBO);

    // GLFW: 释放掉所有之前GLFW分配的资源
    glfwTerminate();

    return 0;
}

// 函数定义
// ------------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------------

// 鼠标移动回调函数
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT))
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        camera.ProcessMouseMovement(xoffset, yoffset, true);
    }
    else
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

}

// 鼠标滚轮回调函数
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset, deltaTime);
}

// 处理所有的输入: 检测是否有对应的键位按下并执行相关处理
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    bool pressLeftShift = false;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
        pressLeftShift = true;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, pressLeftShift ? 2 * deltaTime : deltaTime);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, pressLeftShift ? 2 * deltaTime : deltaTime);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, pressLeftShift ? 2 * deltaTime : deltaTime);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, pressLeftShift ? 2 * deltaTime : deltaTime);

    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
    {
        exposure -= 0.01f;
        if (exposure <= 0.0f)
            exposure = 0.0f;
        std::cout << "exposure: " << exposure << std::endl;
    }

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
    {
        exposure += 0.01f;
        std::cout << "exposure: " << exposure << std::endl;
    }
}

// GLFW: 窗口大小改变后执行的回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// 初始化顶点数组对象(使用索引缓冲对象，使用纹理坐标)
void initVAOWithTexCoods(unsigned int* VAO, unsigned* VBO, unsigned* EBO, float* vtx, int v_s, unsigned int* indx, int i_s)
{
    glBindVertexArray(*VAO);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, v_s, vtx, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, i_s, indx, GL_STATIC_DRAW);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 纹理坐标
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 法向量
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// 简单初始化一个纹理对象
Texture initTexture2D(const char* filePath, const char* type, bool gamma)
{
    Texture texture;

    unsigned int tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    // 加载纹理
    //stbi_set_flip_vertically_on_load(true);
    int width, height, numChannels;
    unsigned char* data = stbi_load(filePath, &width, &height, &numChannels, 0);
    if (data)
    {
        GLenum internalFormat;
        GLenum dataFormat;
        if (numChannels == 1)
            internalFormat = dataFormat = GL_RED;
        else if (numChannels == 3)
        {
            internalFormat = gamma ? GL_SRGB : GL_RGB;
            dataFormat = GL_RGB;
        }
        else if (numChannels == 4)
        {
            internalFormat = gamma ? GL_SRGB_ALPHA : GL_RGBA;
            dataFormat = GL_RGBA;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // 为当前绑定的纹理对象设置环绕、过滤方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        texture.id = tex;
        texture.path = filePath;
        texture.type = type;
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
        std::cout << filePath << std::endl;
    }

    stbi_image_free(data);

    return texture;
}

// 初始化立方体纹理贴图对象
void initCubemap(unsigned int& cubemap, vector<std::string> faces)
{
    glGenTextures(1, &cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

    //stbi_set_flip_vertically_on_load(true);
    int width, height, numChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &numChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;

        }
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

//  加载一张HDR格式图片
unsigned int loadHDRtexture(const char* filePath)
{
    stbi_set_flip_vertically_on_load(true);
    int width, height, numComponent;
    float* data = stbi_loadf(filePath, &width, &height, &numComponent, 0);
    unsigned int hdrTexture;
    if (data)
    {
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        std::cout << "Loading " << filePath << " success." << std::endl;
    }
    else
    {
        std::cout << "Failed to load HDR image." << std::endl;
        std::cout << filePath << std::endl;
    }

    stbi_image_free(data);

    return hdrTexture;
}

// 初始化并多个纹理对象并返回其引用
unsigned int* initMultiTexture2D(const char* pathArray[], int count, bool gammas[])
{
    unsigned int* texIDs = new unsigned int[count];
    glGenTextures(count, texIDs);
    for (int i = 0; i < count; i++)
    {
        glBindTexture(GL_TEXTURE_2D, texIDs[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


        int width, height, numChannels;
        std::string filePath = ProjectDirectory.c_str();
        filePath.append(pathArray[i]);

        //stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &numChannels, 0);
        if (data)
        {
            GLenum internalFormat;
            GLenum dataFormat;
            if (numChannels == 1)
                internalFormat = dataFormat = GL_RED;
            else if (numChannels == 3)
            {
                internalFormat = gammas[i] ? GL_SRGB : GL_RGB;
                dataFormat = GL_RGB;
            }
            else if (numChannels == 4)
            {
                internalFormat = gammas[i] ? GL_SRGB_ALPHA : GL_RGBA;
                dataFormat = GL_RGBA;
            }

            std::string suffix = filePath.substr(filePath.find_last_of(".") + 1);	// 求图片后缀
            std::cout << "Loading " + suffix + "..." << std::endl;

            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture " << i << std::endl;
        }
        stbi_image_free(data);
    }
    return texIDs;
}

// 初始化着色器2D纹理采样器的位置值
void initSampler2DLoaction(Shader_Old shader, const char* textureNames[], int texCount)
{
    shader.use();
    for (int i = 0; i < texCount; i++)
    {
        shader.setInt(textureNames[i], i);
    }
}

// 正常绘制场景
void drawSceneNormal()
{

}

// 获取文件路径
std::string getFilePath(std::string directory, std::string filename)
{
    return (directory + filename);
}

float ssaoScaleLerp(float a, float b, float f)
{
    return a + f * (b - a);
}

//创建并返回一个球体的顶点数据，包括位置、UV、法线以及顶点索引
//参数：
//	X_SGEMENT：球体在 X 方向上的分段
//	Y_SGEMENT：球体在 Y 方向上的分段
//	data     ：返回球体的顶点数据，包括位置、uv、法线
//	indices  ：返回顶点索引
void CreateSphere(unsigned int X_SEGMENT, unsigned int Y_SEGMENT, std::vector<float>& data, std::vector<unsigned int>& indices)
{
    const float PI = 3.14159265359;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uv;
    std::vector<glm::vec3> normals;

    for (unsigned int y = 0; y <= Y_SEGMENT; ++y)
    {
        for (unsigned int x = 0; x <= X_SEGMENT; ++x)
        {
            float xSegment = (float)x / (float)X_SEGMENT;
            float ySegment = (float)y / (float)Y_SEGMENT;
            float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = std::cos(ySegment * PI);
            float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

            positions.push_back(glm::vec3(xPos, yPos, zPos));
            uv.push_back(glm::vec2(xSegment, ySegment));
            normals.push_back(glm::vec3(xPos, yPos, zPos));
        }
    }

    bool oddRow = false;
    for (int y = 0; y < Y_SEGMENT; ++y)
    {
        if (!oddRow) // even rows: y == 0, y == 2; and so on
        {
            for (int x = 0; x <= X_SEGMENT; ++x)
            {
                indices.push_back(y * (X_SEGMENT + 1) + x);
                indices.push_back((y + 1) * (X_SEGMENT + 1) + x);
            }
        }
        else
        {
            for (int x = X_SEGMENT; x >= 0; --x)
            {
                indices.push_back((y + 1) * (X_SEGMENT + 1) + x);
                indices.push_back(y * (X_SEGMENT + 1) + x);
            }
        }
        oddRow = !oddRow;
    }

    for (int i = 0; i < positions.size(); ++i)
    {
        data.push_back(positions[i].x);
        data.push_back(positions[i].y);
        data.push_back(positions[i].z);
        if (uv.size() > 0)
        {
            data.push_back(uv[i].x);
            data.push_back(uv[i].y);
        }
        if (normals.size() > 0)
        {
            data.push_back(normals[i].x);
            data.push_back(normals[i].y);
            data.push_back(normals[i].z);
        }
    }
}

#endif