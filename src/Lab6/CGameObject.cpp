#include "Lab6/CGameObject.h"
#include "stdafx.h"
#include "glm/gtc/matrix_transform.hpp"

#define DTIME 1
#define FOV 60.0f

void CGameObject::move() {
	_position += _velocity * DTIME;
}

bool CGameObject::collide(CGameObject& obj) {
	cv::Point3f objPos = obj.get_pos();
	if (abs(_position.x - objPos.x) <= _radius &&
		abs(_position.y - objPos.y) <= _radius &&
		abs(_position.z - objPos.z) <= _radius)
		return true;

	return false;
}

bool CGameObject::collide_wall(cv::Size board) {
	if (_position.x <= 0 || _position.x >= board.width ||
		_position.y <= 0 || _position.x >= board.height)
		return true;

	return false;
}

void CGameObject::hit() {
	_lives--;
}

void CGameObject::create_gl_objects() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(GLfloat), _vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLuint), _indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}


void CGameObject::draw() {
	GLint matLoc = glGetUniformLocation(_program_id, "mvp"); // change to full mvp
	glUniformMatrix4fv(matLoc, 1, GL_FALSE, &_mvp_matrix[0][0]);
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Back to normal

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void CGameObject::update() {
	glm::mat4 model_matrix = glm::mat4(1.0f);

	model_matrix = glm::rotate(model_matrix, glm::radians(_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	model_matrix = glm::rotate(model_matrix, glm::radians(_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	model_matrix = glm::rotate(model_matrix, glm::radians(_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

	glm::mat4 projection_matrix = glm::perspective(FOV, (float)_window_size.width / _window_size.height, 0.1f, 10.0f);

	_mvp_matrix = projection_matrix * model_matrix;

	/*for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++)
			std::cout << model_matrix[i][j] << ", ";
		std::cout << std::endl;
	}
	std::cout << "****" << std::endl;

	std::cout << _rotation.y << std::endl;*/
}