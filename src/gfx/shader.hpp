#pragma once
// reviewed: 2023-12-24

#include "../metrics.hpp"
#include <vector>

class vertex final {
public:
  float position[3];
  float color[4];
  float normal[3];
  float texture[2];
};

class shaders final {
  class program final {
  public:
    GLuint id = 0;

    inline void activate() { glUseProgram(id); }
  };

  inline static const char *vertex_shader_source = R"(
  #version 330 core
  uniform mat4 umtx_mw;  // model-to-world-matrix
  uniform mat4 umtx_wvp; // world-to-view-to-projection
  in vec3 apos;
  in vec4 argba;
  in vec3 anorm;
  in vec2 atex;
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

  inline static const char *fragment_shader_source = R"(
  #version 330 core
  uniform sampler2D utex;
  uniform vec3 ulht;
  in vec4 vrgba;
  in vec3 vnorm;
  in vec2 vtex;
  out vec4 rgba;
  void main() {
    float diff = max(dot(vnorm, ulht), 0);
    rgba = texture2D(utex, vtex) + diff * vrgba;
  }
)";

  std::vector<program> programs{};

public:
  // enabled_attributes layout in shaders
  static constexpr unsigned apos = 0;
  static constexpr unsigned argba = 1;
  static constexpr unsigned anorm = 2;
  static constexpr unsigned atex = 3;
  // uniform matrixes
  static constexpr unsigned umtx_mw = 0;  // model->world matrix
  static constexpr unsigned umtx_wvp = 1; // world->view->projection matrix
  // uniform textures
  static constexpr unsigned utex = 2; // texture mapper
  static constexpr unsigned ulht = 3; // light vector

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

    printf(":-%10s-:-%7s-:\n", "----------", "-------");
    printf(": %10s : %-7s :\n", "feature", "y/n");
    printf(":-%10s-:-%7s-:\n", "----------", "-------");
    printf(": %10s : %-7s :\n", "cull face",
           glIsEnabled(GL_CULL_FACE) ? "yes" : "no");
    printf(": %10s : %-7s :\n", "zbuffer",
           glIsEnabled(GL_DEPTH_TEST) ? "yes" : "no");
    printf(": %10s : %-7s :\n", "blend", glIsEnabled(GL_BLEND) ? "yes" : "no");
    printf(":-%10s-:-%7s-:\n", "----------", "-------");
    puts("");

    const int default_program_id =
        load_program_from_source(vertex_shader_source, fragment_shader_source);

    programs.at(size_t(default_program_id)).activate();

    printf("shader uniform locations:\n");
    printf(":-%10s-:-%7s-:\n", "----------", "-------");
    printf(": %10s : %-7d :\n", "umtx_mw",
           glGetUniformLocation(programs.at(0).id, "umtx_mw"));
    printf(": %10s : %-7d :\n", "umtx_wvp",
           glGetUniformLocation(programs.at(0).id, "umtx_wvp"));
    printf(": %10s : %-7d :\n", "utex",
           glGetUniformLocation(programs.at(0).id, "utex"));
    printf(": %10s : %-7d :\n", "ulht",
           glGetUniformLocation(programs.at(0).id, "ulht"));
    printf(":-%10s-:-%7s-:\n", "----------", "-------");
    puts("");
    printf("shader attribute locations:\n");
    printf(":-%10s-:-%7s-:\n", "----------", "-------");
    printf(": %10s : %-7d :\n", "apos",
           glGetAttribLocation(programs.at(0).id, "apos"));
    printf(": %10s : %-7d :\n", "argba",
           glGetAttribLocation(programs.at(0).id, "argba"));
    printf(": %10s : %-7d :\n", "anorm",
           glGetAttribLocation(programs.at(0).id, "anorm"));
    printf(": %10s : %-7d :\n", "atex",
           glGetAttribLocation(programs.at(0).id, "atex"));
    printf(":-%10s-:-%7s-:\n", "----------", "-------");
    puts("");

    gl_check_error("after init");
  }

  inline void free() {
    for (const program &p : programs) {
      glDeleteProgram(p.id);
    }
  }

  auto load_program_from_source(const char *vert_src, const char *frag_src)
      -> int {
    gl_check_error("enter load_program_from_source");
    const GLuint program_id = glCreateProgram();
    const GLuint vertex_shader_id = compile(GL_VERTEX_SHADER, vert_src);
    const GLuint fragment_shader_id = compile(GL_FRAGMENT_SHADER, frag_src);
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    glLinkProgram(program_id);
    GLint ok = 0;
    glGetProgramiv(program_id, GL_LINK_STATUS, &ok);
    if (!ok) {
      GLchar msg[1024];
      glGetProgramInfoLog(program_id, sizeof(msg), nullptr, msg);
      printf("program linking error: %s\n", msg);
      std::abort();
    }

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    gl_check_error("exit load_program_from_source");
    programs.push_back({program_id});

    return int(programs.size() - 1);
  }

  inline auto programs_count() const -> int { return int(programs.size()); }

  inline void activate_program(const int ix) {
    programs.at(size_t(ix)).activate();
  }

private:
  inline static auto compile(GLenum shader_type, const char *src) -> GLuint {
    const GLuint shader_id = glCreateShader(shader_type);
    glShaderSource(shader_id, 1, &src, nullptr);
    glCompileShader(shader_id);
    GLint ok = 0;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &ok);
    if (!ok) {
      GLchar msg[1024];
      glGetShaderInfoLog(shader_id, sizeof(msg), NULL, msg);
      printf("compile error in %s shader:\n%s\n",
             shader_name_for_type(shader_type), msg);
      std::abort();
    }
    return shader_id;
  }

  inline static auto gl_get_error_string(const GLenum gl_error) -> const
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

  inline static void gl_print_string(const char *name, const GLenum gl_str) {
    const char *str = (const char *)glGetString(gl_str);
    printf("%s=%s\n", name, str);
  }

  inline static void gl_check_error(const char *user_msg) {
    bool is_error = false;
    for (GLenum error = glGetError(); error; error = glGetError()) {
      printf("!!! %s   glerror %x   %s\n", user_msg, error,
             gl_get_error_string(error));
      is_error = true;
    }
    if (is_error) {
      std::abort();
    }
  }

  inline static auto shader_name_for_type(const GLenum shader_type) -> const
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

static shaders shaders{};