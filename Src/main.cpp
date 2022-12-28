#include <stdio.h>
#include <Windows.h>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/common.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glad.c"
#include "./shader.glsl"
#define RJSON_IMPLEMENTATION
#include "rj_obj.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Image.h"

typedef unsigned int UI;
UI Program;

struct VecV
{
    float x;
    float y;
    float z;
};


struct ball
{
    UI VAO;
    UI VBO;
    UI Texture;
    obj Shape;
};

ball DrawBall(const char* ObjFilePath,const char* texturePath)
{
    ball Result = {};
    HANDLE File = CreateFileA(ObjFilePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    LARGE_INTEGER Size = {};
    GetFileSizeEx(File, &Size);

    if(File)
    {
        printf("File is ok\n");
    }
    else
    {
        printf("File loading Error\n");
    }

    char *Buffer = (char *)malloc(Size.QuadPart);
    ReadFile(File, Buffer, Size.QuadPart, 0, 0);

    Result.Shape = ParseObj((u8 *)Buffer, Size.QuadPart);

    Result.VAO;
    Result.VBO;
    glGenVertexArrays(1, &Result.VAO);
    glBindVertexArray(Result.VAO);
    glGenBuffers(1, &Result.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, Result.VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (Result.Shape.VertexCount * 3 + Result.Shape.NormalCount * 3 + Result.Shape.TexCount * 2), 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * (Result.Shape.VertexCount * 3), Result.Shape.Vertex);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * (Result.Shape.VertexCount * 3), sizeof(float) * (Result.Shape.TexCount * 2), Result.Shape.TexCoords);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * (Result.Shape.VertexCount * 3 + Result.Shape.TexCount * 2), sizeof(float) * (Result.Shape.NormalCount * 3),Result.Shape.Normals);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, 0, 0, (void *)(sizeof(float) * (Result.Shape.VertexCount * 3)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, 0, 0, (void *)(sizeof(float) * (Result.Shape.VertexCount * 3 + Result.Shape.TexCount * 2)));

    glGenTextures(1, &Result.Texture);
    glBindTexture(GL_TEXTURE_2D, Result.Texture);
    int x,y,comp;
    stbi_uc* data = stbi_load(texturePath,&x,&y,&comp,0);
    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,x,y,0,GL_RGB,GL_UNSIGNED_BYTE,data);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else{
        printf("error in texture loading\n");
    }
    stbi_image_free(data);

    return Result;
};

void DrawSphere(float O,float x,float y ,float z,ball b)
{
    glBindVertexArray(b.VAO);
    glBindBuffer(GL_VERTEX_ARRAY,b.VBO);
    glBindTexture(GL_TEXTURE_2D,b.Texture);

    glm::mat4 Model(1.0f);
    Model = glm::translate(Model,glm::vec3(x,y,z));
    // Model = glm::rotate(Model, (45.0f * 3.14f) / 180.0f, glm::vec3(0, 1, 0));
    glUniformMatrix4fv(glGetUniformLocation(Program, "Model"), 1, 0, glm::value_ptr(Model));
    glUniform1f(glGetUniformLocation(Program,"objInt"),O);
    glDrawArrays(GL_TRIANGLES,0,b.Shape.VertexCount);
};

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 500, "Solar System", 0, 0);
    glfwMakeContextCurrent(window);
    gladLoadGL();

    UI vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &VertexShaderSource, 0);
    glCompileShader(vertexShader);

    char infoLog[1000];
    glGetShaderInfoLog(vertexShader, 1000, 0, infoLog);

    printf("%s\n", infoLog);

    UI fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &FragmentShaderSource, 0);
    glCompileShader(fragmentShader);

    glGetShaderInfoLog(fragmentShader, 1000, 0, infoLog);

    printf("%s\n", infoLog);

    Program = glCreateProgram();
    glAttachShader(Program, vertexShader);
    glAttachShader(Program, fragmentShader);
    glLinkProgram(Program);
    glUseProgram(Program);

    glGetProgramInfoLog(Program, 1000, 0, infoLog);
    printf("%s\n", infoLog);

    glm::mat4 Proj(1.0f);
    Proj = glm::perspective((45.0f * 3.14f) / 180.0f, 800.0f / 500.0f, .1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(Program, "Proj"), 1, 0, glm::value_ptr(Proj));

    VecV Eye;
    Eye.x = 0.1f;
    Eye.y = 0.0f;
    Eye.z = -2.0f;

    VecV Center;
    Center.x = 0.0f;
    Center.y = 0.0f;
    Center.z = 0.0f;
    
    VecV Up;
    Up.x = 0.0f;
    Up.y = 2.0f;
    Up.z = 0.0f;
   
    
    ball sun = DrawBall("../dp/earth.obj","../texture/8k_sun.jpg");
    ball mercury = DrawBall("../dp/earth.obj","../texture/2k_mercury.jpg");
    ball venus = DrawBall("../dp/earth.obj","../texture/2k_venus_surface.jpg");
    ball earth = DrawBall("../dp/earth.obj","../texture/8k_earth_daymap.jpg");
    ball moon = DrawBall("../dp/earth.obj","../texture/2k_moon.jpg");
    ball mars = DrawBall("../dp/earth.obj","../texture/2k_mars.jpg");
    ball jupiter = DrawBall("../dp/earth.obj","../texture/2k_jupiter.jpg");
    ball satrun = DrawBall("../dp/earth.obj","../texture/2k_saturn.jpg");
    ball urnaus = DrawBall("../dp/earth.obj","../texture/2k_uranus.jpg");
    ball neptune = DrawBall("../dp/earth.obj","../texture/2k_neptune.jpg");

    float angle = 0;
    float angle1 = 0;
    float angle2 = 0;
    float angle3 = 0;
    float angle4 = 0;
    float angle5 = 0;
    float angle6 = 0;
    float angle7 = 0;
    float angle8 = 0;
    float Earthz = 0;
    float Earthx = 0;
    float moonAngle = 0;
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0,0,0, 1);
        glUseProgram(Program); 
        // camera
        {
            glm::mat4 View(1.0f);
            View = glm::lookAt(glm::vec3(Eye.x,Eye.y,Eye.z),glm::vec3(Center.x,Center.y,Center.z),glm::vec3(Up.x,Up.y,Up.z));

            if(glfwGetKey(window,GLFW_KEY_UP) == GLFW_PRESS)
            {
                Eye.z -= 0.01;
            };
            if(glfwGetKey(window,GLFW_KEY_DOWN) == GLFW_PRESS)
            {
                Eye.z += 0.01;
            };
            if(glfwGetKey(window,GLFW_KEY_LEFT) == GLFW_PRESS)
            {
                Eye.x += 0.01;
            };
            if(glfwGetKey(window,GLFW_KEY_RIGHT) == GLFW_PRESS)
            {
                Eye.x -= 0.01;
            };
            if(glfwGetKey(window,GLFW_KEY_U) == GLFW_PRESS)
            {
                Eye.y -= 0.01;
            };
            if(glfwGetKey(window,GLFW_KEY_J) == GLFW_PRESS)
            {
                Eye.y += 0.01;
            };
            glUniformMatrix4fv(glGetUniformLocation(Program, "View"), 1, 0, glm::value_ptr(View));
        }
        //
    DrawSphere(10,0,0,0,sun);
  
    if(angle < 360)
    {
        float x = 10 * sinf((angle*3.14)/180);
        float z = 10 * cosf((angle*3.14)/180);
        DrawSphere(1,x,0,z,mercury);
        angle += 0.00986;
    }
    else
    {
        angle = 0;
    };
   if(angle1 < 360)
    {
        float x = 15 * sinf((angle1*3.14)/180);
        float z = 15 * cosf((angle1*3.14)/180);
        DrawSphere(1,x,0,z,venus);
        angle1 += 0.0901;
    }
    else
    {
        angle1 = 0;
    };
   if(angle2 < 360)
    {
        Earthx = 20 * sinf((angle2*3.14)/180);
        Earthz = 20 * cosf((angle2*3.14)/180);
        DrawSphere(1,Earthx,0,Earthz,earth);
        angle2 += 0.00901;
    }
    else
    {
        angle2 = 0;
    };
   if(angle4 < 360)
    {
        float x = 30 * sinf((angle4*3.14)/180);
        float z = 30 * cosf((angle4*3.14)/180);
        DrawSphere(1,x,0,z,mars);
        angle4 += 0.0003431;
    }
    else
    {
        angle4 = 0;
    };
   if(angle5 < 360)
    {
        float x = 35 * sinf((angle5*3.14)/180);
        float z = 35 * cosf((angle5*3.14)/180);
        DrawSphere(1,x,0,z,jupiter);
        angle5 += 0.0000045;
    }
    else
    {
        angle5 = 0;
    };
   if(angle6 < 360)
    {
        float x = 40 * sinf((angle6*3.14)/180);
        float z = 40 * cosf((angle6*3.14)/180);
        DrawSphere(1,x,0,z,satrun);
        angle6 += 0.003451;
    }
    else
    {
        angle6 = 0;
    };
   if(angle7 < 360)
    {
        float x = 45 * sinf((angle7*3.14)/180);
        float z = 45 * cosf((angle7*3.14)/180);
        DrawSphere(1,x,0,z,urnaus);
        angle7 += 0.000456;
    }
    else
    {
        angle7 = 0;
    };
   if(angle8 < 360)
    {
        float x = 50 * sinf((angle8*3.14)/180);
        float z = 50 * cosf((angle8*3.14)/180);
        DrawSphere(1,x,0,z,neptune);
        angle8 += 0.000089;
    }
    else
    {
        angle8 = 0;
    };

    if(moonAngle < 360)
    {
        float x =  Earthx -(2.5*sinf((moonAngle*3.14)/180));
        float z =  Earthz -(2.5* cosf((moonAngle*3.14)/180));
        DrawSphere(1,x,0,z,moon);
        moonAngle += 0.1;
    }
    else
    {
        moonAngle = 0;
    }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}