#include "glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Lab6/CPlanet.h"

CPlanet::CPlanet() {}

CPlanet::CPlanet(cv::Size window_size, GLfloat orbit_distance, GLuint program_id) {
    _program_id = program_id;
    _window_size = window_size;
    _orbit_distance = orbit_distance;

    _position = glm::vec3(0);
    _velocity = glm::vec3(0);
    _direction = glm::vec3(0, 1.0f, 0);
    _rotation = glm::vec3(0);
    _scale = glm::vec3(0.85 * _orbit_distance);

    _shininess = 8.0f;
    _specular_strength = 0.2f;

    const float PHI = (1.0f + sqrt(5.0f)) / 2.0f;  // Golden ratio
    const float SIZE = 1 / PHI;  // Scale factor

    _vertices = {
        -SIZE,  PHI * SIZE,  0.0f,  // 0
         +0.0f, +1.0f, +0.0f,

         SIZE,  PHI * SIZE,  0.0f,  // 1
         +0.0f, +0.0f, +1.0f,

        -SIZE, -PHI * SIZE,  0.0f,  // 2
         +1.0f, +0.0f, +0.0f,

         SIZE, -PHI * SIZE,  0.0f,  // 3
         +0.0f, +1.0f, +0.0f,

         0.0f, -SIZE,  PHI* SIZE,  // 4
         +0.0f, +0.0f, +1.0f,

         0.0f,  SIZE,  PHI* SIZE,  // 5
         +0.0f, +1.0f, +0.0f,

         0.0f, -SIZE, -PHI * SIZE,  // 6
         +1.0f, +0.0f, +0.0f,

         0.0f,  SIZE, -PHI * SIZE,  // 7
         +0.0f, +1.0f, +0.0f,

         PHI* SIZE,  0.0f, -SIZE,  // 8
         +0.0f, +0.0f, +1.0f,

         PHI* SIZE,  0.0f,  SIZE,  // 9
         +1.0f, +1.0f, +0.0f,

        -PHI * SIZE,  0.0f, -SIZE,  // 10
         +0.0f, +1.0f, +0.0f,

        -PHI * SIZE,  0.0f,  SIZE,   // 11
         +1.0f, +0.0f, +0.0f

    };

    // Triangular faces using indices
    _indices = {
        0, 11, 5,   0, 5, 1,   0, 1, 7,   0, 7, 10,   0, 10, 11,
        1, 5, 9,   5, 11, 4,   11, 10, 2,   10, 7, 6,   7, 1, 8,
        3, 9, 4,   3, 4, 2,   3, 2, 6,   3, 6, 8,   3, 8, 9,
        4, 9, 5,   2, 4, 11,   6, 2, 10,   8, 6, 7,   9, 8, 1
    };

    compute_vertex_normals();
    std::vector<GLfloat> original_vertices = _vertices;
    _vertices.clear();
    for (size_t i = 0; i < original_vertices.size(); i += 6) {
        // Copy position (x, y, z)
        _vertices.push_back(original_vertices[i]);
        _vertices.push_back(original_vertices[i + 1]);
        _vertices.push_back(original_vertices[i + 2]);

        // Copy color (r, g, b)
        _vertices.push_back(original_vertices[i + 3]);
        _vertices.push_back(original_vertices[i + 4]);
        _vertices.push_back(original_vertices[i + 5]);

        // Copy normal (nx, ny, nz)
        _vertices.push_back(_normals[i / 6].x);
        _vertices.push_back(_normals[i / 6].y);
        _vertices.push_back(_normals[i / 6].z);
    }

    create_gl_objects();
}

CPlanet::~CPlanet() {

}

void CPlanet::move() {

}