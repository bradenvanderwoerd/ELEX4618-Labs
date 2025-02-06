#pragma once
#include "Lab4\\CBase4618.h"

/**
*
* @brief Snake Game V2
*
* This class runs the game Snake by inheriting from CBase4618Threaded.
* Version 2 improves on version 1 by adding colisions, variable snake speed and size,
* and a fixed frame rate.
*
* @author Braden Vanderwoerd
*
*/
class CSnakeGameV2 : public CBase4618 {
	private:

		std::vector<cv::Point> _snake; ///< Segments of snake

		int _direction; ///< Direction of snake

		int _colour; ///< Colour of snake

		int _score; ///< Player score

		bool _reset_flag; ///< Flag to reset game

		bool _exit_flag; ///< Flag to exit game

		std::mutex _snake_mutex; ///< Mutex to make snake vector thread-safe

		cv::Size _canvas_size; ///< Size of the canvas

		int _snake_speed; ///< Number of ms for each snake update

		double _last_update_tick; ///< Number of ticks when the snake was most recently updated

		double _last_frame_time; ///< Time it took for the last frame to draw

		int _fps;

		/** @brief Changes _direction based on joystick input
		*
		* @return nothing to return
		*/
		void gpio_thread();

		/** @brief Changes _direction based on joystick input
		*
		* @return nothing to return
		*/
		void update_thread();

		/** @brief Changes _direction based on joystick input
		*
		* @return nothing to return
		*/
		void draw_thread();

		/** @brief Changes _direction based on joystick input
		*
		* @return nothing to return
		*/
		void gpio();

		/** @brief Advances snake to next location
		*
		* @return nothing to return
		*/
		void update();

		/** @brief Draws snake in new location
		*
		* @return nothing to return
		*/
		void draw();

		/** @brief Resets the snake to the middle of the canvas
		* 
		* @return nothing to return
		*/
		void reset();

	public:

		/** @brief Initializes canvas to 600x600 and auto-initializes COM port */
		CSnakeGameV2();

		/** @brief Initializes canvas to 600x600 and auto-initializes COM port
		*
		* @param size Size of canvas
		*/
		CSnakeGameV2(cv::Size size);

		/** @brief Initializes canvas to 600x600 and auto-initializes COM port
		* 
		* @param size Size of canvas
		* @param comport Number of COM port to be used
		*/
		CSnakeGameV2(cv::Size size, int comport);

		/** @brief CSnakeGame deconstructor */
		~CSnakeGameV2();

		/** @brief Starts gpio, update, and draw threads and waits for command to quit
		*
		* @return nothing to return
		*/
		void run();
};

