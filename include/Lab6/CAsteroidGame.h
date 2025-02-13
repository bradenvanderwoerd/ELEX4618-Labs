#pragma once

#include "Lab4/CBase4618.h"
#include "Lab6/CGameObject.h"

#include <GLFW/glfw3.h>
#include "SDL.h"
#include "SDL_mixer.h"
#include "stdafx.h"

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

	std::vector<CGameObject> objects; ///< Game objects

	bool _exit_flag; ///< Flag to exit game

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

	/** @brief Applies crt effect to Mat object
	*
	* @param Mat on which crt effect is to be applied
	* @return Mat with crt effect applied
	*/
	cv::Mat crt(cv::Mat input);

	/** @brief Calls close_game when key is pressed
	*
	* @return nothing to return
	*/
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

public:
	/** @brief Initializes members and auto-initializes COM port
	*
	* @param size Size of canvas
	*/
	CAsteroidGame(cv::Size size);

	/** @brief CSnakeGame deconstructor */
	~CAsteroidGame();

	/** @brief Ends game
	*
	* @return nothing to return
	*/
	void close_game();

	/** @brief Starts gpio, update, and draw threads and waits for command to quit
	*
	* @return nothing to return
	*/
	void run();
};