#pragma once
#include "Lab6/CGameObject.h"
#include "Lab6/CShip.h"

class CMissile : public CGameObject {

	private:


	public:
		CMissile(cv::Size window_size, GLfloat orbit_distance, CShip* ship, GLuint program_id);
		~CMissile();
		void move();

};

