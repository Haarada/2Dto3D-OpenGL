//#define GLEW_STATIC



#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Camera.h"
#include "ImageData.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

//#include "offsite/glm/gtx/transform.hpp"
//GLFW_KEY_UP
struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1 
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;

            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }

        else
        {
            // probably crash
            ss[(int)type] << line << "\n";
        }
    }
    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char)); // Dynamic memory allocation on stack - as opposed to heap (malloc.h)
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader! \n";
        std::cout << "error message:\n";
        std::cout << message;
        std::cout << "End of error message\n";

        glDeleteShader(id);
        return 0;
    }



    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    //GLuint zamiast unsigned int
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;

}


int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
     
    
    int windowWidth = 1280;
    int windowHeight = 720;
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL renderer", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); // disabling V-sync

    /* GLEW initialization */
    if (glewInit() != GLEW_OK)
        std::cout << "GLEW init error!\n";

    std::cout << glGetString(GL_VERSION) << "\n";
    
    std::string MapColor = "resources\\maps\\color.png";
    std::string MapHeight = "resources\\maps\\height.png";

    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);


    ImageData Map(MapColor, MapHeight);
    Map.LoadImageColor();
    Map.LoadImageDepth();
    Map.GeneratePositionsVec();
    Map.GeneratePositionsPtr();
    Map.GenerateIndiecies();
    Map.GenerateColor();

    float* colors = Map.GetColorPtr();
    float* positions = Map.GetPositionPtr();
    unsigned int* indices = Map.GetIndiecies();


    /* Buffer */
    /*
    float positions[] = {
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, 0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f

    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    */
    unsigned int vao; // verex array object id
    GLCall(glGenVertexArrays(1, &vao));
    GLCall(glBindVertexArray(vao));

    VertexBuffer vb(positions, Map.GetPositionPtrSize() * sizeof(float));
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
    
    unsigned int number = 1024 * 1024*6 -2048*6;
    IndexBuffer ib(indices, number);

    // GLSL GL shading language
    ShaderProgramSource source = ParseShader("resources/shaders/basicshader.shader");
    
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);
    
    glUseProgram(0);
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    /* \/ Camera \/ */
    glm::mat4 Projection;
    // Camera matrix
    glm::mat4 View;
    // Model matrix : an identity matrix (model will be at the origin)
    glm::mat4 Model = glm::mat4(1.0f);
    

    unsigned int MatrixMVP = glGetUniformLocation(shader, "MVP");

    unsigned int colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, Map.GetPositionPtrSize() * sizeof(float), colors, GL_STATIC_DRAW);
    
    Camera mainCamera(window);
    mainCamera.CheckTime();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mainCamera.CheckTime();
        mainCamera.CheckFuncKeys();
        //mainCamera.CalculateMovement();

        Projection = mainCamera.ProjectionMatrix();
        View = mainCamera.ViewMatrix();
        Model = glm::mat4(1.0);
        glm::mat4 mvp = Projection * View * Model; 

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


        glUseProgram(shader);

        glUniformMatrix4fv(MatrixMVP, 1, GL_FALSE, &mvp[0][0]);
        
        GLCall(glBindVertexArray(vao));
        ib.Bind();

        GLCall(glDrawElements(GL_TRIANGLES, number, GL_UNSIGNED_INT, nullptr));
        //glDrawArrays(GL_TRIANGLES, 0, 10000);
        //glDrawElements(GL_TRIANGLES, )

        glDisableVertexAttribArray(1);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}