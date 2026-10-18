#ifndef GEOMETRIA_H
#define GEOMETRIA_H

#include<glad/glad.h>

class Puntos
{
public:
    GLenum drawMode = GL_POINTS;   // le dice a main cómo dibujarse

    GLfloat vertices[7 * 6] =
    {
        -0.3f,  0.3f, 0.0f,   0.0f, 1.0f, 0.333f,
        -0.1f,  0.3f, 0.0f,   0.0f, 1.0f, 0.333f,
         0.1f,  0.3f, 0.0f,   0.0f, 1.0f, 0.333f,
         0.0f,  0.0f, 0.0f,   0.0f, 1.0f, 0.333f, //interno
        -0.3f, -0.3f, 0.0f,   0.0f, 1.0f, 0.333f,
        -0.1f, -0.3f, 0.0f,   0.0f, 1.0f, 0.333f,
         0.1f, -0.3f, 0.0f,   0.0f, 1.0f, 0.333f,
    };

    GLuint indices[7] = { 0, 1, 2, 3, 4, 5, 6 };  // uno por punto, sin conexiones
};

#endif