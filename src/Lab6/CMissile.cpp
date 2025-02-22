#include "Lab6/CMissile.h"

#define MISSILE_SPEED 15.0f

CMissile::CMissile(cv::Size window_size, GLfloat orbit_distance, glm::vec3 ship_position, glm::vec3 ship_direction) {
    _program_id = -1;
    _window_size = window_size;
    _orbit_distance = orbit_distance;

    _position = ship_position;

    _rotation = glm::vec3(0);
    _radius = 0.1f;
    _scale = glm::vec3(_radius);

    _direction = ship_direction;
    _velocity = _direction * MISSILE_SPEED;

    _vertices = {
        +1.0f, +0.0f, -0.707f,
        +1.0f, +0.0f, +0.0f,

        -1.0f, +0.0f, -0.707f,
        +0.0f, +1.0f, +0.0f,

        +0.0f, +1.0f, +0.707f,
        +0.0f, +0.0f, +1.0f,

        +0.0f, -1.0f, +0.707f,
        +1.0f, +1.0f, +0.0f 
    };

    _indices = {
        0, 1, 2,
        0, 2, 3,
        0, 3, 1,
        1, 3, 2 
    };
}

CMissile::~CMissile() {

}

void CMissile::move() {
    // Change position based on velocity, project onto surface
    _position += _velocity * DTIME;
    _position = glm::normalize(_position) * _orbit_distance;  // Re-project

    // Change direction based on input, project onto tangent, normalize
    _direction -= glm::dot(_direction, glm::normalize(_position)) * glm::normalize(_position);
    _direction = normalize(_direction);

    _velocity = _direction * MISSILE_SPEED;
}