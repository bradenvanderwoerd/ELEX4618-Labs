#pragma once

#include "Lab3\\CControl.h"

/**
*
* @brief Provides a base class for ELEX 4618 lab programs that using multithreading
*
* This class provides a base for running an OpenCV image that interacts
* with GPIOs, updates, and draws on an OpenCV image. Each of these operations
* is run on a seperate thread
*
* @author Braden Vanderwoerd
*
*/
class CBase4618Threaded {
	protected:

		CControl _ctrl; ///< Enables communication on COM port

		cv::Mat _canvas; ///< Image to display

		bool _exit_flag; ///< Flag to exit program

		/** @brief Interacts with GPIOs
		* 
		* @return nothing to return
		*/
		virtual void gpio() = 0;

		/** @brief Updates in preperation for next frame
		*
		* @return nothing to return
		*/
		virtual void update() = 0;

		/** @brief Draws the next frame
		*
		* @return True if program is to quit before next frame
		*/
		virtual void draw() = 0;

	public:

		/** @brief CBase4618Threaded constructor */
		CBase4618Threaded();

		/** @brief CBase4618Threaded destrcutor */
		~CBase4618Threaded();

		/** @brief Calls gpio(), update(), and draw() on seperate threads and terminates when key is pressed
		* 
		* @return nothing to return
		*/
		void run();
};

