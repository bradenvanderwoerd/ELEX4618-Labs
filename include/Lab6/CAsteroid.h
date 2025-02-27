#pragma once
#include "CGameObject.h"

/**  
 *  @class CAsteroid
 *  @brief Asteroid class for asteroid game
 *	
 *  This class represents an asteroid in the game.
 *  It inherits from CGameObject and contains methods for moving the asteroid.
 * 
 *  @author Braden Vanderwoerd
 */
class CAsteroid : public CGameObject {
	private:

		void set_position_on_hemi(glm::vec3 ship_position); ///< Sets position opposite to ship

		glm::vec3 random_vec3(); ///< Generates random vector

	public:

		/** @brief CAsteroid constructor
		 *
		 * @param window_size Size of the window
		 * @param orbit_distance Distance from origin
		 * @param program_id Shader program ID
		 * @param ship_position Ship position
		 */
		CAsteroid(cv::Size window_size, GLfloat orbit_distance, GLuint program_id, glm::vec3 ship_position = glm::vec3(0));
		
		/** @brief CAsteroid destructor */
		~CAsteroid();

		/** @brief Moves the asteroid
		 *
		 * @return nothing to return
		 */
		void move();
};

