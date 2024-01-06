#pragma once
// reviewed: 2023-12-24
// reviewed: 2024-01-04

#include <GLES3/gl3.h>
#include <vector>

namespace glos {

struct vertex {
  float position[3];
  float color[4];
  float normal[3];
  float texture[2];
};

class shaders final {
  class program final {
  public:
    GLuint id = 0;

    inline void use() const { glUseProgram(id); }
  };

  // default shader source
  inline static char const *vertex_shader_source = R"(
  #version 330 core
  uniform mat4 umtx_mw;  // model-to-world-matrix
  uniform mat4 umtx_wvp; // world-to-view-to-projection
  layout(location = 0) in vec3 apos;
  layout(location = 1) in vec4 argba;
  layout(location = 2) in vec3 anorm;
  layout(location = 3) in vec2 atex;
  out vec4 vrgba;
  out vec3 vnorm;
  out vec2 vtex;
  void main() {
	  gl_Position = umtx_wvp * umtx_mw * vec4(apos,1);
	  vrgba = argba;
	  vnorm = normalize(transpose(inverse(mat3(umtx_mw))) * anorm);
	  vtex = atex;
  }
  )";

  inline static char const *fragment_shader_source = R"(
  #version 330 core
  uniform sampler2D utex;
  uniform vec3 ulht;
  in vec4 vrgba;
  in vec3 vnorm;
  in vec2 vtex;
  out vec4 rgba;
  void main() {
    float diff = max(dot(vnorm, ulht), 0.5);
    rgba = vec4(vec3(texture2D(utex, vtex)) + diff * vec3(vrgba), vrgba.a);
  }
)";

  std::vector<program> programs{};

public:
  // vertex attributes layout in shaders
  static constexpr GLuint apos = 0;
  static constexpr GLuint argba = 1;
  static constexpr GLuint anorm = 2;
  static constexpr GLuint atex = 3;
  // uniform matrixes
  static constexpr GLint umtx_mw = 0;  // model->world matrix
  static constexpr GLint umtx_wvp = 1; // world->view->projection matrix
  // uniform textures
  static constexpr GLint utex = 2; // texture mapper
  // uniform ambient light
  static constexpr GLint ulht = 3; // light vector

  inline void init() {
    gl_check_error("init");

    puts("");
    gl_print_string("GL_VENDOR", GL_VENDOR);
    gl_print_string("GL_RENDERER", GL_RENDERER);
    gl_print_string("GL_VERSION", GL_VERSION);
    gl_print_string("GL_SHADING_LANGUAGE_VERSION", GL_SHADING_LANGUAGE_VERSION);
    puts("");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    size_t const default_program_ix =
        load_program_from_source(vertex_shader_source, fragment_shader_source);

    GLuint const def_prog_id = programs.at(default_program_ix).id;
    programs.at(default_program_ix).use();

    printf("shader uniforms locations:\n");
    printf(":-%10s-:-%7s-:\n", "----------", "-------");
    printf(": %10s : %-7d :\n", "umtx_mw",
           glGetUniformLocation(def_prog_id, "umtx_mw"));
    printf(": %10s : %-7d :\n", "umtx_wvp",
           glGetUniformLocation(def_prog_id, "umtx_wvp"));
    printf(": %10s : %-7d :\n", "utex",
           glGetUniformLocation(def_prog_id, "utex"));
    printf(": %10s : %-7d :\n", "ulht",
           glGetUniformLocation(def_prog_id, "ulht"));
    printf(":-%10s-:-%7s-:\n", "----------", "-------");

    puts("");
    printf("shader attributes locations:\n");
    printf(":-%10s-:-%7s-:\n", "----------", "-------");
    printf(": %10s : %-7d :\n", "apos",
           glGetAttribLocation(def_prog_id, "apos"));
    printf(": %10s : %-7d :\n", "argba",
           glGetAttribLocation(def_prog_id, "argba"));
    printf(": %10s : %-7d :\n", "anorm",
           glGetAttribLocation(def_prog_id, "anorm"));
    printf(": %10s : %-7d :\n", "atex",
           glGetAttribLocation(def_prog_id, "atex"));
    printf(":-%10s-:-%7s-:\n", "----------", "-------");
    puts("");

    gl_check_error("after init");
  }

  inline void free() {
    for (program const &p : programs) {
      glDeleteProgram(p.id);
    }
    programs.clear();
  }

  auto load_program_from_source(char const *vert_src, char const *frag_src)
      -> size_t {
    gl_check_error("enter load_program_from_source");
    GLuint const program_id = glCreateProgram();
    GLuint const vertex_shader_id = compile(GL_VERTEX_SHADER, vert_src);
    GLuint const fragment_shader_id = compile(GL_FRAGMENT_SHADER, frag_src);
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    glLinkProgram(program_id);
    GLint ok = 0;
    glGetProgramiv(program_id, GL_LINK_STATUS, &ok);
    if (!ok) {
      GLchar msg[1024];
      glGetProgramInfoLog(program_id, sizeof(msg), nullptr, msg);
      fprintf(stderr, "\n%s:%d: program linking error:\n%s\n", __FILE__,
              __LINE__, msg);
      fflush(stderr);
      std::abort();
    }

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    gl_check_error("exit load_program_from_source");

    programs.push_back({program_id});

    return programs.size() - 1;
  }

  inline auto programs_count() const -> size_t { return programs.size(); }

  inline void use_program(size_t const ix) const { programs.at(ix).use(); }

  inline auto program_id(size_t const ix) const -> GLuint {
    return programs.at(ix).id;
  }

private:
  inline static auto compile(GLenum shader_type, char const *src) -> GLuint {
    GLuint const shader_id = glCreateShader(shader_type);
    glShaderSource(shader_id, 1, &src, nullptr);
    glCompileShader(shader_id);
    GLint ok = 0;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &ok);
    if (!ok) {
      GLchar msg[1024];
      glGetShaderInfoLog(shader_id, sizeof(msg), NULL, msg);
      fprintf(stderr, "\n%s:%d: compile error in %s shader:\n%s\n", __FILE__,
              __LINE__, shader_name_for_type(shader_type), msg);
      fflush(stderr);
      std::abort();
    }
    return shader_id;
  }

  inline static auto gl_get_error_string(GLenum const gl_error) -> const
      char * {
    const char *str = nullptr;
    switch (gl_error) {
    case GL_NO_ERROR:
      str = "GL_NO_ERROR";
      break;
    case GL_INVALID_ENUM:
      str = "GL_INVALID_ENUM";
      break;
    case GL_INVALID_VALUE:
      str = "GL_INVALID_VALUE";
      break;
    case GL_INVALID_OPERATION:
      str = "GL_INVALID_OPERATION";
      break;
#if defined __gl_h_ || defined __gl3_h_
    case GL_OUT_OF_MEMORY:
      str = "GL_OUT_OF_MEMORY";
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      str = "GL_INVALID_FRAMEBUFFER_OPERATION";
      break;
#endif
#if defined __gl_h_
    case GL_STACK_OVERFLOW:
      str = "GL_STACK_OVERFLOW";
      break;
    case GL_STACK_UNDERFLOW:
      str = "GL_STACK_UNDERFLOW";
      break;
    case GL_TABLE_TOO_LARGE:
      str = "GL_TABLE_TOO_LARGE";
      break;
#endif
    default:
      str = "(ERROR: Unknown Error Enum)";
      break;
    }
    return str;
  }

public:
  //
  // static functions
  //
  inline static void gl_print_string(char const *name, const GLenum gl_str) {
    const char *str = (const char *)glGetString(gl_str);
    printf("%s=%s\n", name, str);
  }

  inline static void gl_check_error(char const *user_msg) {
    bool is_error = false;
    for (GLenum error = glGetError(); error; error = glGetError()) {
      fprintf(stderr, "\n%s:%d: opengl error %x in '%s'\n%s\n", __FILE__,
              __LINE__, error, user_msg, gl_get_error_string(error));
      is_error = true;
    }
    if (is_error) {
      fflush(stderr);
      std::abort();
    }
  }

  inline static auto shader_name_for_type(GLenum const shader_type) -> const
      char * {
    switch (shader_type) {
    case GL_VERTEX_SHADER:
      return "vertex";
    case GL_FRAGMENT_SHADER:
      return "fragment";
    default:
      return "unknown";
    }
  }
};

inline shaders shaders{};
} // namespace glos