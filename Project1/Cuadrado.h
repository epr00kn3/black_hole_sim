#ifndef CUADRADO_H
#define CUADRADO_H

#include<glad/glad.h>

class Cuadrado
{
public:
    GLenum drawMode = GL_TRIANGLES;

    GLfloat vertices[4 * 6] =
    {
        // COORDENADAS              COLOR
         0.5f,  0.5f, 0.0f,   0.0f, 1.0f, 0.333f,
        -0.5f,  0.5f, 0.0f,   0.0f, 1.0f, 0.333f,
        -0.5f, -0.5f, 0.0f,   0.118f, 0.204f, 0.259f,
         0.5f, -0.5f, 0.0f,   0.118f, 0.204f, 0.259f
    };

    GLuint indices[6] =
    {
        0, 1, 2,
        2, 3, 0
    };
};

#endif