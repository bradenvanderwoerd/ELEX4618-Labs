#include "stdafx.h"
#include "cvui.h"
#include "Lab5\\CSnakeGameV2.h"

#define CVUI_IMPLEMENTATION
#define DEADZONE_PERCENT 40
#define SEGMENTS 20
#define STEP_SIZE 10
#define CANVAS_NAME "SSSSSNAKE"
#define LOOP_PERIOD 28

enum { UP = 0, RIGHT, DOWN, LEFT, RED, GREEN, BLUE};

CSnakeGameV2::CSnakeGameV2() {
	_canvas_size = cv::Size(600, 600);

	_ctrl.init_com();

	_reset_flag = false;
	_exit_flag = false;

	_direction = UP;
	_colour = RED;
	_score = 0;
	
	_snake_speed = 200;

	_last_update_tick = cv::getTickCount();
	_last_frame_time = 1;
	_fps = 30;

	for (int x_pos = _canvas_size.width / 2; x_pos < _canvas_size.width / 2 + SEGMENTS * STEP_SIZE; x_pos += STEP_SIZE) {
		cv::Point current_point(x_pos, _canvas_size.height / 2);
		_snake.push_back(current_point);
	}
}

CSnakeGameV2::CSnakeGameV2(cv::Size canvas_size) {
	_canvas_size = canvas_size;

	_ctrl.init_com();

	_reset_flag = false;
	_exit_flag = false;

	_direction = UP;
	_colour = RED;
	_score = 0;

	_snake_speed = 200;

	_last_update_tick = cv::getTickCount();
	_last_frame_time = 1;
	_fps = 30;

	for (int x_pos = _canvas_size.width / 2; x_pos < _canvas_size.width / 2 + SEGMENTS * STEP_SIZE; x_pos += STEP_SIZE) {
		cv::Point current_point(x_pos, _canvas_size.height / 2);
		_snake.push_back(current_point);
	}
}

CSnakeGameV2::CSnakeGameV2(cv::Size canvas_size, int comport) {
	_canvas_size = canvas_size;

	_ctrl.init_com(comport);

	_reset_flag = false;
	_exit_flag = false;

	_direction = UP;
	_colour = RED;
	_score = 0;

	_snake_speed = 200;

	_last_update_tick = cv::getTickCount();
	_last_frame_time = 1;
	_fps = 30;

	for (int x_pos = _canvas_size.width / 2; x_pos < _canvas_size.width / 2 + SEGMENTS * STEP_SIZE; x_pos += STEP_SIZE) {
		cv::Point current_point(x_pos, _canvas_size.height / 2);
		_snake.push_back(current_point);
	}
}

CSnakeGameV2::~CSnakeGameV2() {
	
}

void CSnakeGameV2::run() {
	std::thread gpio_thread(&CSnakeGameV2::gpio_thread, this);
	std::thread update_thread(&CSnakeGameV2::update_thread, this);
	std::thread draw_thread(&CSnakeGameV2::draw_thread, this);

	gpio_thread.detach();
	update_thread.detach();
	draw_thread.detach();

	do {
		
	} while (!_exit_flag);
}

void CSnakeGameV2::gpio_thread() {
	do {
		gpio();
	} while (_exit_flag == false);
}

void CSnakeGameV2::update_thread() {
	do {
		update();
	} while (_exit_flag == false);
}

void CSnakeGameV2::draw_thread() {
	cvui::init(CANVAS_NAME);
	_canvas = cv::Mat::zeros(_canvas_size, CV_8UC3);
	char key = ' ';

	do {
		double start_ticks = cv::getTickCount();
		auto end_time = std::chrono::system_clock::now() + std::chrono::milliseconds(LOOP_PERIOD);
		
		draw();

		key = cv::waitKey(1);
		

		if (key == 'q')
			_exit_flag = true;

		std::this_thread::sleep_until(end_time);

		_last_frame_time = (cv::getTickCount() - start_ticks) / cv::getTickFrequency();

	} while (_exit_flag == false);
}

void CSnakeGameV2::gpio() {
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
		_reset_flag = true;
}

void CSnakeGameV2::update() {
	cv::Point new_point;

	double elapsed = 1000 * (cv::getTickCount() - _last_update_tick) / cv::getTickFrequency();
	if (elapsed > _snake_speed) {

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
			new_point.x = _canvas_size.width - (_canvas_size.width % STEP_SIZE);
		else if (new_point.x > _canvas_size.width)
			new_point.x = 0;

		if (new_point.y < 0)
			new_point.y = _canvas_size.height - (_canvas_size.height % STEP_SIZE);
		else if (new_point.y > _canvas_size.height)
			new_point.y = 0;

		_snake_mutex.lock();
		_snake.push_back(new_point);
		_snake.erase(_snake.begin());
		_snake_mutex.unlock();

		_last_update_tick = cv::getTickCount();

		_fps = 1 / _last_frame_time;
	}

	if (_reset_flag) {
		_reset_flag = false;

		_direction = UP;
		_colour = RED;
		_score = 0;

		_snake_mutex.lock();
		_snake.clear();
		_snake_mutex.unlock();

		for (int x_pos = _canvas_size.width / 2; x_pos < _canvas_size.width / 2 + SEGMENTS * STEP_SIZE; x_pos += STEP_SIZE) {
			cv::Point current_point(x_pos, _canvas_size.height / 2);

			_snake_mutex.lock();
			_snake.push_back(current_point);
			_snake_mutex.unlock();
		}
	}
}

void CSnakeGameV2::draw() {

	// Reset canvas
	_canvas.setTo(cv::Scalar(0, 0, 0));

	// CVUI window
	cv::Point gui_position(10, 10);
	_snake_mutex.lock();
	cv::Point snake_position = _snake.at(SEGMENTS - 1);
	_snake_mutex.unlock();
	cvui::window(_canvas, gui_position.x, gui_position.y, 220, 200, "Snake: (" + std::to_string(snake_position.x) + ", " + std::to_string(snake_position.y) + 
				 ") (FPS = " + std::to_string(_fps) + ")");

	// Colour and score
	gui_position += cv::Point(5, 25);
	cv::Scalar colour_tuple;
	cv::Scalar depth_tuple;
	if (_colour == RED) {
		cvui::text(_canvas, gui_position.x, gui_position.y, "Colour: RED");
		colour_tuple = cv::Scalar(0, 0, 255);
		depth_tuple = cv::Scalar(0, 0, 155);
	}
	else if (_colour == GREEN) {
		cvui::text(_canvas, gui_position.x, gui_position.y, "Colour: GREEN");
		colour_tuple = cv::Scalar(0, 255, 0);
		depth_tuple = cv::Scalar(0, 155, 0);
	}
	else if (_colour == BLUE) {
		cvui::text(_canvas, gui_position.x, gui_position.y, "Colour: BLUE");
		colour_tuple = cv::Scalar(255, 170, 50);
		depth_tuple = cv::Scalar(155, 85, 25);
	}

	gui_position += cv::Point(0, 25);
	cvui::text(_canvas, gui_position.x, gui_position.y, "Score: " + std::to_string(_score));

	// Reset and quit
	gui_position += cv::Point(0, 25);
	if (cvui::button(_canvas, gui_position.x, gui_position.y, "RESET"))
		_reset_flag = true;

	bool close_window = false;
	gui_position += cv::Point(70, 0);
	if (cvui::button(_canvas, gui_position.x, gui_position.y, "QUIT"))
		close_window = true;


	// Shadow
	int offset = STEP_SIZE / 2 - 1;
	for (int index = 0; index < SEGMENTS; index++) {
		_snake_mutex.lock();
		cv::Point current_point = _snake.at(index);
		_snake_mutex.unlock();
		
		std::vector<cv::Point> depth_shape;
		depth_shape.push_back(current_point + cv::Point(-offset, offset));
		depth_shape.push_back(current_point + cv::Point(-offset + STEP_SIZE / 2, offset + STEP_SIZE / 2));
		depth_shape.push_back(current_point + cv::Point(offset + STEP_SIZE / 2, offset + STEP_SIZE / 2));
		depth_shape.push_back(current_point + cv::Point(offset + STEP_SIZE / 2, -offset + STEP_SIZE / 2));
		depth_shape.push_back(current_point + cv::Point(offset, -offset));
		depth_shape.push_back(current_point + cv::Point(offset, offset));

		cv::fillPoly(_canvas, depth_shape, depth_tuple);
	}

	// Segment
	for (int index = 0; index < SEGMENTS; index++) {
		_snake_mutex.lock();
		cv::Point current_point = _snake.at(index);
		_snake_mutex.unlock();

		cv::rectangle(_canvas,
					  current_point - cv::Point(offset, offset),
					  current_point + cv::Point(offset, offset),
				      colour_tuple, -1);
	}

	// Update
	cvui::update();

	// imshow
	if (close_window || _exit_flag) {
		cv::destroyWindow(CANVAS_NAME);
		_exit_flag = true;
	}
	else
		cv::imshow(CANVAS_NAME, _canvas);
}