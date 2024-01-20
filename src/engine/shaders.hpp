#pragma once
// reviewed: 2023-12-24
// reviewed: 2024-01-04
// reviewed: 2024-01-06
// reviewed: 2024-01-10

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
  static inline char const *vertex_shader_source = R"(
  #version 330 core
  uniform mat4 umtx_mw;  // model-to-world-matrix
  uniform mat4 umtx_wvp; // world-to-view-to-projection
  layout(location = 0) in vec4 apos;
  layout(location = 1) in vec4 argba;
  layout(location = 2) in vec3 anorm;
  layout(location = 3) in vec2 atex;
  out vec4 vrgba;
  out vec3 vnorm;
  out vec2 vtex;
  void main() {
	  gl_Position = umtx_wvp * umtx_mw * apos;
	  vrgba = argba;
	  vnorm = normalize(transpose(inverse(mat3(umtx_mw))) * anorm);
	  vtex = atex;
  }
  )";

  static inline char const *fragment_shader_source = R"(
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
  static constexpr GLint apos = 0;
  static constexpr GLint argba = 1;
  static constexpr GLint anorm = 2;
  static constexpr GLint atex = 3;
  // uniform matrixes
  static constexpr GLint umtx_mw = 0;  // model->world matrix
  static constexpr GLint umtx_wvp = 1; // world->view->projection matrix
  // uniform textures
  static constexpr GLint utex = 2; // texture mapper
  // uniform ambient light
  static constexpr GLint ulht = 3; // light vector

  inline void init() {
    puts("");
    gl_print_string("GL_VENDOR", GL_VENDOR);
    gl_print_string("GL_RENDERER", GL_RENDERER);
    gl_print_string("GL_VERSION", GL_VERSION);
    gl_print_string("GL_SHADING_LANGUAGE_VERSION", GL_SHADING_LANGUAGE_VERSION);
    puts("");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    uint32_t const default_program_ix =
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
  }

  inline void free() {
    for (program const &p : programs) {
      glDeleteProgram(p.id);
    }
    programs.clear();
  }

  auto load_program_from_source(char const *vert_src, char const *frag_src)
      -> uint32_t {
    GLuint const program_id = glCreateProgram();
    GLuint const vertex_shader_id = compile(GL_VERTEX_SHADER, vert_src);
    GLuint const fragment_shader_id = compile(GL_FRAGMENT_SHADER, frag_src);
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    glLinkProgram(program_id);
    // glValidateProgram(program_id);
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

    programs.push_back({program_id});

    return uint32_t(programs.size() - 1);
  }

  inline auto programs_count() const -> size_t { return programs.size(); }

  inline void use_program(size_t const ix) const { programs.at(ix).use(); }

private:
  static inline auto compile(GLenum const shader_type, char const *src)
      -> GLuint {
    GLuint const shader_id = glCreateShader(shader_type);
    glShaderSource(shader_id, 1, &src, nullptr);
    glCompileShader(shader_id);
    GLint ok = 0;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &ok);
    if (not ok) {
      GLchar msg[1024];
      glGetShaderInfoLog(shader_id, sizeof(msg), nullptr, msg);
      fprintf(stderr, "\n%s:%d: compile error in %s shader:\n%s\n", __FILE__,
              __LINE__, shader_name_for_type(shader_type), msg);
      fflush(stderr);
      std::abort();
    }
    return shader_id;
  }

public:
  //
  // static functions
  //
  static inline void gl_print_string(char const *name, GLenum const gl_str) {
    char const *str = (char const *)glGetString(gl_str);
    printf("%s = %s\n", name, str);
  }

  static inline auto shader_name_for_type(GLenum const shader_type)
      -> char const * {
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