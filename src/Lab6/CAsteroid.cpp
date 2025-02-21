#include "glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <random>
#include "Lab6/CAsteroid.h"

#define DTIME 0.1f
#define ROTATE_SPEED 5.0f
#define ASTEROID_SPEED 2.0f
#define DRAG_FORCE 0.5f

CAsteroid::CAsteroid(cv::Size window_size, GLfloat orbit_distance, glm::vec3 ship_position) {
    _program_id = -1;
    _window_size = window_size;
    _orbit_distance = orbit_distance;

    _position = glm::vec3(0);
    srand(time(0));
    if (glm::length(ship_position) == 0)
        _position = _orbit_distance * random_vec3();
    else
        set_position_on_hemi(ship_position);

    _rotation = glm::vec3(0);
    _scale = glm::vec3(1);

    _direction = glm::normalize(glm::cross(random_vec3(), _position));

    _velocity = _direction * ASTEROID_SPEED;

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

         0.0f, -SIZE,  PHI * SIZE,  // 4
         +0.0f, +0.0f, +1.0f,

         0.0f,  SIZE,  PHI * SIZE,  // 5
         +0.0f, +1.0f, +0.0f,

         0.0f, -SIZE, -PHI * SIZE,  // 6
         +1.0f, +0.0f, +0.0f,

         0.0f,  SIZE, -PHI * SIZE,  // 7
         +0.0f, +1.0f, +0.0f,

         PHI * SIZE,  0.0f, -SIZE,  // 8
         +0.0f, +0.0f, +1.0f,

         PHI * SIZE,  0.0f,  SIZE,  // 9
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
}

CAsteroid::~CAsteroid() {

}

void CAsteroid::move() {
    // Change position based on velocity, project onto surface
    _position += _velocity * DTIME;
    _position = glm::normalize(_position) * _orbit_distance;  // Re-project

    // Change direction based on input, project onto tangent, normalize
    _direction -= glm::dot(_direction, glm::normalize(_position)) * glm::normalize(_position);
    _direction = normalize(_direction);

    _velocity = _direction * ASTEROID_SPEED;
}

void CAsteroid::set_position_on_hemi(glm::vec3 ship_position) {
    
}

glm::vec3 CAsteroid::random_vec3() {
    // Random number generator
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);  // Uniform [-1, 1]

    // Generate a random point
    glm::vec3 pos(dist(gen), dist(gen), dist(gen));

    // Ensure non-zero position before normalizing
    while (glm::length(pos) < 0.0001f) {  // Avoid division by zero
        pos = glm::vec3(dist(gen), dist(gen), dist(gen));
    }

    return glm::normalize(pos);
}