#ifndef FONT_HPP
#define FONT_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <GLES2/gl2.h>
#include <string>

class Font {
public:
    Font();
    ~Font();

    bool load(const std::string& path, int size);

    // Returns texture ID, width and height of the generated text texture
    void renderText(const std::string& text, SDL_Color color, GLuint& outTexture, int& outWidth, int& outHeight);

    // Renders the generated texture to screen using a simple shader (assuming an external shader setup or simple immediate mode if available)
    // For GLES2 we need a simple shader to draw the texture. We'll add a helper for drawing 2D quads.
    void drawText(GLuint texture, int x, int y, int width, int height, float scaleX, float scaleY);

private:
    TTF_Font* m_font;
    GLuint m_shaderProgram;
    GLuint m_vbo;
    GLint m_posAttrib;
    GLint m_texAttrib;
    GLint m_samplerUniform;
    GLint m_projUniform;

    bool initShader();
};

#endif // FONT_HPP
