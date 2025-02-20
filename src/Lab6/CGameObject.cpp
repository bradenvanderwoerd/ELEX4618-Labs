#include "Lab6/CGameObject.h"
#include "stdafx.h"

#define DTIME 0.01f
#define THRUST_FORCE 1.0f
#define DRAG_FORCE 0.1f

CGameObject::CGameObject() {
	
}

CGameObject::~CGameObject() {

}

bool CGameObject::collide(CGameObject& obj) {
	/*cv::Point3f objPos = obj.get_pos();
	if (abs(_position.x - objPos.x) <= _radius &&
		abs(_position.y - objPos.y) <= _radius &&
		abs(_position.z - objPos.z) <= _radius)
		return true;*/

	return false;
}

bool CGameObject::collide_wall(cv::Size board) {
	/*if (_position.x <= 0 || _position.x >= board.width ||
		_position.y <= 0 || _position.x >= board.height)
		return true;*/

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
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Filled faces

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void CGameObject::update_scene(CCamera camera) {
	glm::mat4 model_matrix = glm::mat4(1.0f);

	glm::vec3 normal_position = glm::vec3(0);
	if (glm::length(_position) != 0)
		normal_position = glm::normalize(_position);

	glm::vec3 n_axis = glm::cross(glm::vec3(0, 1.0f, 0), normal_position);
	float alpha = glm::acos(glm::dot(glm::vec3(0, 0, 1.0f), n_axis));
	float beta = glm::acos(glm::dot(glm::vec3(0, 1.0f, 0), normal_position));
	float gamma = glm::acos(glm::dot(n_axis, -_direction));

	model_matrix = glm::translate(model_matrix, _position);
	if (glm::length(normal_position) != 0) {
		model_matrix = glm::rotate(model_matrix, gamma, normal_position);
		model_matrix = glm::rotate(model_matrix, beta, n_axis);
		model_matrix = glm::rotate(model_matrix, alpha, glm::vec3(0.0f, 1.0f, 0));
	}
	model_matrix = glm::scale(model_matrix, _scale);

	glm::mat4 projection_matrix = glm::perspective(glm::radians(FOV), (float) _window_size.width / (float) _window_size.height, NEAR_PLANE, FAR_PLANE);

	_mvp_matrix = projection_matrix * camera.get_view_matrix() * model_matrix;
}
