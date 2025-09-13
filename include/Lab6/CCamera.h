#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

/** @class CCamera
 *  @brief Camera class
 *
 *  This class handles camera movement and view direction
 * 
 *  @author Braden Vanderwoerd
 */
class CCamera {
	private:
		glm::vec3 _position; ///< Camera position

		glm::vec3 _view_direction; ///< Camera view direction

		glm::vec2 _old_mouse_position; ///< Previous mouse position

		glm::vec3 _relative_up; ///< Camera up direction

	public: 
		/** @brief CCamera constructor */
		CCamera(); 

		/** @brief CCamera destructor */
		~CCamera();

		/** @brief Follows the ship
		 *
		 * @param ship_position Ship position
		 * @param ship_velocity Ship velocity
		 * 
		 * @return nothing to return
		 */
		void follow_ship(glm::vec3 ship_position, glm::vec3 ship_velocity);

		/** @brief Updates camera position based on mouse movement
		 *
		 * @param new_mouse_position New mouse position
		 *
		 * @return nothing to return
		 */
		void mouse_update(glm::vec2 new_mouse_position);

		/** @brief Gets view matrix
		 *
		 * @return View matrix
		 */
		glm::mat4 get_view_matrix();

		/** @brief Gets position
		 *
		 * @return Position
		 */
		glm::vec3 get_pos() { return _position; }

		/** @brief Moves camera forward
		 *
		 * @return nothing to return
		 */
		void move_forward();

		/** @brief Moves camera backward
		 *
		 * @return nothing to return
		 */
		void move_backward();

		/** @brief Strafes camera left
		 *
		 * @return nothing to return
		 */
		void strafe_left();

		/** @brief Strafes camera right
		 *
		 * @return nothing to return
		 */
		void strafe_right();

		/** @brief Moves camera up
		 *
		 * @return nothing to return
		 */
		void move_up();

		/** @brief Moves camera down
		 *
		 * @return nothing to return
		 */
		void move_down();

		/** @brief Changes view direction to point to origin
		 *
		 * @return nothing to return
		 */
		void look_at_origin();

};

