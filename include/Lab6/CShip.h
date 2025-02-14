#pragma once
#include "glad.h"
#include "Lab6/CGameObject.h"

class CShip : public CGameObject {
	
	private:
		GLuint VAO, VBO, EBO;

	public:
		CShip();
		~CShip();
		void draw();
};