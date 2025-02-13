#pragma once
#include "stdafx.h"


class CGameObject {
	protected:

		cv::Point3f _position;

		cv::Point3f _velocity;

		int _radius;

		int _lives;

	public:

		void move();

		bool collide(CGameObject& obj);

		bool collide_wall(cv::Size board);

		void hit();

		int get_lives() { return _lives; }

		void set_lives(int lives) { _lives = lives; }

		void set_pos(cv::Point3f pos) { _position = pos; }

		cv::Point3f get_pos() { return _position; }

		virtual void draw(cv::Mat& im) = 0;
};

