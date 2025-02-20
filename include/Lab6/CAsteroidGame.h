#pragma once

#include "Lab4/CBase4618.h"
#include "Lab6/CGameObject.h"
#include "Lab6/CCamera.h"

#include <GLFW/glfw3.h>
#include "SDL.h"
#include "SDL_mixer.h"
#include "stdafx.h"
#include <mutex>

/**
*
* @brief Asteroids Game
*
* This class runs the game Asteroids by inheriting from CBase4618.
* The user controls a ship that can shoot and destroy incoming asteroids.
*
* @author Braden Vanderwoerd
*
*/
class CAsteroidGame : public CBase4618 {

	private:

		GLFWwindow* _window;

		cv::Size _window_size;

		std::vector<CGameObject*> _game_objects; ///< Game objects

		int _create_gl_objects_index;

		CCamera _camera;

		float _turn_input;

		bool _thrust;

		bool _fire;

		double _last_update_time;

		double _last_spawn_time;

		bool _free_cam_enabled;

		bool _exit_flag; ///< Flag to exit game

		bool _reset_flag;

		std::mutex _game_mutex; ///< Mutex to protect game attributes across threads

		GLuint _program_id;

		/** @brief Runs gpio() in a loop
		*
		* @return nothing to return
		*/
		void gpio_thread();

		/** @brief Runs update() in a loop
		*
		* @return nothing to return
		*/
		void update_thread();

		/** @brief Runs draw() in a loop
		*
		* @return nothing to return
		*/
		void draw_thread();

		/** @brief Runs sound() in a loop
		*
		* @return nothing to return
		*/
		void sound_thread();

		/** @brief Changes snake direction and colour and may reset game based on controller input
		*
		* @return nothing to return
		*/
		void gpio();

		/** @brief Updates game objects
		*
		* @return nothing to return
		*/
		void update();

		/** @brief Draws game objects and UI
		*
		* @return nothing to return
		*/
		void draw();

		/** @brief Plays sound effects
		*
		* @return nothing to return
		*/
		void sound();

		/** @brief Sets or resets all game objects
		*
		* @return nothing to return
		*/
		void setup_game();

		/** @brief Applies crt effect to Mat object
		*
		* @param Mat on which crt effect is to be applied
		* @return Mat with crt effect applied
		*/
		cv::Mat crt(cv::Mat input);

		/** @brief Handles key presses
		*
		* @return nothing to return
		*/
		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

		/** @brief Handles mouse movements
		*
		* @return nothing to return
		*/
		static void mouse_callback(GLFWwindow* window, double xpos, double ypos);

		/** @brief Compiles and links shaders
		*
		* @return program ID
		*/
		static GLuint install_shaders();

		/** @brief Reads code from glsl file for shader
		*
		* @param file_name Path of glsl file
		* @return Shader code as a string
		*/
		static std::string read_shader_code(std::string file_name);

		/** @brief Checks if shader compiled
		*
		* @param shader_id ID of shader
		* @return True if shader compiledwithout error
		*/
		static bool check_shader_status(GLuint shader_id);

		/** @brief Checks if program linked
		*
		* @param program_id ID of program
		* @return True if program linked without error
		*/
		static bool check_program_status(GLuint program_id);

	public:
		/** @brief Initializes members and auto-initializes COM port
		*
		* @param size Size of canvas
		*/
		CAsteroidGame(cv::Size size);

		/** @brief CSnakeGame deconstructor */
		~CAsteroidGame();

		/** @brief Gets CCamera object
		* 
		* @return CCamera object
		*/
		CCamera* get_camera() { return &_camera; }

		/** @brief Determines if mouse should updates camera
		*
		* @return Value of _do_camera_mouse_updates
		*/
		bool free_cam_enabled() { return _free_cam_enabled; }

		/** @brief Ends game
		*
		* @return nothing to return
		*/
		void close_game();

		/** @brief Toggles mouse camera updates
		*
		* @return nothing to return
		*/
		void toggle_free_cam();

		/** @brief Starts gpio, update, and draw threads and waits for command to quit
		*
		* @return nothing to return
		*/
		void run();
};