#pragma once

#include "Lab4/CBase4618.h"
#include "Lab6/CShip.h"
#include "Lab6/CPlanet.h"
#include "Lab6/CAsteroid.h"
#include "Lab6/CMissile.h"
#include "Lab6/CCamera.h"
#include "Lab6/CTextRenderer.h"

#include <GLFW/glfw3.h>
#include "SDL.h"
#include "SDL_mixer.h"
#include "stdafx.h"
#include <mutex>

/**
* @class CAsteroidGame
* @brief Asteroids Game
*
* This class runs the game Asteroids by inheriting from CBase4618.
* The user controls a ship that can shoot and destroy incoming asteroids.
*
* @author Braden Vanderwoerd
*/
class CAsteroidGame : public CBase4618 {

private:

	GLFWwindow* _window; ///< Pointer to GLFW window

	cv::Size _window_size; ///< Size of the game window

	CTextRenderer* _text_renderer; ///< Pointer to text renderer

	CShip* _ship;

	CPlanet* _planet;

	std::vector<CAsteroid*> _asteroids;

	std::vector<CMissile*> _missiles;

	bool _game_started;

	int _create_new_asteroid;

	int _create_new_missile;

	CCamera _camera; ///< Camera object for handling view

	bool _controller_connected;

	float _turn_input; ///< Player turn input

	bool _thrust; ///< Flag indicating if thrust is active

	bool _fire; ///< Flag indicating if fire is active

	int _score; ///< Player score

	double _last_update_time; ///< Last update time for game loop

	double _last_asteroid_spawn_time; ///< Last time an asteroid was spawned

	double _last_missile_spawn_time; ///< Last time a missile was spawned

	bool _free_cam_enabled; ///< Flag indicating if free camera mode is enabled

	bool _exit_flag; ///< Flag to exit game

	bool _reset_flag; ///< Flag to reset game

	std::mutex _game_mutex; ///< Mutex to protect game attributes across threads

	GLuint _program_id; ///< OpenGL program ID

	GLuint _text_program_id; ///< OpenGL text program ID

	std::thread _gpio_thread; ///< Thread for handling GPIO inputs

	std::thread _update_thread; ///< Thread for handling game updates

	std::thread _draw_thread; ///< Thread for handling rendering

	std::thread _sound_thread; ///< Thread for handling sound effects

	/** @brief Runs gpio() in a loop */
	void gpio_thread();

	/** @brief Runs update() in a loop */
	void update_thread();

	/** @brief Runs draw() in a loop */
	void draw_thread();

	/** @brief Runs sound() in a loop */
	void sound_thread();

	/** @brief Changes ship direction and may reset game based on controller input */
	void gpio();

	/** @brief Updates game objects */
	void update();

	/** @brief Draws game objects and UI */
	void draw();

	/** @brief Plays sound effects */
	void sound();

	/** @brief Sets or resets all game objects */
	void setup_game();

	/** @brief Handles key presses */
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	/** @brief Handles mouse movements */
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);

	/** @brief Compiles and links shaders
	*  @return Program ID
	*/
	static GLuint install_shaders();

	/** @brief Compiles and links text shaders
	*  @return Text program ID
	*/
	static GLuint install_text_shaders();

	/**
	* @brief Reads code from GLSL file for shader
	*
	* @param file_name Path of GLSL file
	* @return Shader code as a string
	*/
	static std::string read_shader_code(std::string file_name);

	/**
	* @brief Checks if shader compiled successfully
	*
	* @param shader_id ID of shader
	* @return True if shader compiled without error
	*/
	static bool check_shader_status(GLuint shader_id);

	/**
	* @brief Checks if program linked successfully
	*
	* @param program_id ID of program
	* @return True if program linked without error
	*/
	static bool check_program_status(GLuint program_id);

public:
	/**
	* @brief Initializes members and auto-initializes COM port
	*
	* @param size Size of canvas
	*/
	CAsteroidGame(cv::Size size);

	/** @brief CAsteroidGame destructor */
	~CAsteroidGame();

	/**
	* @brief Gets CCamera object
	*
	* @return Pointer to CCamera object
	*/
	CCamera* get_camera() { return &_camera; }

	/**
	* @brief Determines if mouse should update camera
	*
	* @return Value of _free_cam_enabled flag
	*/
	bool free_cam_enabled() { return _free_cam_enabled; }

	/** @brief Ends game */
	void close_game();

	/** @brief Toggles free camera mode */
	void toggle_free_cam();

	/** @brief Starts gpio, update, draw, and sound threads and waits for command to quit */
	void run();
};
