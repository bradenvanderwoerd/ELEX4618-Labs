#include "stdafx.h"
#include "cvui.h"
#include "glad.h"
#include <fstream>
#include "Lab6/CAsteroidGame.h"

#define DO_CONTROLLER true
#define DO_SOUND false

#define CANVAS_NAME "Asteroids"
#define SHIP_INDEX 0
#define PLANET_INDEX 1

#define STARTING_ASTEROID_COUNT 1
#define MAX_ASTEROIDS 20

#define ORBIT_DISTANCE 30.0f
#define DEADZONE_PERCENT 20

#define UPDATE_TIME 8
#define LOOP_PERIOD 10
#define ASTEROID_SPAWN_TIME 200
#define MISSILE_SPAWN_TIME 150

CAsteroidGame::CAsteroidGame(cv::Size size) {

	_game_started = false;
	_controller_connected = false;

	_window_size = size;

	_exit_flag = false;
	_reset_flag = false;

	_create_new_asteroid = 0;
	_create_new_missile = 0;

	_last_update_time = cv::getTickCount();
	_last_asteroid_spawn_time = cv::getTickCount();
	_last_missile_spawn_time = cv::getTickCount();

	_free_cam_enabled = false;

	_turn_input = 0;
	_thrust = false;
	_fire = false;
	_do_funny_text = false;

	_score = 0;
}

CAsteroidGame::~CAsteroidGame() {

	_exit_flag = true;

	if (_gpio_thread.joinable()) {
		std::cout << "Joining GPIO thread..." << std::endl;
		_gpio_thread.join();
		std::cout << "GPIO thread exited." << std::endl;
	}
	if (_update_thread.joinable()) {
		std::cout << "Joining Update thread..." << std::endl;
		_update_thread.join();
		std::cout << "Update thread exited." << std::endl;
	}
	if (_draw_thread.joinable()) {
		std::cout << "Joining Draw thread..." << std::endl;
		_draw_thread.join();
		std::cout << "Draw thread exited." << std::endl;
	}
	if (_sound_thread.joinable()) {
		std::cout << "Joining Sound thread..." << std::endl;
		_sound_thread.join();
		std::cout << "Sound thread exited." << std::endl;
	}

	if (_window) {
		glfwMakeContextCurrent(nullptr);  // Ensure no context is active
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
	_gpio_thread = std::thread(&CAsteroidGame::gpio_thread, this);
	_update_thread = std::thread(&CAsteroidGame::update_thread, this);
	_draw_thread = std::thread(&CAsteroidGame::draw_thread, this);
	_sound_thread = std::thread(&CAsteroidGame::sound_thread, this);

	do {
		Sleep(1);
	} while (!_exit_flag);
}

void CAsteroidGame::gpio_thread() {
	_controller_connected = _ctrl.init_com();
	
	while (DO_CONTROLLER && !_controller_connected && !_exit_flag) {
		std::cout << "Retrying auto-connect in 3 seconds..." << std::endl;
		Sleep(3000);
		_controller_connected = _ctrl.init_com();
	}
	
	do {
		if (_game_started)
			gpio();
	} while (_exit_flag == false);
}

void CAsteroidGame::update_thread() {
	srand(time(0));

	do {
		if (_game_started)
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
	_text_renderer = new CTextRenderer("fonts/Arial.ttf");
	_text_program_id = install_text_shaders();

	setup_game();
	
	do {
		double start_ticks = cv::getTickCount();
		auto end_time = std::chrono::system_clock::now() + std::chrono::milliseconds(LOOP_PERIOD);

		draw();

		//std::this_thread::sleep_until(end_time);

		//_last_frame_time = (cv::getTickCount() - start_ticks) / cv::getTickFrequency();

	} while (_exit_flag == false);
}

void CAsteroidGame::sound_thread() {

	SDL_Init(SDL_INIT_AUDIO);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

	_music = Mix_LoadMUS("resources/asteroids.wav");
	
	if (DO_SOUND)
		Mix_PlayMusic(_music, -1);

	do {
		Sleep(10);
		//sound();
	} while (_exit_flag == false);

	Mix_FreeMusic(_music);
	Mix_CloseAudio();
	SDL_Quit();
}

void CAsteroidGame::gpio() {
	// Implement auto connection!
	int dummy = -1;
	_controller_connected = _ctrl.get_data(DIGITAL, 0, dummy);
	if (!_controller_connected) {
		_turn_input = 0;
		_thrust = false;
		std::cout << "Controller disconnected" << std::endl;
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

	int fire_button	= _ctrl.get_button(BUTTON1);
	int reset_button = _ctrl.get_button(BUTTON2);
	int funny_button = _ctrl.get_button(LOWER_BUTTON1);

	if (fire_button == 0)
		_fire = true;
	else if (fire_button == 1)
		_fire = false;

	if (reset_button == 0)
		_reset_flag = true;

	if (funny_button == 0)
		_do_funny_text = true;
	else if (funny_button == 1)
		_do_funny_text = false;
}

void CAsteroidGame::update() {
	// Change size logic to _start_game flag
	if (1000 * (cv::getTickCount() - _last_update_time) / cv::getTickFrequency() > UPDATE_TIME) {
		_game_mutex.lock();

		_ship->update_input(_turn_input, _thrust);
		_ship->update_scene(_camera);
		_ship->move();

		_planet->update_scene(_camera);

		for (CAsteroid* asteroid : _asteroids) {
			asteroid->update_scene(_camera);
			asteroid->move();
		}

		for (CMissile* missile : _missiles) {
			missile->update_scene(_camera);
			missile->move();
		}

		_game_mutex.unlock();

		if (!_free_cam_enabled)
			_camera.follow_ship(_ship->get_pos(), _ship->get_dir());

		_last_update_time = cv::getTickCount();
	}

	if (1000 * (cv::getTickCount() - _last_asteroid_spawn_time) / cv::getTickFrequency() > ASTEROID_SPAWN_TIME && _asteroids.size() < MAX_ASTEROIDS) {
		// Add ship distance
		_create_new_asteroid++;
		_last_asteroid_spawn_time = cv::getTickCount();
	}

	if (_fire && 1000 * (cv::getTickCount() - _last_missile_spawn_time) / cv::getTickFrequency() > MISSILE_SPAWN_TIME) {
		_create_new_missile++;
		_last_missile_spawn_time = cv::getTickCount();
	}

	if (_reset_flag) {
		std::cout << "Game reset!" << std::endl;
	}

	CShip* ship_copy;
	std::vector<CAsteroid*> asteroids_copy;
	std::vector<CMissile*> missiles_copy;

	_game_mutex.lock();

	ship_copy = _ship;
	asteroids_copy = _asteroids;
	missiles_copy = _missiles;

	_game_mutex.unlock();

	_ship_hit = false;

	for (int asteroid_index = 0; asteroid_index < asteroids_copy.size(); asteroid_index++) {
		if (ship_copy->collide(*asteroids_copy.at(asteroid_index))) {

			asteroids_copy.at(asteroid_index)->hit();
			_ship_hit = true;
			
			/*_ship_hit = true;

			_game_mutex.lock();
			_asteroids_to_remove.push_back(asteroid_index);
			_game_mutex.unlock();*/
		}

		for (int missile_index = 0; missile_index < missiles_copy.size(); missile_index++) {

			if (asteroids_copy.at(asteroid_index)->collide(*missiles_copy.at(missile_index))) {
				asteroids_copy.at(asteroid_index)->hit();
				missiles_copy.at(missile_index)->hit();

				/*_game_mutex.lock();
				_asteroids_to_remove.push_back(asteroid_index);
				_missiles_to_remove.push_back(missile_index);
				_game_mutex.unlock();*/

				break;
			}
			else if (glm::dot(missiles_copy.at(missile_index)->get_pos(), ship_copy->get_pos()) < 0) {
				missiles_copy.at(missile_index)->hit();

				/*_game_mutex.lock();
				_missiles_to_remove.push_back(missile_index);
				_game_mutex.unlock();*/

				break;
			}
		}
	}
}

void CAsteroidGame::draw() {
	glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_game_mutex.lock();

	while (_create_new_asteroid > 0) {
		CAsteroid* asteroid = new CAsteroid(_window_size, ORBIT_DISTANCE, _program_id, _ship->get_pos());
		_asteroids.push_back(asteroid);
		_create_new_asteroid--;
	}

	while (_create_new_missile > 0) {
		CMissile* missile = new CMissile(_window_size, ORBIT_DISTANCE, _ship, _program_id);
		_missiles.push_back(missile);
		_create_new_missile--;
	}

	if (_ship_hit) {
		_ship->hit();
		_score -= 150;
		_ship_hit = false;
	}

	for (CAsteroid* asteroid : _asteroids) {
		if (asteroid->get_lives() < 0) {
			_score += 150;
			delete asteroid;
			_asteroids.erase(std::remove(_asteroids.begin(), _asteroids.end(), asteroid), _asteroids.end());
		}
	}

	for (CMissile* missile : _missiles) {
		if (missile->get_lives() < 0) {
			delete missile;
			_missiles.erase(std::remove(_missiles.begin(), _missiles.end(), missile), _missiles.end());
		}
	}

	_ship->draw();
	_planet->draw();

	for (CAsteroid* asteroid : _asteroids)
		asteroid->draw();

	for (CMissile* missile : _missiles)
		missile->draw();

	_game_mutex.unlock();

	std::string hud_text = "Score: " + std::to_string(_score) +
						 "  Lives: " + std::to_string(_ship->get_lives()) + 
						 "  Asteroids: " + std::to_string(_asteroids.size()) + 
						 "  Missiles: " + std::to_string(_missiles.size());

	if (_do_funny_text)
		hud_text = "There's no sound in space, Craig.";

	_text_renderer->render_text(_text_program_id, hud_text, 25.0f, 50.0f, 0.5f, glm::vec3(1));


	glfwPollEvents();
	glfwSwapBuffers(_window);

	if (_reset_flag) {
		setup_game();
		_reset_flag = false;
	}
}

void CAsteroidGame::sound() {
	
	
	Sleep(1);
}

void CAsteroidGame::setup_game() {
	_ship = new CShip(_window_size, ORBIT_DISTANCE, _program_id);
	_ship->update_scene(_camera);

	_planet = new CPlanet(_window_size, ORBIT_DISTANCE, _program_id);
	_planet->update_scene(_camera);

	_asteroids.clear();
	for (int asteroid_count = 0; asteroid_count < STARTING_ASTEROID_COUNT; asteroid_count++) {
		CAsteroid* asteroid = new CAsteroid(_window_size, ORBIT_DISTANCE, _program_id);
		asteroid->update_scene(_camera);
		_asteroids.push_back(asteroid);
	}

	_missiles.clear();

	_score = 0;

	_game_started = true;
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

	std::string vertex_code = read_shader_code("shaders/vertex_shader.glsl");
	std::string fragment_code = read_shader_code("shaders/fragment_shader.glsl");

	const GLchar* vertex_source = vertex_code.c_str();
	const GLchar* fragment_source = fragment_code.c_str();

	glShaderSource(vertex_shader_id, 1, &vertex_source, 0);
	glShaderSource(fragment_shader_id, 1, &fragment_source, 0);

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

GLuint CAsteroidGame::install_text_shaders() {
	GLuint text_vertex_id = glad_glCreateShader(GL_VERTEX_SHADER);
	GLuint text_fragment_id = glad_glCreateShader(GL_FRAGMENT_SHADER);

	std::string text_vertex_code = read_shader_code("shaders/text_vertex.glsl");
	std::string text_fragment_code = read_shader_code("shaders/text_fragment.glsl");

	const GLchar* text_vertex_source = text_vertex_code.c_str();
	const GLchar* text_fragment_source = text_fragment_code.c_str();
	 
	glShaderSource(text_vertex_id, 1, &text_vertex_source, 0);
	glShaderSource(text_fragment_id, 1, &text_fragment_source, 0);

	glCompileShader(text_vertex_id);
	glCompileShader(text_fragment_id);

	if (!check_shader_status(text_vertex_id) || !check_shader_status(text_fragment_id))
		return -1;

	GLuint program_id = glCreateProgram();
	glAttachShader(program_id, text_vertex_id);
	glAttachShader(program_id, text_fragment_id);
	glLinkProgram(program_id);

	if (!check_program_status(program_id))
		return -1;

	glDeleteShader(text_vertex_id);
	glDeleteShader(text_fragment_id);

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