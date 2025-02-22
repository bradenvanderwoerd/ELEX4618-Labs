#pragma once
#include "CGameObject.h"
class CMissile : public CGameObject {

	private:


	public:
		CMissile(cv::Size window_size, GLfloat orbit_distance, glm::vec3 ship_position, glm::vec3 ship_direction);
		~CMissile();
		void move();

};

