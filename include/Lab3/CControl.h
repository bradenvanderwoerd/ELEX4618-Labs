#pragma once

#include "Serial.h"

#define JOY_X 11
#define JOY_Y 4

#define BUTTON1 33
#define BUTTON2 32
#define LOWER_BUTTON1 31
#define LOWER_BUTTON2 17

#define RED_LED 39
#define GREEN_LED 38
#define BLUE_LED 37

enum { DIGITAL = 0, ANALOG, SERVO };

/**
*
* @brief Implements ELEX 4618 communication protocol on serial port
*
* This class implements the ELEX 4618 communication protocol using functions
* that allow reading and writing to digital and analog I/Os via a serial port
*
* @author Braden Vanderwoerd
*
*/
class CControl {
	private:

		Serial _com; ///< Serial port object

	public:

		/** @brief CControl constructor*/
		CControl();

		/** @brief CControl destructor*/
		~CControl();

		/** @brief Initializes serial port object
		*	
		* @param comport The number of the com port to be used
		* @return nothing to return
		*/
		void init_com(int comport);

		/** @brief Initializes serial port object (scans ports automatically)
		* 
		* @return nothing to return
		*/
		void init_com();

		/** @brief Gets data from the com port
		*
		* @param type The type of data that will be requested (DIGITAL, ANALOG, SERVO)
		* @param channel The pin from which the data will be requested 
		* @param result The variable to store the result
		* @return True is the get command is successful, false if request timed out
		*/
		bool get_data(int type, int channel, int& result);

		/** @brief Sets data on the com port
		*
		* @param type The type of data that will be set (DIGITAL, SERVO)
		* @param channel The pin on which the data will be set
		* @param val The data that will be set
		* @return True is the set command is successful, false if request timed out
		*/
		bool set_data(int type, int channel, int val);

		/** @brief Gets data from an analog pin
		*
		* @param analog_channel The analog channel from which the data will be requested
		* @return The percent of the maximum value on the analog channel
		*/
		float get_analog(int analog_channel);

		/** @brief Gets the state of a button
		*
		* @param button_number The button from which the state will be requested
		* @return 0 if the button is pressed, 1 if the button is not pressed
		*/
		int get_button(int button_number);
};