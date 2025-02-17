#include "glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Lab6/CShip.h"

CShip::CShip(GLuint program_id, cv::Size window_size) {
    _program_id = program_id;

    _rotation = glm::vec3(-20, 0, 0);

    _window_size = window_size;
    
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

    create_gl_objects();
}

CShip::~CShip() {

}
