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

		std::vector<cv::Point> _apples; ///< Apple points

		int _direction; ///< Direction of snake

		int _colour; ///< Colour of snake

		int _score; ///< Player score

		bool _reset_flag; ///< Flag to reset game

		bool _exit_flag; ///< Flag to exit game

		std::mutex _snake_mutex; ///< Mutex to make snake vector thread-safe

		cv::Size _canvas_size; ///< Size of the canvas

		int _snake_size; ///< Number of pixels per snake step

		int _snake_speed; ///< Number of ms for each snake update

		double _last_update_tick; ///< Number of ticks when the snake was most recently updated

		double _last_frame_time; ///< Time it took for the last frame to draw

		double _last_apple_tick; ///< Number of ticks when the last apple spawned

		int _fps; ///< Frames per second (updated at the same rate as the snake)

		bool _game_over;

		cv::Mat _crt_mask;

		bool _do_crt;

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

	public:

		/** @brief Initializes members and auto-initializes COM port
		*
		* @param size Size of canvas
		*/
		CSnakeGameV2(cv::Size size);

		/** @brief CSnakeGame deconstructor */
		~CSnakeGameV2();

		/** @brief Starts gpio, update, and draw threads and waits for command to quit
		*
		* @return nothing to return
		*/
		void run();
};

