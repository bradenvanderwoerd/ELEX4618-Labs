#include "Lab6/CGameObject.h"
#include "stdafx.h"
#include <cmath>
#include <glm/gtc/type_ptr.hpp>

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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

void CGameObject::draw() {
	glUseProgram(_program_id);
	glViewport(0, 0, _window_size.width, _window_size.height);

	glActiveTexture(GL_TEXTURE1);  // Use texture unit 1
	glBindTexture(GL_TEXTURE_2D, _depth_map);
	GLint shadow_map_loc = glGetUniformLocation(_program_id, "shadow_map");
	glUniform1i(shadow_map_loc, 1);  // Tell OpenGL that "shadow_map" is bound to texture unit 1

	GLint model_loc = glGetUniformLocation(_program_id, "model");
	glUniformMatrix4fv(model_loc, 1, GL_FALSE, &_model_matrix[0][0]);

	GLint view_loc = glGetUniformLocation(_program_id, "view");
	glUniformMatrix4fv(view_loc, 1, GL_FALSE, &_view_matrix[0][0]);

	GLint proj_loc = glGetUniformLocation(_program_id, "projection");
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, &_projection_matrix[0][0]);

	// Pass light position to shader
	GLint light_pos_loc = glGetUniformLocation(_program_id, "light_pos");
	glUniform3f(light_pos_loc, _light_pos.x, _light_pos.y, _light_pos.z); // Set your light's position

	// Pass camera position to shader
	GLint view_pos_loc = glGetUniformLocation(_program_id, "view_pos");
	glUniform3f(view_pos_loc, _camera_pos.x, _camera_pos.y, _camera_pos.z); // Use actual camera position

	// Pass light color
	GLint light_color_loc = glGetUniformLocation(_program_id, "light_color");
	glUniform3f(light_color_loc, _light_color.r, _light_color.g, _light_color.b);

	// Pass shininess
	GLint shininess_loc = glGetUniformLocation(_program_id, "shininess");
	glUniform1f(shininess_loc, _shininess);

	// Pass specular strength
	GLint specular_strength_loc = glGetUniformLocation(_program_id, "specular_strength");
	glUniform1f(specular_strength_loc, _specular_strength);

	GLuint light_space_matrix_loc = glGetUniformLocation(_program_id, "light_space_matrix");
	glUniformMatrix4fv(light_space_matrix_loc, 1, GL_FALSE, glm::value_ptr(_light_space_matrix));

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Filled faces

	glBindVertexArray(_VAO);
	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void CGameObject::draw_depth(GLuint depth_program_id, glm::mat4 light_space_matrix) {
	glUseProgram(depth_program_id);
	glEnable(GL_DEPTH_TEST);

	// Compute model matrix (same as in normal rendering)
	glm::mat4 model_matrix = _model_matrix;

	// Upload model matrix to shader
	GLint model_loc = glGetUniformLocation(depth_program_id, "model");
	glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model_matrix));

	GLint light_space_loc = glGetUniformLocation(depth_program_id, "light_space_matrix");
	glUniformMatrix4fv(light_space_loc, 1, GL_FALSE, glm::value_ptr(light_space_matrix));

	// Bind VAO and draw
	glBindVertexArray(_VAO);
	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void CGameObject::update_scene(CCamera camera, glm::vec3 light_pos, glm::vec3 light_color, glm::mat4 light_space_matrix, GLuint depth_map) {
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

	_model_matrix = model_matrix;
	_view_matrix = camera.get_view_matrix();
	_projection_matrix = projection_matrix;

	_camera_pos = camera.get_pos();

	_light_pos = light_pos;
	_light_color = light_color;

	_light_space_matrix = light_space_matrix;
	_depth_map = depth_map;
}

void CGameObject::compute_vertex_normals() {
	_normals = std::vector<glm::vec3>(_vertices.size() / 6, glm::vec3(0.0f)); // One normal per vertex

	for (size_t i = 0; i < _indices.size(); i += 3) {
		unsigned int i0 = _indices[i];
		unsigned int i2 = _indices[i + 2];
		unsigned int i1 = _indices[i + 1];

		glm::vec3 v0(_vertices[i0 * 6], _vertices[i0 * 6 + 1], _vertices[i0 * 6 + 2]);
		glm::vec3 v1(_vertices[i1 * 6], _vertices[i1 * 6 + 1], _vertices[i1 * 6 + 2]);
		glm::vec3 v2(_vertices[i2 * 6], _vertices[i2 * 6 + 1], _vertices[i2 * 6 + 2]);

		glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

		_normals[i0] += normal;
		_normals[i1] += normal;
		_normals[i2] += normal;
	}

	for (glm::vec3& normal : _normals) {
		normal = glm::normalize(normal);
	}
}