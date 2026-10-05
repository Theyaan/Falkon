#include "Font.hpp"
#include <iostream>
#include <vector>

const char* vertexShaderSource = R"(
    attribute vec2 position;
    attribute vec2 texcoord;
    varying vec2 v_texcoord;
    uniform mat4 projection;
    void main() {
        gl_Position = projection * vec4(position, 0.0, 1.0);
        v_texcoord = texcoord;
    }
)";

const char* fragmentShaderSource = R"(
    precision mediump float;
    varying vec2 v_texcoord;
    uniform sampler2D texSampler;
    void main() {
        gl_FragColor = texture2D(texSampler, v_texcoord);
    }
)";

Font::Font() : m_font(nullptr), m_shaderProgram(0), m_vbo(0) {
    if (!TTF_WasInit()) {
        if (TTF_Init() == -1) {
            std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
        }
    }
}

Font::~Font() {
    if (m_font) {
        TTF_CloseFont(m_font);
        m_font = nullptr;
    }
    if (m_vbo) {
        glDeleteBuffers(1, &m_vbo);
    }
    if (m_shaderProgram) {
        glDeleteProgram(m_shaderProgram);
    }
}

bool Font::load(const std::string& path, int size) {
    if (m_font) {
        TTF_CloseFont(m_font);
    }
    m_font = TTF_OpenFont(path.c_str(), size);
    if (!m_font) {
        std::cerr << "Failed to load font " << path << ": " << TTF_GetError() << std::endl;
        return false;
    }

    return initShader();
}

bool Font::initShader() {
    if (m_shaderProgram) return true; // Already initialized

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex Shader Error: " << infoLog << std::endl;
        return false;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment Shader Error: " << infoLog << std::endl;
        return false;
    }

    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vertexShader);
    glAttachShader(m_shaderProgram, fragmentShader);
    glLinkProgram(m_shaderProgram);

    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(m_shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader Program Link Error: " << infoLog << std::endl;
        return false;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    m_posAttrib = glGetAttribLocation(m_shaderProgram, "position");
    m_texAttrib = glGetAttribLocation(m_shaderProgram, "texcoord");
    m_samplerUniform = glGetUniformLocation(m_shaderProgram, "texSampler");
    m_projUniform = glGetUniformLocation(m_shaderProgram, "projection");

    glGenBuffers(1, &m_vbo);

    return true;
}

void Font::renderText(const std::string& text, SDL_Color color, GLuint& outTexture, int& outWidth, int& outHeight) {
    if (!m_font) return;

    SDL_Surface* surface = TTF_RenderText_Blended(m_font, text.c_str(), color);
    if (!surface) {
        std::cerr << "TTF_RenderText_Blended failed: " << TTF_GetError() << std::endl;
        return;
    }

    outWidth = surface->w;
    outHeight = surface->h;

    glGenTextures(1, &outTexture);
    glBindTexture(GL_TEXTURE_2D, outTexture);

    int format = GL_RGB;
    if (surface->format->BytesPerPixel == 4) {
        if (surface->format->Rmask == 0x000000ff) {
            format = GL_RGBA;
        } else {
            // Some systems, especially little-endian ARGB surfaces from SDL, need BGRA format for OpenGL
            // For OpenGL ES 2.0 without GL_EXT_texture_format_BGRA8888, we might have to manually swap channels
            // For now, assume RGBA works or we can enforce it.
            // A simple fix for SDL_ttf's Blended format is often just RGBA, but if it swaps colors,
            // you'd typically use GL_BGRA_EXT if supported. We'll use GL_RGBA and let SDL do its thing for now,
            // but we can convert surface if needed.
            format = GL_RGBA;
        }
    }

    // To be perfectly safe across systems with GLES2, we can convert the surface to a known format:
    SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ABGR8888, 0);
    if (formattedSurface) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, formattedSurface->w, formattedSurface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, formattedSurface->pixels);
        SDL_FreeSurface(formattedSurface);
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    SDL_FreeSurface(surface);
}

void Font::drawText(GLuint texture, int x, int y, int width, int height, float scaleX, float scaleY) {
    if (!m_shaderProgram) return;

    glUseProgram(m_shaderProgram);

    // Enable blending for text
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Orthographic projection for 2D rendering. Assuming 720x720 base logical resolution
    // scaled by the actual display window size (handled by scaleX, scaleY if applied to coordinates)
    // We will construct a simple ortho matrix for 720x720
    float left = 0.0f;
    float right = 720.0f;
    float bottom = 720.0f;
    float top = 0.0f;
    float zNear = -1.0f;
    float zFar = 1.0f;

    float ortho[16] = {
        2.0f / (right - left), 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
        0.0f, 0.0f, -2.0f / (zFar - zNear), 0.0f,
        -(right + left) / (right - left), -(top + bottom) / (top - bottom), -(zFar + zNear) / (zFar - zNear), 1.0f
    };

    glUniformMatrix4fv(m_projUniform, 1, GL_FALSE, ortho);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(m_samplerUniform, 0);

    // Quad vertices (x, y, u, v)
    float x1 = x;
    float y1 = y;
    float x2 = x + width;
    float y2 = y + height;

    float vertices[] = {
        x1, y1, 0.0f, 0.0f,
        x1, y2, 0.0f, 1.0f,
        x2, y1, 1.0f, 0.0f,
        x2, y2, 1.0f, 1.0f
    };

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(m_posAttrib);
    glVertexAttribPointer(m_posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(m_texAttrib);
    glVertexAttribPointer(m_texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableVertexAttribArray(m_posAttrib);
    glDisableVertexAttribArray(m_texAttrib);
    glDisable(GL_BLEND);
}
