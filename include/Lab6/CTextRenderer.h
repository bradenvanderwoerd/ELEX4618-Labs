#pragma once

#include <map>
#include <string>
#include <glm/glm.hpp>
#include "glad.h"
#include <ft2build.h>
#include FT_FREETYPE_H

/**
 * @struct Character
 * @brief Represents a single character's rendering data.
 *
 * This struct stores information about a single glyph, including its texture,
 * size, bearing (offset from baseline), and advance distance for proper text spacing.
 * 
 * @author Braden Vanderwoerd
 */
struct Character {
    GLuint TextureID;  ///< Glyph texture ID

    glm::ivec2 Size;   ///< Glyph size

    glm::ivec2 Bearing;///< Offset from baseline

    GLuint Advance;    ///< Advance to next character
};

/** @class CTextRenderer
 *  @brief Text renderer class
 *
 *  This class renders text using FreeType
 *
 *  @author Braden Vanderwoerd
 */
class CTextRenderer {
    private:
		std::map<char, Character> Characters; ///< Map of characters

		GLuint VAO, VBO; ///< Vertex array object and vertex buffer object

    public:
		/** @brief CTextRenderer constructor
		 *
		 * @param fontPath Path to font file
		 */
        CTextRenderer(const std::string& fontPath);

		/** @brief CTextRenderer destructor */
        ~CTextRenderer();

		/** @brief Renders text
		 *	
		 * @param shader Shader program ID
		 * @param text Text to render
		 * @param x X position
		 * @param y Y position
		 * @param scale Scale of text
		 * @param color Text color
		 * 
		 * @return nothing to return
		 */
        void render_text(GLuint shader, std::string text, float x, float y, float scale, glm::vec3 color);
};