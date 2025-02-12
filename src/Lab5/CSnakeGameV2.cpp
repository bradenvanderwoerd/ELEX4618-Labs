#include "stdafx.h"
#include "cvui.h"
#include "Lab5/CSnakeGameV2.h"
#include <fstream>

#define CVUI_IMPLEMENTATION

#define DEADZONE_PERCENT 40
#define STARTING_SEGMENTS 10
#define CANVAS_NAME "RETRO SNAKE"
#define LOOP_PERIOD 28
#define SIMPLE_RENDER_SIZE 3
#define APPLE_SPAWN_RATE 5
#define X_OFFSET 0
#define CURVATURE 0.09
#define MENU_SPEED 500
#define DO_SESSION_HIGH_SCORE 0


enum { UP = 0, RIGHT, DOWN, LEFT };
enum { RED = 0, GREEN, BLUE };

CSnakeGameV2::CSnakeGameV2(cv::Size canvas_size) {
	_canvas_size = canvas_size;

	_ctrl.init_com();

	_reset_flag = true;
	_exit_flag = false;

	_snake_size = 27;
	_snake_speed = 100;

	_last_update_tick = cv::getTickCount();
	_last_frame_time = 1;
	_fps = 30;

	_do_crt = true;

	_show_start_text = false;
	_start_game = false;

	_canvas = cv::Mat::zeros(_canvas_size, CV_8UC3);

	cv::Mat colour_pattern(_canvas_size.height, 3, CV_8UC3);
	colour_pattern.col(0) = cv::Scalar(2, 0, 0);
	colour_pattern.col(1) = cv::Scalar(0, 2, 0);
	colour_pattern.col(2) = cv::Scalar(0, 0, 2);
	cv::repeat(colour_pattern, 1, _canvas_size.width / 3 + 1, _crt_mask);
	if (_canvas_size.width % 3 != 0)
		_crt_mask = _crt_mask.colRange(0, _canvas_size.width);

	_barrel_x.create(_canvas_size.height, _canvas_size.width, CV_32FC1);
	_barrel_y.create(_canvas_size.height, _canvas_size.width, CV_32FC1);

	float cx = _canvas_size.width / 2.0f, cy = _canvas_size.height / 2.0f;
	float max_dist = std::sqrt(cx * cx + cy * cy);

	for (int y = 0; y < _canvas_size.height; y++) {
		for (int x = 0; x < _canvas_size.width; x++) {
			// Barrel Distortion
			float dx = (x - cx) / cx;
			float dy = (y - cy) / cy;
			float r2 = dx * dx + dy * dy;
			float scale = 1 + CURVATURE * r2;
			_barrel_x.at<float>(y, x) = cx + dx * cx * scale;
			_barrel_y.at<float>(y, x) = cy + dy * cy * scale;
		}
	}

	_menu_no_start_text = cv::imread("resources/menu_start.png");
	_menu_start_text = cv::imread("resources/menu_no_start.png");

	_play_snake_music = false;
	_play_upgrade_sound = false;
	_play_game_over_sound = false;
	_upgrade_channel = -1;
	_game_over_channel = -1;

	_score = 0;
	if (DO_SESSION_HIGH_SCORE == 1)
		_high_score = 0;
	else {
		std::ifstream infile("resources/high_score.txt");
		infile >> _high_score;
		infile.close();
	}

	_last_update_complete = true;
}

CSnakeGameV2::~CSnakeGameV2() {
	if (_score > _high_score) {
		std::ofstream outfile("resources/high_score.txt", std::ios::trunc);
		outfile << _score;
		outfile.close();
	}
}

void CSnakeGameV2::run() {
	std::thread gpio_thread(&CSnakeGameV2::gpio_thread, this);
	std::thread update_thread(&CSnakeGameV2::update_thread, this);
	std::thread draw_thread(&CSnakeGameV2::draw_thread, this);
	std::thread sound_thread(&CSnakeGameV2::sound_thread, this);

	gpio_thread.detach();
	update_thread.detach();
	draw_thread.detach();
	sound_thread.detach();

	do {
		Sleep(10);
	} while (!_exit_flag);
}

void CSnakeGameV2::gpio_thread() {
	do {
		gpio();
	} while (_exit_flag == false);
}

void CSnakeGameV2::update_thread() {
	srand(time(0));

	do {
		update();
	} while (_exit_flag == false);
}

void CSnakeGameV2::draw_thread() {
	cvui::init(CANVAS_NAME);

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

void CSnakeGameV2::sound_thread() {

	// Move to sound thread
	SDL_Init(SDL_INIT_AUDIO);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

	_music = Mix_LoadMUS("resources/snake.wav");
	_upgrade_sound = Mix_LoadWAV("resources/upgrade.wav");
	_game_over_sound = Mix_LoadWAV("resources/game_over.wav");

	do {
		sound();
	} while (_exit_flag == false);

	Mix_FreeMusic(_music);
	Mix_CloseAudio();
	SDL_Quit();
}

void CSnakeGameV2::gpio() {
	int dummy;
	if (!_ctrl.get_data(DIGITAL, BUTTON1, dummy))
		_ctrl.init_com();
	
	if (!_start_game) {
		_start_game = _ctrl.get_button(LOWER_BUTTON1) == 0 ||
					  _ctrl.get_button(LOWER_BUTTON2) == 0 ||
					  _ctrl.get_button(BUTTON1) == 0 ||
					  _ctrl.get_button(BUTTON2) == 0;
		
		return;
	}
	
	float right_left = _ctrl.get_analog(JOY_X);
	float up_down = _ctrl.get_analog(JOY_Y);
	
	if (_last_update_complete) {
		if (up_down > 50 + DEADZONE_PERCENT / 2 && up_down > right_left && up_down > 100 - right_left && _direction != DOWN) {
			_direction = UP;
			_last_update_complete = false;
		}
		else if (right_left > 50 + DEADZONE_PERCENT / 2 && right_left > up_down && right_left > 100 - up_down && _direction != LEFT) {
			_direction = RIGHT;
			_last_update_complete = false;
		}
		else if (up_down < 50 - DEADZONE_PERCENT / 2 && up_down < right_left && up_down < 100 - right_left && _direction != UP) {
			_direction = DOWN;
			_last_update_complete = false;
		}
		else if (right_left < 50 - DEADZONE_PERCENT / 2 && right_left < up_down && right_left < 100 - up_down && _direction != RIGHT) {
			_direction = LEFT;
			_last_update_complete = false;
		}
	}

	int switch_colour = _ctrl.get_button(BUTTON1);
	// Change to start with LED on
	if (switch_colour == 0) {
		if (_colour == RED) {
			_colour = BLUE;
			/*_ctrl.set_data(DIGITAL, RED_LED, 0);
			_ctrl.set_data(DIGITAL, BLUE_LED, 1);*/
		}
		else if (_colour == BLUE) {
			_colour = GREEN;
			/*_ctrl.set_data(DIGITAL, BLUE_LED, 0);
			_ctrl.set_data(DIGITAL, GREEN_LED, 1);*/
		}
		else if (_colour == GREEN) {
			_colour = RED;
			/*_ctrl.set_data(DIGITAL, GREEN_LED, 0);
			_ctrl.set_data(DIGITAL, RED_LED, 1);*/
		}
	}

	int do_reset = _ctrl.get_button(BUTTON2);
	if (do_reset == 0)
		_reset_flag = true;

	if (!_ctrl.get_button(LOWER_BUTTON1))
		_do_crt = !_do_crt;
}

void CSnakeGameV2::update() {
	
	if (_reset_flag) {
		_reset_flag = false;

		_direction = UP;
		_colour = RED;

		if (_score > _high_score) {
			std::ofstream outfile("resources/high_score.txt", std::ios::trunc);
			outfile << _score;
			_high_score = _score;
			outfile.close();
		}
		_score = 0;

		_snake_mutex.lock();
		_snake.clear();
		_snake_mutex.unlock();

		for (int x_pos = (_canvas_size.width - X_OFFSET) / 2 / _snake_size; x_pos < (_canvas_size.width - X_OFFSET) / 2 / _snake_size + STARTING_SEGMENTS; x_pos++) {
			cv::Point current_point(x_pos, _canvas_size.height / 2 / _snake_size);

			_snake_mutex.lock();
			_snake.push_back(current_point);
			_snake_mutex.unlock();
		}

		_snake_mutex.lock();
		_apple = cv::Point(rand() % ((_canvas_size.width - X_OFFSET) / _snake_size - 1) + 1, rand() % (_canvas_size.height / _snake_size - 1) + 1);
		_snake_mutex.unlock();

		_game_over = false;

		_play_snake_music = true;
	}

	double elapsed = 1000 * (cv::getTickCount() - _last_update_tick) / cv::getTickFrequency();
	if (elapsed > _snake_speed && !_game_over) {

		if (!_start_game) {
			_show_start_text = !_show_start_text;

			return;
		}

		cv::Point new_point;

		if (_direction == UP) {
			new_point.x = _snake[_snake.size() - 1].x;
			new_point.y = _snake[_snake.size() - 1].y - 1;
		}
		else if (_direction == RIGHT) {
			new_point.x = _snake[_snake.size() - 1].x + 1;
			new_point.y = _snake[_snake.size() - 1].y;
		}
		else if (_direction == DOWN) {
			new_point.x = _snake[_snake.size() - 1].x;
			new_point.y = _snake[_snake.size() - 1].y + 1;
		}
		else if (_direction == LEFT) {
			new_point.x = _snake[_snake.size() - 1].x - 1;
			new_point.y = _snake[_snake.size() - 1].y;
		}

		if (new_point.x < 1)
			new_point.x = (_canvas_size.width - X_OFFSET) / _snake_size - 1;
		else if (new_point.x > (_canvas_size.width - X_OFFSET) / _snake_size - 1)
			new_point.x = 1;

		if (new_point.y < 1)
			new_point.y = _canvas_size.height / _snake_size - 1;
		else if (new_point.y > _canvas_size.height / _snake_size - 1)
			new_point.y = 1;

		_snake_mutex.lock();
		
		for (cv::Point segment : _snake) {
			if (new_point.x == segment.x && new_point.y == segment.y) {
				_game_over = true;
				_play_game_over_sound = true;
				_snake_mutex.unlock();
				return;
			}
		}

		if (!_game_over) {
			_snake.push_back(new_point);
			if (_snake.size() > STARTING_SEGMENTS + _score)
				_snake.erase(_snake.begin());
		}
		_snake_mutex.unlock();

		_last_update_tick = cv::getTickCount();

		_fps = 1 / _last_frame_time;

		_last_update_complete = true;
	}

	_snake_mutex.lock();
	if (_snake.at(_snake.size() - 1).x == _apple.x &&
		_snake.at(_snake.size() - 1).y == _apple.y) {

		_score++;

		_play_upgrade_sound = true;
		
		cv::Point new_apple;
		bool point_is_valid;
		do {
			point_is_valid = true;
			new_apple = cv::Point(rand() % ((_canvas_size.width - X_OFFSET) / _snake_size - 1) + 1, rand() % (_canvas_size.height / _snake_size - 1) + 1);
			for (cv::Point segment : _snake) {
				if (new_apple.x == segment.x && new_apple.y == segment.y) {
					point_is_valid = false;
					break;
				}
			}
		} while (!point_is_valid);

		_apple = new_apple;
	}
	_snake_mutex.unlock();

}

void CSnakeGameV2::draw() {

	// Reset canvas
	_canvas.setTo(cv::Scalar(0, 0, 0));

	if (!_start_game) {
		if (_show_start_text)
			_menu_start_text.copyTo(_canvas);
		else
			_menu_no_start_text.copyTo(_canvas);

		_canvas = crt(_canvas);

		cv::imshow(CANVAS_NAME, _canvas);

		return;
	}

	cv::Scalar colour_tuple;
	cv::Scalar depth_tuple;
	int colour_offset = 0;
	if (_colour == RED) {
		colour_tuple = cv::Scalar(50, 60, 230);
		depth_tuple = cv::Scalar(15, 20, 110);
		colour_offset = 0;
	}
	else if (_colour == GREEN) {
		colour_tuple = cv::Scalar(60, 180, 60);
		depth_tuple = cv::Scalar(20, 80, 20);
		colour_offset = 0;
	}
	else if (_colour == BLUE) {
		colour_tuple = cv::Scalar(255, 170, 50);
		depth_tuple = cv::Scalar(155, 85, 25);
		colour_offset = -1;
	}

	if (cvui::button(_canvas, 300, 735, "RESETRESET"))
		_reset_flag = true;
	cv::rectangle(_canvas, cv::Rect(290, 735, 120, 30), cv::Scalar(0, 0, 0), cv::FILLED);

	bool close_window = false;
	if (cvui::button(_canvas, 640, 735, "QUITQUITQUIT"))
		close_window = true;
	cv::rectangle(_canvas, cv::Rect(640, 735, 130, 30), cv::Scalar(0, 0, 0), cv::FILLED);

	// Shadow
	int offset;
	if (_snake_size > SIMPLE_RENDER_SIZE)
		offset = _snake_size / 2 - 1;
	else if (_snake_size < 24)
		offset = _snake_size - 1;
	else
		offset = _snake_size - 2;

	_snake_mutex.lock();
	for (int index = 0; index < _snake.size() && _snake_size > SIMPLE_RENDER_SIZE; index++) {
		
		cv::Point current_point = _snake.at(index) * _snake_size;
		
		std::vector<cv::Point> depth_shape;
		depth_shape.push_back(current_point + cv::Point(-offset + X_OFFSET + colour_offset, offset));
		depth_shape.push_back(current_point + cv::Point(-offset + _snake_size / 2 + X_OFFSET + colour_offset, offset + _snake_size / 2));
		depth_shape.push_back(current_point + cv::Point(offset + _snake_size / 2 + X_OFFSET + colour_offset, offset + _snake_size / 2));
		depth_shape.push_back(current_point + cv::Point(offset + _snake_size / 2 + X_OFFSET + colour_offset, -offset + _snake_size / 2));
		depth_shape.push_back(current_point + cv::Point(offset + X_OFFSET + colour_offset, -offset));
		depth_shape.push_back(current_point + cv::Point(offset + X_OFFSET + colour_offset, offset));

		cv::fillPoly(_canvas, depth_shape, depth_tuple);
	}
	_snake_mutex.unlock();

	// Segment
	_snake_mutex.lock();
	for (int index = 0; index < _snake.size(); index++) {
		cv::Point current_point = _snake.at(index) * _snake_size;

		cv::rectangle(_canvas,
					  current_point - cv::Point(offset - X_OFFSET - colour_offset, offset),
					  current_point + cv::Point(offset + X_OFFSET + colour_offset, offset),
				      colour_tuple, -1);
	}
	_snake_mutex.unlock();

	// Apple
	_snake_mutex.lock();
	int radius = 1;
	if (_snake_size > SIMPLE_RENDER_SIZE + 7) {
		radius = _snake_size / 2;
		cv::circle(_canvas, (_apple * _snake_size + cv::Point(4 + X_OFFSET, 4)), radius, cv::Scalar(0, 100, 150), -1, cv::LineTypes::LINE_AA);
	}
	cv::circle(_canvas, _apple * _snake_size + cv::Point(X_OFFSET, 0), radius, cv::Scalar(0, 150, 250), -1, cv::LineTypes::LINE_AA);
	_snake_mutex.unlock();

	if (_game_over)
		cvui::text(_canvas, _canvas_size.width / 2 - 80, _canvas_size.height / 2 - 10, "GAME OVER", 1);

	cvui::text(_canvas, 370, 25, "Score: " + std::to_string(_score) + "  FPS: " + std::to_string(_fps), 1);
	if (_score <= _high_score)
		cvui::text(_canvas, 390, 55, "High Score: " + std::to_string(_high_score), 1);
	else
		cvui::text(_canvas, 375, 55, "New High Score!", 1);

	cvui::text(_canvas, 290, 750, "RESET", 1);
	cvui::text(_canvas, 650, 750, "Quit", 1);

	cv::rectangle(_canvas, cv::Rect(10, 10, _canvas_size.width - 5, _canvas_size.height - 20), cv::Scalar(200, 200, 200), 5);

	if (_do_crt) {
		_canvas = crt(_canvas);
	}
	else {
		cvui::trackbar(_canvas, 20, 380, 210, &_snake_size, 1, 30);
		cvui::trackbar(_canvas, 20, 420, 210, &_snake_speed, 10, 200);
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

void CSnakeGameV2::sound() {
	if (_play_snake_music && _start_game) {
		Mix_PlayMusic(_music, -1);
		_play_snake_music = !_play_snake_music;
	}
	
	if (_play_upgrade_sound) {
		_upgrade_channel = Mix_PlayChannel(-1, _upgrade_sound, 0);
		_play_upgrade_sound = false;
	}

	if (_play_game_over_sound) {
		Mix_HaltMusic();
		_game_over_channel = Mix_PlayChannel(-1, _game_over_sound, 0);
		_play_game_over_sound = false;
	}
}

cv::Mat CSnakeGameV2::crt(cv::Mat input) {

	input = input.mul(_crt_mask);

	for (int row = 0; row < _canvas_size.height; row += 3)
		input.row(row) *= 0.8;

	cv::remap(input, input, _barrel_x, _barrel_y, cv::INTER_NEAREST, cv::BORDER_CONSTANT);
	
	return input;
}