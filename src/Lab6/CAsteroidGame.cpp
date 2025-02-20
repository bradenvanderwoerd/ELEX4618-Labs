#include "stdafx.h"
#include "cvui.h"
#include "glad.h"
#include <fstream>
#include "Lab6/CAsteroidGame.h"
#include "Lab6/CShip.h"
#include "Lab6/CPlanet.h"
#include "Lab6/CAsteroid.h"

#define DO_CONTROLLER true

#define CANVAS_NAME "Asteroids"
#define SHIP_INDEX 0
#define PLANET_INDEX 1

#define ORBIT_DISTANCE 30.0f
#define DEADZONE_PERCENT 20

#define UPDATE_TIME 20
#define SPAWN_TIME 2000

CAsteroidGame::CAsteroidGame(cv::Size size) {

	_window_size = size;

	_exit_flag = false;
	_reset_flag = false;

	// Move some stuff to setup_game
	_last_update_time = cv::getTickCount();
	_last_spawn_time = cv::getTickCount();

	_free_cam_enabled = false;

	_turn_input = 0;
	_thrust = false;
	_fire = false;

	_program_id = -1;

	setup_game();
}

CAsteroidGame::~CAsteroidGame() {
	for (auto obj : _game_objects) {
		delete obj;
	}
	
	if (_window) {
		glfwDestroyWindow(_window);
	}
	glfwTerminate();
}

void CAsteroidGame::close_game() {
	_exit_flag = true;
}

void CAsteroidGame::toggle_free_cam() {
	_free_cam_enabled = !_free_cam_enabled;
	if (!_free_cam_enabled)
		glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else
		glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void CAsteroidGame::run() {
	
	// Create threads
	std::thread t1(&CAsteroidGame::gpio_thread, this);
	std::thread t2(&CAsteroidGame::update_thread, this);
	std::thread t3(&CAsteroidGame::draw_thread, this);
	std::thread t4(&CAsteroidGame::sound_thread, this);

	t1.detach();
	t2.detach();
	t3.detach();
	t4.detach();

	do {
		Sleep(10);
	} while (!_exit_flag);
}

void CAsteroidGame::gpio_thread() {
	if (DO_CONTROLLER)
		_ctrl.init_com();
	
	do {
		gpio();
	} while (_exit_flag == false);
}

void CAsteroidGame::update_thread() {
	srand(time(0));

	do {
		update();
	} while (_exit_flag == false);
}

void CAsteroidGame::draw_thread() {
	// Create glfw window
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW\n";
		_window = nullptr;
		_exit_flag = true;
		return;
	}
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	_window = glfwCreateWindow(_window_size.width, _window_size.height, CANVAS_NAME, nullptr, nullptr);

	if (!_window) {
		std::cerr << "Failed to create GLFW window\n";
		glfwTerminate();
		_exit_flag = true;
		return;
	}
	glfwMakeContextCurrent(_window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD\n";
		glfwTerminate();
		_exit_flag = true;
		return;
	}
	glViewport(0, 0, _window_size.width, _window_size.height); // Set OpenGL viewport

	glfwSetWindowUserPointer(_window, this);
	glfwSetKeyCallback(_window, CAsteroidGame::key_callback);
	glfwSetCursorPosCallback(_window, CAsteroidGame::mouse_callback);
	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	glEnable(GL_DEPTH_TEST);

	_program_id = install_shaders();

	// Create GL objects!!

	do {
		double start_ticks = cv::getTickCount();
		//auto end_time = std::chrono::system_clock::now() + std::chrono::milliseconds(LOOP_PERIOD);

		draw();

		//std::this_thread::sleep_until(end_time);

		//_last_frame_time = (cv::getTickCount() - start_ticks) / cv::getTickFrequency();

	} while (_exit_flag == false);
}

void CAsteroidGame::sound_thread() {

	/*
	SDL_Init(SDL_INIT_AUDIO);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

	
	_music = Mix_LoadMUS("resources/snake.wav");
	_upgrade_sound = Mix_LoadWAV("resources/upgrade.wav");
	_game_over_sound = Mix_LoadWAV("resources/game_over.wav");
	*/

	do {
		sound();
	} while (_exit_flag == false);

	/*
	Mix_FreeMusic(_music);
	Mix_CloseAudio();
	SDL_Quit();
	*/
}

void CAsteroidGame::gpio() {
	int dummy = 1;
	if (!_ctrl.get_data(DIGITAL, 1, dummy)) {
		_ctrl.init_com();
		return;
	}

	float joy_x = _ctrl.get_analog(JOY_X);
	float joy_y = _ctrl.get_analog(JOY_Y);

	_game_mutex.lock();

	if (joy_x < 50 - DEADZONE_PERCENT || joy_x > 50 + DEADZONE_PERCENT)
		_turn_input = -(50 - joy_x) / 50;
	else
		_turn_input = 0;

	_thrust = joy_y > 50 + DEADZONE_PERCENT;

	_game_mutex.unlock();

	int fire_button = _ctrl.get_button(BUTTON1);
	int reset_button = _ctrl.get_button(BUTTON2);

	if (fire_button == 0)
		_fire = true;
	if (reset_button == 0)
		_reset_flag = true;
}

void CAsteroidGame::update() {
	// Change size logic to _start_game flag
	if (_game_objects.size() > 0 && 1000 * (cv::getTickCount() - _last_update_time) / cv::getTickFrequency() > UPDATE_TIME) {
		_game_mutex.lock();

		CShip* ship = dynamic_cast<CShip*>(_game_objects.at(SHIP_INDEX));
		ship->update_input(_turn_input, _thrust);
		for (auto obj : _game_objects) {
			obj->update_scene(_camera);
			obj->move();
		}

		_game_mutex.unlock();

		if (!_free_cam_enabled)
			_camera.follow_ship(_game_objects.at(SHIP_INDEX)->get_pos(), _game_objects.at(SHIP_INDEX)->get_dir());

		_last_update_time = cv::getTickCount();
	}

	if (_game_objects.size() > 0 && 1000 * (cv::getTickCount() - _last_spawn_time) / cv::getTickFrequency() > SPAWN_TIME) {
		// Add ship distance
		CGameObject* asteroid = new CAsteroid(_program_id, _window_size, ORBIT_DISTANCE);
		
		_game_mutex.lock();
		_create_gl_objects_index = _game_objects.size();
		_game_objects.push_back(asteroid);
		_game_mutex.unlock();

		_last_spawn_time = cv::getTickCount();
	}

	if (_fire) {
		std::cout << "Shots fired!" << std::endl;
		_fire = false;
	}

	if (_reset_flag) {
		std::cout << "Game reset!" << std::endl;
		_reset_flag = false;
	}
}

void CAsteroidGame::draw() {
	glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_game_mutex.lock();

	if (_create_gl_objects_index > -1)
		_game_objects.at(_create_gl_objects_index)->create_gl_objects();

	for (auto obj : _game_objects) {
		if (_create_gl_objects_index = -1)
			obj->create_gl_objects();
		obj->draw();
	}

	_create_gl_objects_index = -2;

	_game_mutex.unlock();

	glfwPollEvents();
	glfwSwapBuffers(_window);

	/*GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error: " << err << std::endl;
	}*/
}

void CAsteroidGame::sound() {
	Sleep(10);
}

void CAsteroidGame::setup_game() {
	CGameObject* ship = new CShip(_program_id, _window_size, ORBIT_DISTANCE);
	_game_objects.push_back(ship);

	CGameObject* planet = new CPlanet(_program_id, _window_size, ORBIT_DISTANCE);
	_game_objects.push_back(planet);

	_create_gl_objects_index = -1;
}

cv::Mat crt(cv::Mat input) {
	return input;
}

void CAsteroidGame::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	CAsteroidGame* game = static_cast<CAsteroidGame*>(glfwGetWindowUserPointer(window));
	if (game && key == GLFW_KEY_Q && action == GLFW_PRESS) {
		game->close_game();
	}

	else if (game && key == GLFW_KEY_M && action == GLFW_PRESS) {
		game->toggle_free_cam();
	}
	
	else if (game && key == GLFW_KEY_O && action == GLFW_PRESS) {
		game->get_camera()->look_at_origin();
	}

	else if (game && glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && game->free_cam_enabled())
		game->get_camera()->move_forward();
	else if (game && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && game->free_cam_enabled())
		game->get_camera()->move_backward();
	else if (game && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && game->free_cam_enabled())
		game->get_camera()->strafe_left();
	else if (game && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && game->free_cam_enabled())
		game->get_camera()->strafe_right();
	else if (game && glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && game->free_cam_enabled())
		game->get_camera()->move_up();
	else if (game && glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && game->free_cam_enabled())
		game->get_camera()->move_down();
}

void CAsteroidGame::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	CAsteroidGame* game = static_cast<CAsteroidGame*>(glfwGetWindowUserPointer(window));
	if (game && game->free_cam_enabled()) {
		game->get_camera()->mouse_update(glm::vec2(xpos, ypos));
	}
}

GLuint CAsteroidGame::install_shaders() {
	GLuint vertex_shader_id = glad_glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment_shader_id = glad_glCreateShader(GL_FRAGMENT_SHADER);

	std::string vertexCode = read_shader_code("shaders/vertex_shader.glsl");
	std::string fragmentCode = read_shader_code("shaders/fragment_shader.glsl");

	const GLchar* vertexSource = vertexCode.c_str();
	const GLchar* fragmentSource = fragmentCode.c_str();

	glShaderSource(vertex_shader_id, 1, &vertexSource, 0);
	glShaderSource(fragment_shader_id, 1, &fragmentSource, 0);

	glCompileShader(vertex_shader_id);
	glCompileShader(fragment_shader_id);

	if (!check_shader_status(vertex_shader_id) || !check_shader_status(fragment_shader_id))
		return -1;

	GLuint program_id = glCreateProgram();
	glAttachShader(program_id, vertex_shader_id);
	glAttachShader(program_id, fragment_shader_id);
	glLinkProgram(program_id);

	if (!check_program_status(program_id))
		return -1;

	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);

	glUseProgram(program_id);

	return program_id;
}

std::string CAsteroidGame::read_shader_code(std::string file_name) {
	std::ifstream infile;
	infile.open(file_name);
	
	return std::string(std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>());
}

bool CAsteroidGame::check_shader_status(GLuint shader_id) {
	GLint compile_status;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_status);
	if (compile_status != GL_TRUE) {
		GLint info_log_length; glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
		GLchar* buffer = new GLchar[info_log_length];

		GLsizei buffer_size;
		glGetShaderInfoLog(shader_id, info_log_length, &buffer_size, buffer);
		std::cout << buffer << std::endl;
		delete[] buffer;

		return false;
	}
	return true;
}

bool CAsteroidGame::check_program_status(GLuint program_id) {
	GLint link_status;
	glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);
	if (link_status != GL_TRUE) {
		GLint info_log_length; glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
		GLchar* buffer = new GLchar[info_log_length];

		GLsizei buffer_size;
		glGetProgramInfoLog(program_id, info_log_length, &buffer_size, buffer);
		std::cout << buffer << std::endl;
		delete[] buffer;

		return false;
	}
	return true;
}