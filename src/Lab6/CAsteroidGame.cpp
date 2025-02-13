#include "stdafx.h"
#include "cvui.h"
#include "glad.h"
#include "Lab6/CAsteroidGame.h"

#define CANVAS_NAME "Asteroids"

CAsteroidGame::CAsteroidGame(cv::Size size) {

	_window_size = size;

	_exit_flag = false;

}

CAsteroidGame::~CAsteroidGame() {
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

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Optional: prevent resizing
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
	glfwSetKeyCallback(_window, CAsteroidGame::keyCallback);

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
	Sleep(10);
}

void CAsteroidGame::draw() {
	glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glfwSwapBuffers(_window);
	glfwPollEvents();
}

void CAsteroidGame::sound() {
	Sleep(10);
}

cv::Mat crt(cv::Mat input) {
	return input;
}

void CAsteroidGame::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	CAsteroidGame* game = static_cast<CAsteroidGame*>(glfwGetWindowUserPointer(window));
	if (game && key == GLFW_KEY_Q && action == GLFW_PRESS) {
		game->close_game();
	}
}