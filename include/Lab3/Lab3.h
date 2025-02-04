#pragma once

#include "Lab3\\CControl.h"

/**
*
* @brief Tests CControl class
*
* This class tests the analog and digital functions of the CControl class
*
* @author Braden Vanderwoerd
*
*/
class Lab3 {
	private:

		CControl _ctrl; ///< Enables communication on COM port

		/** @brief Prints Lab 3 menu to command line
		*
		* @return nothing to return
		*/
		void print_menu();

		/** @brief Tests two analog channels
		*
		* @return nothing to return
		*/
		void test_analog();

		/** @brief Tests a digital channel
		*
		* @return nothing to return
		*/
		void test_digital();

		/** @brief Tests a digital channel with a button
		*
		* @return nothing to return
		*/
		void test_button();

		/** @brief Tests a servo
		*
		* @return nothing to return
		*/
		void test_servo();

	public:

		/** @brief Lab3 constructor*/
		Lab3();

		/** @brief Lab3 destructor*/
		~Lab3();

		/** @brief Prompts user to choose a test to run
		*
		* @return nothing to return
		*/
		void lab3main();
};