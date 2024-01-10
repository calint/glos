#pragma once
// reviewed: 2024-01-06
// reviewed: 2024-01-10

//
//! colors do not get converted correctly. color 'red' works though.
//

namespace glos {

class hud final {
public:
  size_t program_ix = 0;

  inline void init() {
    if (TTF_Init()) {
      fprintf(stderr, "\n%s:%d: cannot initiate ttf: %s\n", __FILE__, __LINE__,
              TTF_GetError());
      fflush(stderr);
      std::abort();
    }

    constexpr GLfloat quad_vertices[] = {
        // positions  // texture coords
        -1.0f, 1.0f,  0.0f, 0.0f, // top left
        1.0f,  1.0f,  1.0f, 0.0f, // top right
        1.0f,  -1.0f, 1.0f, 1.0f, // bottom right
        -1.0f, -1.0f, 0.0f, 1.0f  // bottom left
    };

    constexpr GLuint quad_indices[] = {
        2, 1, 0, // first triangle
        3, 2, 0  // second triangle
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices,
                 GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices,
                 GL_STATIC_DRAW);

    // Set the vertex attribute pointers
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (GLvoid *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (GLvoid *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

    program_ix = shaders.load_program_from_source(vertex_shader_source,
                                                  fragment_shader_source);
  }

  inline void free() {
    if (font) {
      TTF_CloseFont(font);
    }
    TTF_Quit();
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
    glDeleteTextures(1, &texture);
  }

  inline void load_font(char const *ttf_path, int const size) {
    font = TTF_OpenFont(ttf_path, size);
    if (not font) {
      fprintf(stderr, "\n%s:%d: cannot load font '%s': %s\n", __FILE__,
              __LINE__, ttf_path, TTF_GetError());
      fflush(stderr);
      std::abort();
    }
  }

  inline void render() const {
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(0, 0); // sets uniform "utex" to texture unit 0
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    // glDisable(GL_BLEND);
  }

  inline void print(char const *text, SDL_Color color, int x, int y) const {
    SDL_Surface *text_surface = TTF_RenderUTF8_Blended(font, text, color);
    // ARGB8888
    // printf("Surface Format: %s\n",
    //        SDL_GetPixelFormatName(text_surface->format->format));

    if (not text_surface) {
      fprintf(stderr, "\n%s:%d: cannot render text: %s\n", __FILE__, __LINE__,
              SDL_GetError());
      fflush(stderr);
      std::abort();
    }

    SDL_Surface *converted_surface =
        SDL_ConvertSurfaceFormat(text_surface, SDL_PIXELFORMAT_RGBA8888, 0);
    if (not converted_surface) {
      fprintf(stderr, "\n%s:%d: cannot convert surface: %s\n", __FILE__,
              __LINE__, SDL_GetError());
      fflush(stderr);
      std::abort();
    }

    glBindTexture(GL_TEXTURE_2D, texture);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, convertedSurface->w,
    //              convertedSurface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
    //              convertedSurface->pixels);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 0,
    //              GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, converted_surface->w,
                    converted_surface->h, GL_RGBA, GL_UNSIGNED_BYTE,
                    converted_surface->pixels);

    SDL_FreeSurface(text_surface);
    SDL_FreeSurface(converted_surface);
  }

private:
  GLuint vbo = 0;
  GLuint vao = 0;
  GLuint ebo = 0;
  GLuint texture = 0;
  TTF_Font *font = nullptr;

  static constexpr GLsizei texture_width = 256;
  static constexpr GLsizei texture_height = 256;

  static inline char const *vertex_shader_source = R"(
    #version 330 core
    layout(location = 0) in vec2 position;
    layout(location = 1) in vec2 tex_coord;
    out vec2 v_tex_coord;
    void main() {
        gl_Position = vec4(position, 0, 1.0);
        v_tex_coord = tex_coord;
    }
)";

  static inline char const *fragment_shader_source = R"(
    #version 330 core
    in vec2 v_tex_coord;
    out vec4 frag_color;
    uniform sampler2D utex;
    void main() {
        frag_color = texture(utex, v_tex_coord);
    }
)";
};

inline hud hud{};
} // namespace glos