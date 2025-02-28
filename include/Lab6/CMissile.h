#pragma once
#include "Lab6/CGameObject.h"
#include "Lab6/CShip.h"

/**  
 *  @class CMissile
 *  @brief Missile class for asteroid game
 *
 *  This class represents a missile in the game that is fired by the player's ship.
 *  It inherits from CGameObject and contains methods for moving the missile.
 *
 *  @author Braden Vanderwoerd
 */
class CMissile : public CGameObject {

	private:


	public:
		/** @brief CMissile constructor
		 *
		 * @param window_size Size of the window
		 * @param orbit_distance Distance from origin
		 * @param ship Ship object
		 * @param program_id Shader program ID
		 */
		CMissile(cv::Size window_size, GLfloat orbit_distance, CShip* ship, GLuint program_id);

		/** @brief CMissile destructor */
		~CMissile();

		/** @brief Moves the missile
		 *
		 * @return nothing to return
		 */
		void move();

};

