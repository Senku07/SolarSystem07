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

typedef unsigned int UI;

int main() { glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800,500,"Solar System",0,0);
    glfwMakeContextCurrent(window);
    gladLoadGL();


    UI vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1,&VertexShaderSource,0);
    glCompileShader(vertexShader);

    char infoLog[1000];
    glGetShaderInfoLog(vertexShader,1000,0,infoLog);

    printf("%s\n", infoLog);

    UI fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader,1,&FragmentShaderSource,0);
    glCompileShader(fragmentShader);

    glGetShaderInfoLog(fragmentShader,1000,0,infoLog);

    printf("%s\n", infoLog);



    UI Program = glCreateProgram();
    glAttachShader(Program,vertexShader);
    glAttachShader(Program,fragmentShader);
    glLinkProgram(Program);
    glUseProgram(Program);

    glGetProgramInfoLog(Program,1000,0,infoLog);
    printf("%s\n", infoLog);



    HANDLE File = CreateFileA("../dp/earth.obj", GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

    LARGE_INTEGER Size = {};
    GetFileSizeEx(File, &Size);

    char *Buffer = (char *)malloc(Size.QuadPart);
    ReadFile(File, Buffer, Size.QuadPart, 0, 0);

    obj planet = ParseObj((u8*)Buffer, Size.QuadPart);
    
    UI VAO,VBO;
    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1,&VBO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);

    glBufferData(GL_ARRAY_BUFFER,sizeof(float) * (planet.VertexCount * 3 + planet.NormalCount * 3 + planet.TexCount * 2), 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(float) * (planet.VertexCount * 3),planet.Vertex);
    glBufferSubData(GL_ARRAY_BUFFER,sizeof(float) * (planet.VertexCount * 3),sizeof(float) * (planet.TexCount * 2),planet.TexCoords);
    glBufferSubData(GL_ARRAY_BUFFER,sizeof(float) * (planet.VertexCount * 3 + planet.TexCount * 2),sizeof(float) * (planet.NormalCount * 3),planet.Normals);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,0,0,0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,2,GL_FLOAT,0,0,(void*)(4 * planet.VertexCount * 3));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,3,GL_FLOAT,0,0,(void*)(4 * (planet.VertexCount * 3+ planet.TexCount * 2)));

    glm::mat4 Model(1.0f);
    Model = glm::translate(Model,glm::vec3(0,0,-5.0f));
    Model = glm::rotate(Model,(45.0f*3.14f)/180.0f,glm::vec3(0,1,0));
    glm::mat4 View(1.0f);
    glm::mat4 Proj(1.0f);
    Proj = glm::perspective((45.0f*3.14f)/180.0f,800.0f/500.0f,.1f,100.0f);

    glUniformMatrix4fv(glGetUniformLocation(Program,"Model"),1,0,glm::value_ptr(Model));
    glUniformMatrix4fv(glGetUniformLocation(Program,"View"),1,0,glm::value_ptr(View));
    glUniformMatrix4fv(glGetUniformLocation(Program,"Proj"),1,0,glm::value_ptr(Proj));

    while(!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(1,0,0,1);

        glDrawArrays(GL_TRIANGLES,0,planet.VertexCount);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}