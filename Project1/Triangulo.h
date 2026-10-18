#ifndef TRIANGULO_H
#define TRIANGULO_H

#include<glad/glad.h>
#include <cmath>

class Triangulo
{
public:
    GLenum drawMode = GL_TRIANGLES;

    GLfloat vertices[6 * 6] =
    {
        //					COORDENADAS					 /		 COLOR			//
            -0.5f, -0.5f * float(sqrt(3)) / 3,		0.0f,	0.118f, 0.204f, 0.259f,	// Esquina inferior izquierda
             0.5f, -0.5f * float(sqrt(3)) / 3,		0.0f,	0.118f, 0.204f, 0.259f,	// Esquina inferior derecha
             0.0f,  0.5f * float(sqrt(3)) * 2 / 3,  0.0f,	0.914f, 0.847f, 0.784f,	// Esquina superior
            -0.25f, 0.5f * float(sqrt(3)) / 6,		0.0f,	0.0f, 1.0f, 0.333f,	// Inferior izquierda centro
             0.25f, 0.5f * float(sqrt(3)) / 6,		0.0f,	0.0f, 1.0f, 0.333f,	// Inferior derecha centro
             0.0f, -0.5f * float(sqrt(3)) / 3,		0.0f,	0.118f, 0.204f, 0.259f,	// Inferior abajo
    };

    GLuint indices[9] =
    {
        0, 3, 5,
        3, 2, 4,
        5, 4, 1
    };
};

#endif