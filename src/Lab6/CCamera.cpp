#include "Lab6/CCamera.h"
#include "stdafx.h"

#define ROTATION_SPEED 0.01f
#define MOVEMENT_SPEED 0.5f
#define CAM_OFFSET 3.0f

CCamera::CCamera() {
	_position = glm::vec3(0.0f, 10.0f, 1.0f);
	_view_direction = glm::vec3(0.0f, 0.0f, -1.0f);
	_old_mouse_position = glm::vec2(0.0f, 0.0f);
	_relative_up = glm::vec3(0, 1.0f, 0);
}

CCamera::~CCamera() {

}

void CCamera::follow_ship(glm::vec3 ship_position, glm::vec3 ship_direction) {
	glm::vec3 camera_offset = glm::vec3(0.0f, 2.6f, -3.0f); // Above and behind

	// Compute the "right" and "up" vectors for local ship space
	glm::vec3 up_vector = glm::normalize(ship_position);  // Normal to the sphere
	glm::vec3 right_vector = glm::normalize(glm::cross(ship_direction, up_vector));
	glm::vec3 forward_vector = -glm::cross(up_vector, right_vector); // Recomputed ship direction

	// Construct a transformation matrix from these vectors
	glm::mat4 ship_rotation = glm::mat4(
		glm::vec4(right_vector, 0.0f),
		glm::vec4(up_vector, 0.0f),
		glm::vec4(forward_vector, 0.0f),
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
	);

	// Rotate the camera offset into world space
	glm::vec3 world_camera_offset = glm::vec3(ship_rotation * glm::vec4(camera_offset, 1.0f));

	// Compute final camera position
	_position = ship_position + world_camera_offset;

	// Compute view direction (always towards the ship)
	_view_direction = glm::normalize(ship_position - _position);

	_relative_up = glm::normalize(ship_position);
}

void CCamera::mouse_update(glm::vec2 new_mouse_position) {
	glm::vec2 mouse_delta = new_mouse_position - _old_mouse_position;

	if (glm::length(mouse_delta) > 800.0f) {
		_old_mouse_position = new_mouse_position;
		return;
	}

	_view_direction = glm::mat3(glm::rotate((float)glm::radians(-mouse_delta.x * ROTATION_SPEED), glm::vec3(0.0f, 1.0f, 0.0f))) * _view_direction;

	glm::vec3 y_rotate_vec = glm::cross(_view_direction, glm::vec3(0.0f, 1.0f, 0.0f));
	_view_direction = glm::mat3(glm::rotate((float)glm::radians(-mouse_delta.y * ROTATION_SPEED), y_rotate_vec)) * _view_direction;

	_old_mouse_position = new_mouse_position;
}

glm::mat4 CCamera::get_view_matrix() {
	return glm::lookAt(_position, _position + _view_direction, _relative_up);
}

void CCamera::move_forward() {
	_position += MOVEMENT_SPEED * _view_direction;
}

void CCamera::move_backward() {
	_position -= MOVEMENT_SPEED * _view_direction;
}
void CCamera::strafe_left() {
	glm::vec3 strafe_direction = glm::cross(_view_direction, glm::vec3(0.0f, 1.0f, 0.0f));
	_position -= MOVEMENT_SPEED * strafe_direction;
}
void CCamera::strafe_right() {
	glm::vec3 strafe_direction = glm::cross(_view_direction, glm::vec3(0.0f, 1.0f, 0.0f));
	_position += MOVEMENT_SPEED * strafe_direction;
}

void CCamera::move_up() {
	_position += MOVEMENT_SPEED * glm::vec3(0.0f, 1.0f, 0.0f);
}

void CCamera::move_down() {
	_position -= MOVEMENT_SPEED * glm::vec3(0.0f, 1.0f, 0.0f);
}

void CCamera::look_at_origin() {
	_view_direction = glm::normalize(- 1.0f * _position);
}