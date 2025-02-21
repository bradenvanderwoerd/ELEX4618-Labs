#pragma once

#include <map>
#include <string>
#include <glm/glm.hpp>
#include "glad.h"
#include <ft2build.h>
#include FT_FREETYPE_H

struct Character {
    GLuint TextureID;  // Glyph texture ID
    glm::ivec2 Size;   // Glyph size
    glm::ivec2 Bearing;// Offset from baseline
    GLuint Advance;    // Advance to next character
};

class CTextRenderer {
    public:
        std::map<char, Character> Characters;
        GLuint VAO, VBO;

        CTextRenderer(const std::string& fontPath);
        ~CTextRenderer();
        void render_text(GLuint shader, std::string text, float x, float y, float scale, glm::vec3 color);
};