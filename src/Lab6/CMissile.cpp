#include "Lab6/CMissile.h"

#define MISSILE_SPEED 15.0f

CMissile::CMissile(cv::Size window_size, GLfloat orbit_distance, CShip* ship, GLuint program_id) {
    _program_id = program_id;
    _window_size = window_size;
    _orbit_distance = orbit_distance;

    _rotation = glm::vec3(0);
    _radius = 0.1f;
    _scale = glm::vec3(_radius);

    _direction = ship->get_dir();
	_velocity = _direction * (MISSILE_SPEED + glm::length(ship->get_vel()));

    _position = ship->get_pos() +0.3f * _direction;

    _shininess = 100.0f;
    _specular_strength = 0.8f;

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
        0, 2, 1,
        0, 3, 2,
        0, 1, 3,
        1, 2, 3
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

CMissile::~CMissile() {

}

void CMissile::move() {
	const float speed = glm::length(_velocity);

    // Change position based on velocity, project onto surface
    _position += _velocity * DTIME;
    _position = glm::normalize(_position) * _orbit_distance;  // Re-project

    // Change direction based on input, project onto tangent, normalize
    _direction -= glm::dot(_direction, glm::normalize(_position)) * glm::normalize(_position);
    _direction = normalize(_direction);

	_velocity = _direction * speed;
}