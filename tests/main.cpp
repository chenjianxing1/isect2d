#include "isect2d.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <memory>
#include <random>

#include <GLFW/glfw3.h>

//#define AXIS_DRAW
#define N_BOX 100

GLFWwindow* window;
float width = 800;
float height = 600;
float dpiRatio = 1;

bool pause = false;

std::vector<isect2d::OBB> obbs;

float rand_0_1(float scale) {
    return ((float)rand() / (float)(RAND_MAX)) * scale;
}

void update() {
    double time = glfwGetTime();
    if(!pause) {
        int i = 0;
        for (auto& obb : obbs) {
            float r1 = rand_0_1(10);
            float r2 = rand_0_1(20);
            float r3 = rand_0_1(M_PI);
            auto center = obb.getCenter();

            if (++i % 2 == 0) {
                obb.move(center.x, center.y + .02 * cos(time * 0.25) * r2);
                obb.rotate(cos(r3) * 0.1 + obb.getAngle());
            } else {
                obb.move(center.x + 0.1 * cos(time) * r1, center.y);
                obb.rotate(cos(r2) * 0.1 + obb.getAngle());
            }
        }
    }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(key == 'P' && action == GLFW_PRESS) {
        pause = !pause;
    }
}

void initBBoxes() {

    int n = N_BOX;
    float o = (2 * M_PI) / n;
    float size = 250;
    float boxSize = n / (0.5 * n);

    for (int i = 0; i < n; ++i) {
        float r = rand_0_1(20);

        obbs.push_back(isect2d::OBB(cos(o * i) * size + width / 2,
                    sin(o * i) * size + height / 2, r,
                    r + boxSize * 8, r * boxSize / 3 + boxSize));
    }

}

void init() {
    glfwInit();

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 2);
    window = glfwCreateWindow(width, height, "isect2d", NULL, NULL);

    if (!window) {
        glfwTerminate();
    }

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glfwSetKeyCallback(window, keyCallback);

    dpiRatio = fbWidth / width;

    glfwMakeContextCurrent(window);

    initBBoxes();
}

void line(float sx, float sy, float ex, float ey) {
    glBegin(GL_LINES);
    glVertex2f(sx, sy);
    glVertex2f(ex, ey);
    glEnd();
}

void cross(float x, float y, float size = 3) {
    line(x - size, y, x + size, y);
    line(x, y - size, x, y + size);
}

void drawOBB(const isect2d::OBB& obb, bool isect) {
    const isect2d::Vec2* quad = obb.getQuad();
    const isect2d::Vec2* axes = obb.getAxes();

    for(int i = 0; i < 4; ++i) {
        if(isect) {
            glColor4f(0.5, 1.0, 0.5, 1.0);
        } else {
            glColor4f(1.0, 0.5, 0.5, 1.0);
        }

        isect2d::Vec2 start = quad[i];
        isect2d::Vec2 end = quad[(i + 1) % 4];
        line(start.x, start.y, end.x, end.y);
        glColor4f(1.0, 1.0, 1.0, 0.1);
        cross(obb.getCenter().x, obb.getCenter().y, 2);

#ifdef AXIS_DRAW
        // draw separating axes and the projections
        for(int j = 0; j < 2; ++j) {
            isect2d::Vec2 proj = project(quad[i], axes[j]);

            glColor4f(1.0, 1.0, 1.0, 0.1);
            cross(proj.x, proj.y);
        }
#endif
    }

#ifdef AXIS_DRAW
    for(int i = 0; i < 2; ++i) {
        isect2d::Vec2 end = axes[i] * 1000.0;
        isect2d::Vec2 start = end * -1;

        glColor4f(0.4, 0.4, 0.4, 0.1);
        line(start.x, start.y, end.x, end.y);
    }
#endif
}

void render() {
    while (!glfwWindowShouldClose(window)) {
        update();

        glViewport(0, 0, width * dpiRatio, height * dpiRatio);
        glClearColor(0.18f, 0.18f, 0.22f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, width, 0, height, -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        for (auto& obb : obbs) {
            drawOBB(obb, false);
        }

        const clock_t begin_time = clock();
        auto pairs = intersect(&obbs[0], obbs.size(), &obbs[0], obbs.size());
        std::cout << float(clock () - begin_time) /  CLOCKS_PER_SEC * 1000 << "ms" << std::endl;

        for (auto pair : pairs) {
            auto obb1 = obbs[pair.first];
            auto obb2 = obbs[pair.second];
            drawOBB(obb1, true);
            line(obb1.getCenter().x, obb1.getCenter().y, obb2.getCenter().x, obb2.getCenter().y);
        }

        glfwSwapBuffers(window);

        glfwPollEvents();
    }
}

int main() {

    init();
    render();

    return 0;
}


