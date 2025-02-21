#pragma once
#include "glad.h"
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "stdafx.h"
#include "Lab6/CCamera.h"

#define FOV 60.0f
#define NEAR_PLANE 0.1f
#define FAR_PLANE 100.0f

class CGameObject {
	protected:

		cv::Size _window_size;

		glm::vec3 _position;

		GLfloat _orbit_distance;

		glm::vec3 _velocity;
		
		glm::vec3 _direction;

		glm::vec3 _rotation;

		glm::vec3 _scale;

		int _radius;

		int _lives;

		GLuint _VAO, _VBO, _EBO;

		GLuint _program_id;

		std::vector<GLfloat> _vertices;

		std::vector<GLuint> _indices;

		glm::mat4 _mvp_matrix;

	public:

		CGameObject();

		~CGameObject();

		virtual void move() = 0;

		bool collide(CGameObject& obj);

		bool collide_wall(cv::Size board);

		void hit();

		int get_lives() { return _lives; }

		void set_lives(int lives) { _lives = lives; }

		void set_pos(glm::vec3 pos) { _position = pos; }

		glm::vec3 get_pos() { return _position; }

		void set_vel(glm::vec3 vel) { _velocity = vel; }

		glm::vec3 get_vel() { return _velocity; }
		
		void set_dir(glm::vec3 dir) { _direction = dir; }

		glm::vec3 get_dir() { return _direction; }

		void set_rotation(glm::vec3 rot) { _rotation = rot; }

		glm::vec3 get_rotation() { return _rotation; }

		void draw();

		virtual void update_scene(CCamera camera);

		void create_gl_objects();

		void set_program_id(GLuint program_id) { _program_id = program_id; }
};

