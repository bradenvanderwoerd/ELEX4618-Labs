#pragma once
#include "Lab6/CGameObject.h"

class CPlanet : public CGameObject {

	private:

	public:
		CPlanet();
		CPlanet(cv::Size window_size, GLfloat orbit_distance, GLuint program_id);
		~CPlanet();
		void move();
};
