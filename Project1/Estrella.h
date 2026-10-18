#ifndef ESTRELLA_H
#define ESTRELLA_H

#include<glad/glad.h>

class Estrella
{
public:
    GLenum drawMode = GL_TRIANGLES;

    static constexpr float Ro = 0.5f;    // radio de las puntas
    static constexpr float Ri = 0.2f;    // radio de los valles
    static constexpr float C = 0.8660f;  // cos(30°) = sqrt(3)/2
    static constexpr float S = 0.5f;     // sin(30°)

    GLfloat vertices[7 * 6] =
    {
        // COORDENADAS           COLOR
         0.0f,   0.0f,   0.0f,   0.0f, 1.0f, 0.333f,  // 0 Centro

         0.0f,      Ro,       0.0f,     0.118f, 0.204f, 0.259f,     // 1 Punta   90°
         Ri * C,    Ri * S,   0.0f,     0.118f, 0.204f, 0.259f,     // 2 Valle   30°
         Ro * C,   -Ro * S,   0.0f,     0.118f, 0.204f, 0.259f,      // 3 Punta  -30°
         0.0f,     -Ri,       0.0f,     0.118f, 0.204f, 0.259f,     // 4 Valle  -90°
        -Ro * C,   -Ro * S,   0.0f,     0.118f, 0.204f, 0.259f,      // 5 Punta -150°
        -Ri * C,    Ri * S,   0.0f,     0.118f, 0.204f, 0.259f       // 6 Valle  150°
    };

    GLuint indices[18] =
    {
        0, 1, 2,
        0, 2, 3,
        0, 3, 4,
        0, 4, 5,
        0, 5, 6,
        0, 6, 1
    };
};

#endif