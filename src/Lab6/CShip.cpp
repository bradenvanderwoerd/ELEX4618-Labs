#include "glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Lab6/CShip.h"

#define ROTATE_SPEED 1.5f
#define THRUST_FORCE 4.0f
#define DRAG_FORCE 0.3f

CShip::CShip() {}

CShip::CShip(cv::Size window_size, GLfloat orbit_distance) {
    _program_id = -1;
    _window_size = window_size;
    _orbit_distance = orbit_distance;

    _radius = 0.6;

    _position = glm::vec3(0.1f, _orbit_distance, 0);
    _velocity = glm::vec3(0);
    _direction = glm::vec3(0, 0, 1.0f);
    _rotation = glm::vec3(0);
    _scale = glm::vec3(_radius);

    _turn_input = 0;
    _thrust = false;

    _lives = 5;
    
    _vertices = {
        +0.0f, +0.3f, +0.0f,
        +1.0f, +0.0f, +0.0f,

        +0.5f, +0.0f, +0.25f,
        +0.0f, +1.0f, +0.0f,

        -0.5f, +0.0f, +0.25f,
        +0.0f, +0.0f, +1.0f,

        +0.0f, +0.0f, -1.0f,
        +1.0f, +1.0f, +0.0f,
    };

    _indices = {
        0, 1, 2,
        0, 2, 3,
        0, 3, 1,
        1, 3, 2 
    };
}

CShip::~CShip() {

}

void CShip::move() {
    glm::vec3 normal = normalize(_position);

    // Change velocity based on thrust or drag, project onto tangent
    if (_thrust)
        _velocity += _direction * THRUST_FORCE * DTIME;

    _velocity *= (1.0f - DRAG_FORCE * DTIME);
    _velocity -= glm::dot(_velocity, normal) * normal;

    // Change position based on velocity, project onto surface
    _position -= _velocity * DTIME;
    normal = normalize(_position);
    _position = normal * _orbit_distance;

    // Change direction based on input, project onto tangent, normalize
    _direction = glm::mat3(glm::rotate(-glm::radians(_turn_input * ROTATE_SPEED), normal)) * _direction;
    _direction -= glm::dot(_direction, normal) * normal;
    _direction = normalize(_direction);

    _rotation.x = _turn_input * _turn_input / 10;
    _rotation.y = -_turn_input / 2;
    _rotation.z = _turn_input / 4;
}

void CShip::update_input(float turn_input, bool thrust) {
    _turn_input = turn_input;
    _thrust = thrust;
}