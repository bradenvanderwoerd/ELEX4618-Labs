#pragma once
#include "Lab6/CGameObject.h"

/** 
 *  @class CPlanet
 *  @brief Planet class for asteroid game
 *
 *  This class represents a planet in the game.
 *  It inherits from CGameObject and contains methods for moving the planet.
 *
 *  @author Braden Vanderwoerd
 */
class CPlanet : public CGameObject {

	private:

	public:
		/** @brief CPlanet constructor */
		CPlanet();

		/** @brief CPlanet constructor
		 *
		 * @param window_size Size of the window
		 * @param orbit_distance Distance from origin
		 * @param program_id Shader program ID
		 */
		CPlanet(cv::Size window_size, GLfloat orbit_distance, GLuint program_id);
		
		/** @brief CPlanet destructor */
		~CPlanet();

		/** @brief Implements CGameObject move method
		 *
		 * @return nothing to return
		 */
		void move();
};
