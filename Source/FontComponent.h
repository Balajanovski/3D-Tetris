// 
// Created by Balajanovski on 29/05/2018.
//

#ifndef INC_3D_TETRIS_FONTCOMPONENT_H
#define INC_3D_TETRIS_FONTCOMPONENT_H

#include <glad/glad.h>
#include <glm/vec2.hpp>

#include <string>
#include <map>

struct Glyph {
    GLuint texture_id;  // ID handle of the glyph texture
    glm::ivec2 size;    // Size of glyph
    glm::ivec2 bearing; // Offset from baseline to left/top of glyph
    GLuint advance;     // Offset to advance to next glyph
};

class FontComponent {
public:
    FontComponent(const std::string& font_src);
    Glyph fetch_glyph(GLchar c) { return loaded_glyphs[c]; }
private:
    std::map<GLchar, Glyph> loaded_glyphs;
};


#endif //INC_3D_TETRIS_FONTCOMPONENT_H
