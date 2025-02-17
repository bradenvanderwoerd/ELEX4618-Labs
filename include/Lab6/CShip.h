#pragma once
#include "Lab6/CGameObject.h"

class CShip : public CGameObject {
	
	private:

	public:
		CShip(GLuint program_id, cv::Size window_size);
		~CShip();
};