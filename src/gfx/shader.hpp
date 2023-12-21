#pragma once
#include "../lib.h"
#include "../metrics.hpp"
#include <vector>

class vertex {
public:
  float position[3];
  float color[4];
  float normal[3];
  float texture[2];
};

static const char *shader_vertex_source = R"(
  #version 130
  uniform mat4 umtx_mw; // model-to-world-matrix
  uniform mat4 umtx_wvp;// world-to-view-to-projection
  in vec3 apos;
  in vec4 argba;
  in vec3 anorm;
  in vec2 atex;
  out vec4 vrgba;
  out vec3 vnorm;
  out vec2 vtex;
  void main(){
	  gl_Position=umtx_wvp*umtx_mw*vec4(apos,1);
	  vrgba=argba;
	  vnorm=anorm;
	  vtex=atex;
  }
)";

static const char *shader_fragment_source = R"(
  #version 130
  uniform sampler2D utex;
  in vec4 vrgba;
  in vec3 vnorm;
  in vec2 vtex;
  out vec4 rgba;
  void main(){
    float ambient_light=dot(vnorm,normalize(vec3(0,15,-15)));
    rgba=texture2D(utex,vtex)+vrgba*ambient_light;
  }
)";

#define shader_apos 0
#define shader_argba 1
#define shader_anorm 2
#define shader_atex 3
#define shader_umtx_mw 0
#define shader_umtx_wvp 1
#define shader_utex 2

class program {
public:
  GLuint id = 0;
  unsigned attached_vtxshdr_id = 0;
  unsigned attached_frgshdr_id = 0;
  std::vector<int> attributes{};
};

static std::vector<program> programs{};

inline static const program &
shader_load_program_from_source(const char *vert_src, const char *frag_src,
                                std::vector<int> attrs);
inline static void gl_check_error(const char *op);
inline static void gl_print_string(const char *name, const GLenum s);
inline static const char *shader_name_for_type(GLenum shader_type);

inline static void shader_init() {
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

  std::vector<int> attrs{shader_apos, shader_argba, shader_anorm, shader_atex};
  shader_load_program_from_source(shader_vertex_source, shader_fragment_source,
                                  attrs);

  gl_check_error("after shader_init");
}

inline static void shader_free() {
  for (const program &p : programs) {
    glDeleteShader(p.attached_vtxshdr_id);
    glDeleteShader(p.attached_frgshdr_id);
    glDeleteProgram(p.id);
  }
}

inline static GLuint shader_compile(GLenum shaderType, const char *code) {
  GLuint id = glCreateShader(shaderType);
  size_t length = strlen(code);
  glShaderSource(id, 1, (const GLchar **)&code, (GLint *)&length);
  glCompileShader(id);
  GLint ok;
  glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
  if (!ok) {
    GLchar messages[1024];
    glGetShaderInfoLog(id, sizeof(messages), NULL, &messages[0]);
    printf("compiler error in %s shader:\n%s\n",
           shader_name_for_type(shaderType), messages);
    exit(7);
  }
  return id;
}

inline static const program &
shader_load_program_from_source(const char *vert_src, const char *frag_src,
                                std::vector<int> attrs) {
  gl_check_error("enter shader_program_load");

  unsigned pid = glCreateProgram();
  programs.push_back({pid, shader_compile(GL_VERTEX_SHADER, vert_src),
                      shader_compile(GL_FRAGMENT_SHADER, frag_src),
                      std::move(attrs)});

  glAttachShader(pid, programs.back().attached_vtxshdr_id);
  glAttachShader(pid, programs.back().attached_frgshdr_id);
  glLinkProgram(pid);

  GLint ok;
  glGetProgramiv(pid, GL_LINK_STATUS, &ok);
  if (!ok) {
    GLint len;
    glGetProgramiv(pid, GL_INFO_LOG_LENGTH, &len);

    GLchar msg[1024];
    if (len > (signed)sizeof msg) {
      len = sizeof msg;
    }
    glGetProgramInfoLog(pid, len, NULL, &msg[0]);
    printf("program linking error: %s\n", msg);
    exit(8);
  }
  gl_check_error("exit shader_program_load");
  return programs.back();
}

inline static const char *gl_get_error_string(const GLenum error) {
  const char *str;
  switch (error) {
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

inline static void gl_print_string(const char *name, const GLenum s) {
  const char *v = (const char *)glGetString(s);
  printf("%s=%s\n", name, v);
}

inline static void gl_check_error(const char *op) {
  int err = 0;
  for (GLenum error = glGetError(); error; error = glGetError()) {
    printf("!!! %s   glerror %x   %s\n", op, error, gl_get_error_string(error));
    err = 1;
  }
  if (err)
    exit(11);
}

inline static const char *shader_name_for_type(GLenum shader_type) {
  switch (shader_type) {
  case GL_VERTEX_SHADER:
    return "vertex";
  case GL_FRAGMENT_SHADER:
    return "fragment";
  default:
    return "unknown";
  }
}
