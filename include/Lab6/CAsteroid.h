#pragma once
#include "CGameObject.h"

class CAsteroid : public CGameObject {
	private:
		void set_position_on_hemi(glm::vec3 ship_position);
		glm::vec3 random_vec3();

	public:
		CAsteroid(cv::Size window_size, GLfloat orbit_distance, GLuint program_id, glm::vec3 ship_position = glm::vec3(0));
		~CAsteroid();
		void move();
};

