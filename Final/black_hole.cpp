#include <chrono>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace glm;
using namespace std;
using Clock = std::chrono::high_resolution_clock;

// Variables
double c = 299792458.0;
double G = 6.67430e-11;
bool Gravity = false;

//INICIO
struct Engine {
    GLFWwindow* window = nullptr;
	int WIDTH = 800;
	int HEIGHT = 600;
	double width = 1e11;
	double height = 7.5e10;

    // Navigation state
    float offsetX = 0.0f, offsetY = 0.0f;
    float zoom = 1.0f;
    bool middleMousePressed = false;
    double lastMouseX = 0.0, lastMouseY = 0;

    Engine() {
        if (!glfwInit()) {
            std::cerr << "Fallo al iniciar GLFW" << std::endl;
            exit(EXIT_FAILURE);
        }
        window = glfwCreateWindow(WIDTH, HEIGHT, "Black Hole SIM", nullptr, nullptr);
        if (!window) {
            std::cerr << "Fallo al crear ventana" << std::endl;
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
        glfwMakeContextCurrent(window);
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) {
            std::cerr << "Fallo al iniciar GLEW" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    void run() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        double left = -width + offsetX;
        double right = width + offsetX;
        double bottom = -height + offsetY;
        double top = height + offsetY;
        glOrtho(left, right, bottom, top, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
};
Engine engine;

//Estructura: Forma sencilla de crear tu propio tipi de dato que puede almacenar sus propias variables y funciones
struct BlackHole { 
    vec2 position;
    double mass; //Masa
    double r_s; //Horizonte de sucesos, distancia a la que ni siquiera la luz puede escapar

    BlackHole(vec2 pos, double m) : position(pos), mass(m) { r_s = (2.0 * G * mass) / (c * c);}

    void draw() {
        glBegin(GL_TRIANGLE_FAN);
        glColor3f(1.0, 0.0, 0.0); //Color rojo para el black hole
        glVertex2f(position.x, position.y); // Centro
        for (int i = 0; i <= 100; i++) {
            double angle = 2.0f * M_PI * i / 100;
            double x = r_s * cos(angle) + position.x; //Radio por 0.1
            double y = r_s * sin(angle) + position.y;
            glVertex2f(x, y);
        }
        glEnd();
    }
};

BlackHole SagA(vec3(0.0, 0.0, 0.0), 8.54e36);; // Sagittarius A black hole

struct Ray {
    // posición cartesiana
    double x, y, z;
    // coordenadas polares
    double r, phi, theta;
    double dr, dphi, theta; //velocidades 
    double d2r, d2phi, theta;
    // cantidades conservadas
    double E, L;
    // dirección original
    vec2 dir;
    vector<vec2> trail;
    bool alive = true;

    

    Ray(vec2 pos, vec2 dir) : x(pos.x), y(pos.y) {
        r = hypot(x, y);
        phi = atan2(y, x);

        // velocidad inicial = c en la dirección dada
        dr = c * (dir.x * cos(phi) + dir.y * sin(phi));
        dphi = c * (-dir.x * sin(phi) + dir.y * cos(phi)) / r;

        L = r * r * dphi;
        double f = 1.0 - SagA.r_s / r;
        double dt_dlambda = sqrt((dr * dr) / (f * f) + (r * r * dphi * dphi) / f) / c;
        E = f * dt_dlambda;

        trail.push_back(vec2((float)x, (float)y));
    }

    static void deriv(const double s[4], double out[4], double rs, double E) {
        double r = s[0], dr = s[2], dphi = s[3];
        double f = 1.0 - rs / r;
        double dt = E / f;

        out[0] = dr;
        out[1] = dphi;
        out[2] = -(c * c * rs / (2.0 * r * r)) * f * dt * dt
            + (rs / (2.0 * r * r * f)) * dr * dr
            + (r - rs) * dphi * dphi;
        out[3] = -2.0 * dr * dphi / r;
    }

    void draw() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glLineWidth(1.0f);

        size_t N = trail.size();
        if (N < 2) return;

        glBegin(GL_LINE_STRIP);
        for (size_t i = 0; i < N; ++i) {
            float alpha = float(i) / float(N - 1);
            glColor4f(1.0f, 1.0f, 1.0f, std::max(alpha, 0.05f));
            glVertex2f(trail[i].x, trail[i].y);
        }
        glEnd();
    }

    void step(double rs, double dlambda) {
        if (!alive) return;
        if (r <= rs * 1.01) { alive = false; return; }   // cayó al agujero negro

        double s[4] = { r, phi, dr, dphi };
        double k1[4], k2[4], k3[4], k4[4], t[4];

        deriv(s, k1, rs, E);
        for (int i = 0; i < 4; i++) t[i] = s[i] + k1[i] * dlambda / 2;
        deriv(t, k2, rs, E);
        for (int i = 0; i < 4; i++) t[i] = s[i] + k2[i] * dlambda / 2;
        deriv(t, k3, rs, E);
        for (int i = 0; i < 4; i++) t[i] = s[i] + k3[i] * dlambda;
        deriv(t, k4, rs, E);

        for (int i = 0; i < 4; i++)
            s[i] += dlambda / 6.0 * (k1[i] + 2 * k2[i] + 2 * k3[i] + k4[i]);

        r = s[0]; phi = s[1]; dr = s[2]; dphi = s[3];

        x = r * cos(phi);
        y = r * sin(phi);

        trail.push_back(vec2((float)x, (float)y));
        if (trail.size() > 2000) trail.erase(trail.begin());
    }
};
vector<Ray> rays;

// calcula las derivadas {dr, dphi, d2r, d2phi} y las guarda en rhs
void geodesic(const Ray& ray, double rhs[4], double rs) {
    double r = ray.r, dr = ray.dr, dphi = ray.dphi, E = ray.E;
    double f = 1.0 - rs / r;
    double dt_dlambda = E / f;

    rhs[0] = dr;
    rhs[1] = dphi;
    rhs[2] = -(c * c * rs / (2.0 * r * r)) * f * dt_dlambda * dt_dlambda
        + (rs / (2.0 * r * r * f)) * dr * dr
        + (r - rs) * dphi * dphi;
    rhs[3] = -2.0 * dr * dphi / r;
}

// out = a + b * factor
void addState(const double a[4], const double b[4], double factor, double out[4]) {
    for (int i = 0; i < 4; i++) out[i] = a[i] + b[i] * factor;
}

void rk4Step(Ray& ray, double dlambda, double rs) {
    double y0[4] = { ray.r, ray.phi, ray.dr, ray.dphi };
    double k1[4], k2[4], k3[4], k4[4], temp[4];

    geodesic(ray, k1, rs);
    addState(y0, k1, dlambda / 2.0, temp);
    Ray r2 = ray; r2.r = temp[0]; r2.phi = temp[1]; r2.dr = temp[2]; r2.dphi = temp[3];
    geodesic(r2, k2, rs);

    addState(y0, k2, dlambda / 2.0, temp);
    Ray r3 = ray; r3.r = temp[0]; r3.phi = temp[1]; r3.dr = temp[2]; r3.dphi = temp[3];
    geodesic(r3, k3, rs);

    addState(y0, k3, dlambda, temp);
    Ray r4 = ray; r4.r = temp[0]; r4.phi = temp[1]; r4.dr = temp[2]; r4.dphi = temp[3];
    geodesic(r4, k4, rs);

    double h = dlambda / 6.0;
    ray.r += h * (k1[0] + 2 * k2[0] + 2 * k3[0] + k4[0]);
    ray.phi += h * (k1[1] + 2 * k2[1] + 2 * k3[1] + k4[1]);
    ray.dr += h * (k1[2] + 2 * k2[2] + 2 * k3[2] + k4[2]);
    ray.dphi += h * (k1[3] + 2 * k2[3] + 2 * k3[3] + k4[3]);

    ray.x = ray.r * cos(ray.phi);
    ray.y = ray.r * sin(ray.phi);
    ray.trail.push_back(vec2((float)ray.x, (float)ray.y));
}

int main() {
    int numRays = 15;
    for (int i = 0; i < numRays; ++i) {
        float y = -7e10f + i * (1.4e11f / (numRays - 1));
        rays.push_back(Ray(vec2(-9.5e10f, y), vec2(1.0f, 0.0f)));
    }

    while (!glfwWindowShouldClose(engine.window)) {
        engine.run();
        SagA.draw();

        for (auto& ray : rays) {
            ray.step(SagA.r_s, 2.0);
            ray.draw();
        }

        glfwSwapBuffers(engine.window);
        glfwPollEvents();
    }
    return 0;
}