#pragma once
#include "glad.h"
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "stdafx.h"
#include "Lab6/CCamera.h"

#define FOV 60.0f
#define NEAR_PLANE 0.1f
#define FAR_PLANE 100.0f
#define DTIME 0.02f

/**
 * @class CGameObject
 * @brief Game object class
 *
 * This class represents a game object in the scene
 *
 * This class is an abstract class that represents a game object in the scene.
 * It contains members and methods that are common to all game objects.
 *
 * @author Braden Vanderwoerd
 */
class CGameObject {
	protected:

		cv::Size _window_size; ///< Size of the window

		glm::vec3 _position; ///< Position of the object

		GLfloat _orbit_distance; ///< Distance from origin

		glm::vec3 _velocity; ///< Velocity of the object
		
		glm::vec3 _direction; ///< Direction of the object

		glm::vec3 _rotation; ///< Rotation of the object

		glm::vec3 _scale; ///< Scale of the object

		GLfloat _shininess; ///< Shininess of the object

		GLfloat _specular_strength; ///< Specular strength of the object

		float _radius; ///< Radius of the object

		int _lives; ///< Number of lives

		GLuint _VAO; ///< Vertex array object

		GLuint _VBO; ///< Vertex buffer object

		GLuint _EBO; ///< Element buffer object

		GLuint _program_id; ///< Shader program ID

		std::vector<GLfloat> _vertices; ///< Vertex data

		std::vector<GLuint> _indices; ///< Index data

		std::vector<glm::vec3> _normals; ///< Normal data

		glm::mat4 _model_matrix; ///< Model matrix

		glm::mat4 _view_matrix; ///< View matrix

		glm::mat4 _projection_matrix; ///< Projection matrix

		glm::vec3 _camera_pos; ///< Camera position

		glm::vec3 _light_pos; ///< Light position

		glm::vec3 _light_color; ///< Light color

		glm::mat4 _light_space_matrix; ///< Light space matrix

		GLuint _depth_map; ///< Depth map texture

	public:

		/** @brief CGameObject constructor */
		CGameObject();

		/** @brief CGameObject destructor */
		~CGameObject();

		/** @brief Moves the object 
		 * 
		 * @return nothing to return
		 */
		virtual void move() = 0;

		/** @brief Checks for collision with another object
		 *
		 * @param obj Object to check for collision with
		 * @return True if collision occurred
		 */
		bool collide(CGameObject& obj);

		/** @brief Handles hit event
		 *
		 * @return nothing to return
		 */
		void hit();

		/** @brief Gets number of lives
		 *
		 * @return Number of lives
		 */
		int get_lives() { return _lives; }

		/** @brief Sets number of lives
		 *
		 * @param lives Number of lives
		 * @return nothing to return
		 */
		void set_lives(int lives) { _lives = lives; }

		/** @brief Sets radius
		 *
		 * @param radius Radius
		 * @return nothing to return
		 */
		float get_radius() { return _radius; }

		/** @brief Sets radius
		 *
		 * @param radius Radius
		 * @return nothing to return
		 */
		void set_pos(glm::vec3 pos) { _position = pos; }

		/** @brief Gets position
		 *
		 * @return Position
		 */
		glm::vec3 get_pos() { return _position; }

		/** @brief Sets velocity
		 *
		 * @param vel Velocity
		 * @return nothing to return
		 */
		void set_vel(glm::vec3 vel) { _velocity = vel; }

		/** @brief Gets velocity
		 *
		 * @return Velocity
		 */
		glm::vec3 get_vel() { return _velocity; }
		
		/** @brief Sets direction
		 *
		 * @param dir Direction
		 * @return nothing to return
		 */
		void set_dir(glm::vec3 dir) { _direction = dir; }

		/** @brief Gets direction
		 *
		 * @return Direction
		 */
		glm::vec3 get_dir() { return _direction; }

		/** @brief Sets rotation
		 *
		 * @param rotation rotation
		 * @return nothing to return
		 */
		void set_rotation(glm::vec3 rot) { _rotation = rot; }

		/** @brief Gets rotation
		 *
		 * @return Rotation
		 */
		glm::vec3 get_rotation() { return _rotation; }

		/** @brief Gets model matrix
		 * 
		 * @return Model matrix
		 */
		glm::mat4 get_model_matrix() { return _model_matrix; }

		/** @brief Gets projection matrix
		 *
		 * @return projection matrix
		 */
		glm::mat4 get_projection_matrix() { return _projection_matrix; }

		/** @brief Draws object
		 *
		 * @return nothing to return
		 */
		void draw();

		/** @brief Updates scene
		 *
		 * @param camera Camera object
		 * @return nothing to return
		 */
		virtual void update_scene(CCamera camera, glm::vec3 light_pos, glm::vec3 light_color, glm::mat4 light_space_matrix, GLuint depth_map);

		/** @brief Creates OpenGL objects
		 *
		 * @return nothing to return
		 */
		void create_gl_objects();

		/** @brief Computes vertex normals
		 *
		 * @return nothing to return
		 */
		void compute_vertex_normals();
};

