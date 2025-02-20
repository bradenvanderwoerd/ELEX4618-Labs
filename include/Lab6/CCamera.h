#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

class CCamera {
	private:
		glm::vec3 _position;
		glm::vec3 _view_direction;
		glm::vec2 _old_mouse_position;
		glm::vec3 _relative_up;

	public: 
		CCamera();
		~CCamera();
		void follow_ship(glm::vec3 ship_position, glm::vec3 ship_velocity);
		void mouse_update(glm::vec2 new_mouse_position);
		glm::mat4 get_view_matrix();
		void move_forward();
		void move_backward();
		void strafe_left();
		void strafe_right();
		void move_up();
		void move_down();
		void look_at_origin();

};

