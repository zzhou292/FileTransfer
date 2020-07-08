// header file generated by txt2h, 2003-2006 by ScottManDeath
#ifndef  TXT_HEADER_bar_vert
#define  TXT_HEADER_bar_vert


const char bar_vert [] =
"#version 330\n"
"\n"
"layout(location = 0) in vec3 vertex_position;\n"
"layout(location = 2) in vec3 vertex_color;\n"
"layout(location = 1) in vec3 vertex_normal;\n"
"\n"
"\n"
"uniform mat4 projection_matrix;\n"
"uniform mat4 view_matrix;\n"
"\n"
"flat out vec3 color;\n"
"\n"
"void main() {\n"
"  mat4 modelview = view_matrix;\n"
"  mat4 mvp = projection_matrix * modelview;\n"
"  gl_Position = mvp * vec4(vertex_position, 1.0);\n"
"  color = vertex_color;\n"
"}\n"
;


#endif  // #ifdef TXT_HEADER_bar_vert
