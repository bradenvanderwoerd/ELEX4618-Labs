#pragma once
#include "Lab6/CGameObject.h"

class CShip : public CGameObject {
	
	private:
		float _turn_input;
		bool _thrust;

	public:
		CShip(GLuint program_id, cv::Size window_size, GLfloat orbit_distance);
		~CShip();
		void move();
		void update_input(float turn_input, bool thrust);
};
