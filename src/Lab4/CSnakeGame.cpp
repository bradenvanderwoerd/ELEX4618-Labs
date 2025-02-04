#include "stdafx.h"
#include "cvui.h"
#include "Lab4\\CSnakeGame.h"

#define CVUI_IMPLEMENTATION
#define DEADZONE_PERCENT 40
#define SEGMENTS 20
#define STEP_SIZE 10
#define CANVAS_NAME "SSSSSNAKE"

enum { UP = 0, RIGHT, DOWN, LEFT, RED, GREEN, BLUE};

CSnakeGame::CSnakeGame() {
	_canvas = cv::Mat::zeros(cv::Size(600,600), CV_8UC3);
	cvui::init(CANVAS_NAME);
	_ctrl.init_com();

	reset();
}

CSnakeGame::CSnakeGame(cv::Size canvas_size) {
	_canvas = cv::Mat::zeros(canvas_size, CV_8UC3);
	cvui::init(CANVAS_NAME);
	_ctrl.init_com();

	reset();
}

CSnakeGame::CSnakeGame(cv::Size canvas_size, int comport) {
	_canvas = cv::Mat::zeros(canvas_size, CV_8UC3);
	cvui::init(CANVAS_NAME);
	_ctrl.init_com(comport);

	reset();
}

CSnakeGame::~CSnakeGame() {
	
}

void CSnakeGame::gpio() {
	float right_left = _ctrl.get_analog(JOY_X);
	float up_down = _ctrl.get_analog(JOY_Y);

	if (up_down > 50 + DEADZONE_PERCENT / 2 && up_down > right_left && up_down > 100 - right_left && _direction != DOWN)
		_direction = UP;
	else if (right_left > 50 + DEADZONE_PERCENT / 2 && right_left > up_down && right_left > 100 - up_down && _direction != LEFT)
		_direction = RIGHT;
	else if (up_down < 50 - DEADZONE_PERCENT / 2 && up_down < right_left && up_down < 100 - right_left && _direction != UP)
		_direction = DOWN;
	else if (right_left < 50 - DEADZONE_PERCENT / 2 && right_left < up_down && right_left < 100 - up_down && _direction != RIGHT)
		_direction = LEFT;

	int switch_colour = _ctrl.get_button(BUTTON1);
	if (switch_colour == 0) {
		if (_colour == RED) {
			_colour = BLUE;
			_ctrl.set_data(DIGITAL, RED_LED, 0);
			_ctrl.set_data(DIGITAL, BLUE_LED, 1);
		}
		else if (_colour == BLUE) {
			_colour = GREEN;
			_ctrl.set_data(DIGITAL, BLUE_LED, 0);
			_ctrl.set_data(DIGITAL, GREEN_LED, 1);
		}
		else if (_colour == GREEN) {
			_colour = RED;
			_ctrl.set_data(DIGITAL, GREEN_LED, 0);
			_ctrl.set_data(DIGITAL, RED_LED, 1);
		}
	}

	int do_reset = _ctrl.get_button(BUTTON2);
	if (do_reset == 0)
		_reset_flag = 1;
}

void CSnakeGame::update() {
	cv::Point new_point;

	if (_direction == UP) {
		new_point.x = _snake[SEGMENTS - 1].x;
		new_point.y = _snake[SEGMENTS - 1].y - STEP_SIZE;
	}
	else if (_direction == RIGHT) {
		new_point.x = _snake[SEGMENTS - 1].x + STEP_SIZE;
		new_point.y = _snake[SEGMENTS - 1].y;
	}
	else if (_direction == DOWN) {
		new_point.x = _snake[SEGMENTS - 1].x;
		new_point.y = _snake[SEGMENTS - 1].y + STEP_SIZE;
	}
	else if (_direction == LEFT) {
		new_point.x = _snake[SEGMENTS - 1].x - STEP_SIZE;
		new_point.y = _snake[SEGMENTS - 1].y;
	}

	if (new_point.x < 0)
		new_point.x = _canvas.cols - (_canvas.cols % STEP_SIZE);
	else if (new_point.x > _canvas.cols)
		new_point.x = 0;

	if (new_point.y < 0)
		new_point.y = _canvas.rows - (_canvas.rows % STEP_SIZE);
	else if (new_point.y > _canvas.rows)
		new_point.y = 0;

	_snake.push_back(new_point);
	_snake.erase(_snake.begin());

	if (_reset_flag)
		reset();
}

void CSnakeGame::draw() {
	_canvas = cv::Mat::zeros(_canvas.size(), CV_8UC3);

	cv::Point gui_position(10, 10);
	cvui::window(_canvas, gui_position.x, gui_position.y, 140, 80, "Snake: " + std::to_string(_snake.at(SEGMENTS-1).x) + ", " + std::to_string(_snake.at(SEGMENTS - 1).y));
	
	gui_position += cv::Point(5, 25);
	if (_colour == RED)
		cvui::text(_canvas, gui_position.x, gui_position.y, "Colour: RED");
	else if (_colour == GREEN)
		cvui::text(_canvas, gui_position.x, gui_position.y, "Colour: GREEN");
	else if (_colour == BLUE)
		cvui::text(_canvas, gui_position.x, gui_position.y, "Colour: BLUE");

	gui_position += cv::Point(0, 25);
	if (cvui::button(_canvas, gui_position.x, gui_position.y, "RESET"))
		_reset_flag = 1;

	gui_position += cv::Point(70, 0);
	if (cvui::button(_canvas, gui_position.x, gui_position.y, "QUIT"))
		_exit_flag = 1;

	cv::Scalar colour_tuple;
	if (_colour == RED)
		colour_tuple = cv::Scalar(0, 0, 255);
	else if (_colour == GREEN)
		colour_tuple = cv::Scalar(0, 255, 0);
	else if (_colour == BLUE)
		colour_tuple = cv::Scalar(255, 0, 0);

	for (int index = 0; index < SEGMENTS; index++) {
		int offset = STEP_SIZE / 2 - 1;
		cv::rectangle(_canvas, _snake.at(index) - cv::Point(offset, offset), _snake.at(index) + cv::Point(offset, offset), colour_tuple, -1);
	}

	cvui::update();

	if (_exit_flag)
		cv::destroyWindow(CANVAS_NAME);
	else
		cv::imshow(CANVAS_NAME, _canvas);
}

void CSnakeGame::reset() {
	_reset_flag = 0;
	
	_direction = UP;
	_colour = RED;

	_ctrl.set_data(DIGITAL, RED_LED, 1);
	_ctrl.set_data(DIGITAL, GREEN_LED, 0);
	_ctrl.set_data(DIGITAL, BLUE_LED, 0);

	_snake.clear();

	for (int x_pos = 300; x_pos < 300 + SEGMENTS * STEP_SIZE; x_pos += STEP_SIZE) {
		cv::Point current_point(x_pos, 300);
		_snake.push_back(current_point);
	}
}