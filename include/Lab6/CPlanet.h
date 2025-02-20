#pragma once
#include "Lab6/CGameObject.h"

class CPlanet : public CGameObject {

	private:

	public:
		CPlanet(GLuint program_id, cv::Size window_size, GLfloat orbit_distance);
		~CPlanet();
		void move();
};
