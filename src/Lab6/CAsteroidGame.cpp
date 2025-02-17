#include "stdafx.h"
#include "cvui.h"
#include "glad.h"
#include <fstream>
#include "Lab6/CAsteroidGame.h"
#include "Lab6/CShip.h"

#define CANVAS_NAME "Asteroids"
#define SHIP_INDEX 0
#define UPDATE_TIME 20

CAsteroidGame::CAsteroidGame(cv::Size size) {

	_window_size = size;

	_exit_flag = false;

	_last_update_time = cv::getTickCount();

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
		return;
	}

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	_window = glfwCreateWindow(_window_size.width, _window_size.height, CANVAS_NAME, nullptr, nullptr);

	if (!_window) {
		std::cerr << "Failed to create GLFW window\n";
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(_window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD\n";
		glfwTerminate();
		return;
	}

	glViewport(0, 0, _window_size.width, _window_size.height); // Set OpenGL viewport

	glfwSetWindowUserPointer(_window, this);
	glfwSetKeyCallback(_window, CAsteroidGame::key_callback);
	glEnable(GL_DEPTH_TEST);

	GLuint program_id = install_shaders();

	CGameObject* ship = new CShip(program_id, _window_size);
	_game_objects.push_back(ship);

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
	Sleep(10);
}

void CAsteroidGame::update() {
	if (_game_objects.size() > 0 && 1000 * (cv::getTickCount() - _last_update_time) / cv::getTickFrequency() > UPDATE_TIME) {
		_game_mutex.lock();
		_game_objects.at(SHIP_INDEX)->set_rotation(glm::vec3(
			_game_objects.at(SHIP_INDEX)->get_rotation().x,
			_game_objects.at(SHIP_INDEX)->get_rotation().y + 1,
			0));
		for (auto obj : _game_objects)
			obj->update();
		_game_mutex.unlock();

		_last_update_time = cv::getTickCount();
	}

	Sleep(10);
}

void CAsteroidGame::draw() {
	glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_game_mutex.lock();
	for (auto obj : _game_objects)
		obj->draw();
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

cv::Mat crt(cv::Mat input) {
	return input;
}

void CAsteroidGame::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	CAsteroidGame* game = static_cast<CAsteroidGame*>(glfwGetWindowUserPointer(window));
	if (game && key == GLFW_KEY_Q && action == GLFW_PRESS) {
		game->close_game();
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