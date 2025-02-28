#pragma once
#include "Lab6/CGameObject.h"

/**
 * @class CShip
 * @brief Ship class for asteroid game
 * 
 * This class represents the player's ship in the game.
 * It inherits from CGameObject and contains methods for moving the ship.
 *
 * @author Braden Vanderwoerd
 */
class CShip : public CGameObject {
	
	private:
		float _turn_input; ///< Turn input

		bool _thrust; ///< Thrust flag

	public:
		/** @brief CShip constructor */
		CShip();

		/** @brief CShip constructor
		 *
		 * @param window_size Size of the window
		 * @param orbit_distance Distance from origin
		 * @param program_id Shader program ID
		 */
		CShip(cv::Size window_size, GLfloat orbit_distance, GLuint program_id);

		/** @brief CShip destructor */
		~CShip();

		/** @brief Moves the ship
		 *
		 * @return nothing to return
		 */
		void move();

		/** @brief Updates ship input
		 *
		 * @param turn_input Turn input
		 * @param thrust Thrust flag
		 * @return nothing to return
		 */
		void update_input(float turn_input, bool thrust);
};
