// 
// Created by Balajanovski on 29/05/2018.
//

#include "FontComponent.h"

#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H

FontComponent::FontComponent(const std::string &font_src) {
    FT_Library ft = nullptr;
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "Freetype error: Could not init FreeType Library\n";
    }

    FT_Face face = nullptr;
    if (FT_New_Face(ft, font_src.c_str(), 0, &face))
        std::cerr << "Freetype error: Failed to load font\n";
    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

    // Load the first 128 letters of the ASCII character set
    for (int c = 0; c < 128; ++c) {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cerr << "Freetype error: Failed to load Glyph\n";
            continue;
        }

        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
        );
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Glyph glyph = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<GLuint>(face->glyph->advance.x)
        };
        loaded_glyphs.insert(std::pair<GLchar, Glyph>(c, glyph));
    }

    // Free Freetype resources
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}