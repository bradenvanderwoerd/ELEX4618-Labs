#pragma once
#include "Lab4\\CBase4618.h"

/**
*
* @brief Snake Game
*
* This class runs the game Snake by inheriting from CBase4618
*
* @author Braden Vanderwoerd
*
*/
class CSnakeGame : public CBase4618 {
	private:

		std::vector<cv::Point> _snake; ///< Segments of snake

		int _direction; ///< Current direction of snake

		int _previous_direction; ///< True if joystick returned to center

		int _colour; ///< Colour of snake

		bool _reset_flag; ///< Flag to reset game

		bool _exit_flag; ///< Flag to exit program

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
		CSnakeGame();

		/** @brief Initializes canvas to 600x600 and auto-initializes COM port
		*
		* @param size Size of canvas
		*/
		CSnakeGame(cv::Size size);

		/** @brief Initializes canvas to 600x600 and auto-initializes COM port
		* 
		* @param size Size of canvas
		* @param comport Number of COM port to be used
		*/
		CSnakeGame(cv::Size size, int comport);

		/** @brief CSnakeGame deconstructor */
		~CSnakeGame();

		/** @brief Runs game code in a loop
		*
		* @return nothing to return
		*/
		void run();
};

