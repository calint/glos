#pragma once
// reviewed: 2023-12-22

#include "../metrics.hpp"
#include <vector>

static const char *vertex_shader_source = R"(
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
  void main(){
	  gl_Position = umtx_wvp * umtx_mw * vec4(apos,1);
	  vrgba = argba;
	  vnorm = normalize(transpose(inverse(mat3(umtx_mw))) * anorm);
//	  vnorm = anorm;
	  vtex = atex;
  }
)";

static const char *fragment_shader_source = R"(
  #version 330 core
  uniform sampler2D utex;
  uniform vec3 ulht;
  in vec4 vrgba;
  in vec3 vnorm;
  in vec2 vtex;
  out vec4 rgba;
  void main(){
    float diff = max(dot(vnorm, ulht), 0);
    rgba = texture2D(utex, vtex) + diff * vrgba;
  }
)";

class vertex final {
public:
  float position[3];
  float color[4];
  float normal[3];
  float texture[2];
};

class program final {
public:
  GLuint id = 0;
  GLuint vertex_shader_id = 0;
  GLuint fragment_shader_id = 0;
  std::vector<GLuint> enabled_attributes{};
};

class shaders final {
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

  std::vector<program> programs{};

  inline void init() {
    gl_check_error("shader_init");

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

    load_program_from_source(
        vertex_shader_source, fragment_shader_source,
        {shaders::apos, shaders::argba, shaders::anorm, shaders::atex});

    gl_check_error("after shader_init");

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
  }

  inline void free() {
    for (const program &p : programs) {
      glDeleteShader(p.vertex_shader_id);
      glDeleteShader(p.fragment_shader_id);
      glDeleteProgram(p.id);
    }
  }

  auto load_program_from_source(const char *vert_src, const char *frag_src,
                                std::vector<GLuint> attrs) -> int {
    gl_check_error("enter shader_program_load");

    const GLuint id = glCreateProgram();
    programs.push_back({id, compile(GL_VERTEX_SHADER, vert_src),
                        compile(GL_FRAGMENT_SHADER, frag_src),
                        std::move(attrs)});

    glAttachShader(id, programs.back().vertex_shader_id);
    glAttachShader(id, programs.back().fragment_shader_id);
    glLinkProgram(id);

    GLint ok = 0;
    glGetProgramiv(id, GL_LINK_STATUS, &ok);
    if (!ok) {
      GLint len = 0;
      glGetProgramiv(id, GL_INFO_LOG_LENGTH, &len);

      GLchar msg[1024];
      if (len > (signed)sizeof(msg)) {
        len = sizeof(msg);
      }
      glGetProgramInfoLog(id, len, NULL, msg);
      printf("program linking error: %s\n", msg);
      exit(8);
    }
    gl_check_error("exit shader_program_load");
    return int(programs.size() - 1);
  }

private:
  inline static auto compile(GLenum shader_type, const char *src) -> GLuint {
    const GLuint id = glCreateShader(shader_type);
    const size_t length = strlen(src);
    glShaderSource(id, 1, (const GLchar **)&src, (GLint *)&length);
    glCompileShader(id);
    GLint ok = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
    if (!ok) {
      GLchar msg[1024];
      glGetShaderInfoLog(id, sizeof(msg), NULL, msg);
      printf("compiler error in %s shader:\n%s\n",
             shader_name_for_type(shader_type), msg);
      exit(7);
    }
    return id;
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
      //		str = "GL_STACK_OVERFLOW";-Wunused-variable
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

  inline static void gl_check_error(const char *op) {
    int err = 0;
    for (GLenum error = glGetError(); error; error = glGetError()) {
      printf("!!! %s   glerror %x   %s\n", op, error,
             gl_get_error_string(error));
      err = 1;
    }
    if (err) {
      exit(11);
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