#include <glm/glm.hpp>
#include "Lab6/CShip.h"


CShip::CShip() {
    std::vector<glm::vec3> shipVertices = {
            { 0.0f,  0.5f,  0.0f}, { 0.5f, -0.5f,  0.5f},
            {-0.5f, -0.5f,  0.5f}, { 0.0f, -0.5f, -0.5f}
    };

    std::vector<unsigned int> shipEdges = {
            0, 1,  0, 2,  0, 3,  1, 2,  2, 3,  3, 1
    };

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);


}

CShip::~CShip() {

}

void CShip::draw() {

}