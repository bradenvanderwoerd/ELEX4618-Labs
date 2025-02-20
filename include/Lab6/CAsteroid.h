#pragma once
#include "CGameObject.h"

class CAsteroid : public CGameObject {
	private:
		void set_position_on_sphere();
		void set_position_on_hemi(glm::vec3 ship_position);

	public:
		CAsteroid(GLuint program_id, cv::Size window_size, GLfloat orbit_distance, glm::vec3 ship_position = glm::vec3(0));
		~CAsteroid();
		void move();
};

