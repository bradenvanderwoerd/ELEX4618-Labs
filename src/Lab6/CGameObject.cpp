#include "Lab6/CGameObject.h"
#include "stdafx.h"
#include <cmath>

#define THRUST_FORCE 1.0f
#define DRAG_FORCE 0.1f

CGameObject::CGameObject() {
	_lives = 1;
}

CGameObject::~CGameObject() {
	glDeleteBuffers(1, &_VBO);
	glDeleteBuffers(1, &_EBO);
	glDeleteVertexArrays(1, &_VAO);
}

bool CGameObject::collide(CGameObject& obj) {
	if (glm::distance(_position, obj.get_pos()) < _radius + obj.get_radius())
		return true;

	return false;
}

void CGameObject::hit() {
	_lives--;
}

void CGameObject::create_gl_objects() {
	glGenVertexArrays(1, &_VAO);
	glGenBuffers(1, &_VBO);
	glGenBuffers(1, &_EBO);

	glBindVertexArray(_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, _VBO);
	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(GLfloat), _vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLuint), _indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void CGameObject::draw() {
	glUseProgram(_program_id);
	GLint matLoc = glGetUniformLocation(_program_id, "mvp"); // change to full mvp
	glUniformMatrix4fv(matLoc, 1, GL_FALSE, &_mvp_matrix[0][0]);
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Filled faces

	glBindVertexArray(_VAO);
	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void CGameObject::update_scene(CCamera camera) {
	glm::mat4 model_matrix = glm::mat4(1.0f);
	glm::mat4 rotation_matrix = glm::mat4(1.0f);

	// If the object is at the origin, skip rotation adjustments
	if (_position != glm::vec3(0.0f)) {
		// 1. Compute the new Up vector (normal at the sphere)
		glm::vec3 new_up = glm::normalize(_position);

		// 2. Compute the new Forward direction (opposite of _direction)
		glm::vec3 new_forward = glm::normalize(-_direction);

		// 3. Compute the new Right vector (perpendicular to Up & Forward)
		glm::vec3 new_right = glm::normalize(glm::cross(new_up, new_forward));

		// 4. Recalculate Up to ensure orthogonality
		new_up = glm::cross(new_forward, new_right);

		// 5. Create the rotation matrix from the new basis vectors
		rotation_matrix[0] = glm::vec4(new_right, 0.0f);  // X-axis
		rotation_matrix[1] = glm::vec4(new_up, 0.0f);     // Y-axis
		rotation_matrix[2] = glm::vec4(new_forward, 0.0f); // Z-axis

		glm::mat4 pitch_matrix = glm::rotate(_rotation.x, new_right);
		glm::mat4 yaw_matrix = glm::rotate(_rotation.y, new_up);
		glm::mat4 roll_matrix = glm::rotate(_rotation.y, new_forward);

		rotation_matrix = roll_matrix * pitch_matrix * yaw_matrix * rotation_matrix;
	}

	// 6. Apply translation and scaling
	model_matrix = glm::translate(model_matrix, _position);
	model_matrix *= rotation_matrix;
	model_matrix = glm::scale(model_matrix, _scale);

	// 7. Compute the final MVP matrix
	glm::mat4 projection_matrix = glm::perspective(glm::radians(FOV),
		(float)_window_size.width / (float)_window_size.height,
		NEAR_PLANE, FAR_PLANE);

	_mvp_matrix = projection_matrix * camera.get_view_matrix() * model_matrix;
}
