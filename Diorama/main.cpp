#include <iostream>
#include <cstddef>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "Shaders.h"

// Ahora el vertice tiene: posicion + color(solo para el alfa) + coord. de textura
struct VertexPCT {
    GLfloat pos[3];
    GLfloat color[4]; // usamos RGB=1,1,1 (blanco) y solo variamos el alfa
    GLfloat tex[2];
};

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

static void loadTexturesFromFiles(const char* texturePaths[], int count, GLuint textures[])
{
    stbi_set_flip_vertically_on_load(1);

    for (int i = 0; i < count; ++i) {
        int w, h, channels;
        unsigned char* data = stbi_load(texturePaths[i], &w, &h, &channels, 0);
        if (!data) {
            std::cerr << "Failed to load texture: " << texturePaths[i] << std::endl;
            unsigned char fallback[4] = { 255, 0, 255, 255 };
            glBindTexture(GL_TEXTURE_2D, textures[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, fallback);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            continue;
        }

        GLenum format = GL_RGB;
        if (channels == 1) format = GL_RED;
        else if (channels == 3) format = GL_RGB;
        else if (channels == 4) format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Triangulos con textura", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    gladLoadGL();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shaders shaders("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
    GLuint shaderProgram = shaders.getProgram();
    shaders.use(shaderProgram);
    GLint mvpLoc = glGetUniformLocation(shaderProgram, "uMVP");
    GLint texLoc = glGetUniformLocation(shaderProgram, "uTex");


    const float baseY = -0.6f;

    VertexPCT triangles[] = {
        // Verde (atras, alfa 1.0, el mas chico)
        { {  0.0f,  0.35f, -0.3f }, { 0.0f, 1.0f, 0.0f, 1.0f },  { 0.5f, 1.0f } }, // apice
        { { -0.28f, baseY, -0.3f }, { 0.0f, 1.0f, 0.0f, 1.0f },  { 0.0f, 0.0f } }, // izquierda
        { {  0.28f, baseY, -0.3f }, { 0.0f, 1.0f, 0.0f, 1.0f },  { 1.0f, 0.0f } }, // derecha

        // Azul (en medio, mas transparente)
        { {  0.0f,  0.6f,  0.0f }, { 0.0f, 0.0f, 1.0f, 0.35f }, { 0.5f, 1.0f } }, // apice
        { { -0.5f, baseY,  0.0f }, { 0.0f, 0.0f, 1.0f, 0.35f }, { 0.0f, 0.0f } }, // izquierda
        { {  0.5f, baseY,  0.0f }, { 0.0f, 0.0f, 1.0f, 0.35f }, { 1.0f, 0.0f } }, // derecha

        // Rojo (adelante, opacidad media, el mas grande)
        { {  0.0f,  0.9f,  0.3f }, { 1.0f, 0.0f, 0.0f, 0.6f },  { 0.5f, 1.0f } }, // apice
        { { -0.75f, baseY,  0.3f }, { 1.0f, 0.0f, 0.0f, 0.6f }, { 0.0f, 0.0f } }, // izquierda
        { {  0.75f, baseY,  0.3f }, { 1.0f, 0.0f, 0.0f, 0.6f }, { 1.0f, 0.0f } }, // derecha
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangles), triangles, GL_STATIC_DRAW);

    GLsizei stride = sizeof(VertexPCT);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offsetof(VertexPCT, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offsetof(VertexPCT, color));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offsetof(VertexPCT, tex));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Una imagen distinta por triangulo. Reemplaza estos paths por
    // las tuyas (pueden ser .jpg o .png, cualquier tamanio).
    const char* texturePaths[3] = {
        "images/green.jpg", // triangulo verde (atras)
        "images/blue.jpg",  // triangulo azul (en medio)
        "images/red.jpg"    // triangulo rojo (adelante)
    };

    GLuint textures[3];
    glGenTextures(3, textures);
    loadTexturesFromFiles(texturePaths, 3, textures);

    shaders.use(shaderProgram);
    if (texLoc >= 0) glUniform1i(texLoc, 0);

    glClearColor(0.05f, 0.05f, 0.08f, 1.0f);

    while (!glfwWindowShouldClose(window)) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);

        int width = 0, height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        float aspect = height > 0 ? (float)width / (float)height : 4.0f / 3.0f;
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        // Una sola rotacion para los 3 -> giran exactamente igual
        float t = (float)glfwGetTime();
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), t * glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 mvp = projection * view * model;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);

        // Verde: vertices 0,1,2
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Azul: vertices 3,4,5
        glBindTexture(GL_TEXTURE_2D, textures[1]);
        glDrawArrays(GL_TRIANGLES, 3, 3);

        // Rojo: vertices 6,7,8
        glBindTexture(GL_TEXTURE_2D, textures[2]);
        glDrawArrays(GL_TRIANGLES, 6, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}