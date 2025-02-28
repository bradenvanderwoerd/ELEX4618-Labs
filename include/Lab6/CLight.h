#pragma once

#include "glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "Lab6/CShip.h"
#include "Lab6/CPlanet.h"
#include "Lab6/CAsteroid.h"
#include "Lab6/CMissile.h"

class CLight {
	private:
		glm::vec3 _position; ///< Position of the light

		glm::vec3 _color; ///< Color of the light

		glm::mat4 _light_space_matrix; ///< Light space matrix

		GLuint _depth_map_FBO; ///< Depth map frame buffer object

		GLuint _depth_map; ///< Depth map texture

	public:
		/** @brief CLight default constructor */
		CLight();
		
		/** @brief CLight constructor
		 *
		 * @param pos Position of the light
		 * @param col Color of the light
		 */
		CLight(glm::vec3 pos, glm::vec3 col);

		/** @brief CLight destructor */
		~CLight();

		/** @brief Creates depth map
		 * 
		 * @return nothing to return
		 */
		void init_depth_map();

		/** @brief Updates light space matrix
		 *
		 * @return nothing to return
		 */
		void update_light_space_matrix();

		/** @brief Renders depth map
		 *
		 * @param depthShader Depth shader program ID
		 * @param ship Ship object
		 * @param asteroids Vector of asteroid objects
		 * @param missiles Vector of missile objects
		 * 
		 * @return nothing to return
		 */
		void render_depth_map(GLuint depth_shader, CShip*& ship, std::vector<CAsteroid*>& asteroids, std::vector<CMissile*>& missiles);

		/** @brief Gets position of the light
		 *
		 * @return Position of the light
		 */
		glm::vec3 get_pos() { return _position; }

		/** @brief Gets color of the light
		 *
		 * @return Color of the light
		 */
		glm::vec3 get_color() { return _color; }

		/** @brief Gets light space matrix
		 *
		 * @return Light space matrix
		 */
		glm::mat4 get_light_space_matrix() { return _light_space_matrix; }

		/** @brief Gets depth map
		 *
		 * @return Depth map
		 */
		GLuint get_depth_map() { return _depth_map; }
};

