#pragma once
#include "glad.h"
#include <glm/glm.hpp>
#include "stdafx.h"


class CGameObject {
	protected:

		cv::Size _window_size;

		cv::Point3f _position;

		cv::Point3f _velocity;

		glm::vec3 _rotation;

		int _radius;

		int _lives;

		GLuint VAO, VBO, EBO;

		GLuint _program_id;

		std::vector<GLfloat> _vertices;

		std::vector<GLuint> _indices;

		glm::mat4 _mvp_matrix;

		void create_gl_objects();

	public:

		void move();

		bool collide(CGameObject& obj);

		bool collide_wall(cv::Size board);

		void hit();

		int get_lives() { return _lives; }

		void set_lives(int lives) { _lives = lives; }

		void set_pos(cv::Point3f pos) { _position = pos; }

		cv::Point3f get_pos() { return _position; }

		void set_rotation(glm::vec3 rot) { _rotation = rot; }

		glm::vec3 get_rotation() { return _rotation; }

		void draw();

		void update();
};

