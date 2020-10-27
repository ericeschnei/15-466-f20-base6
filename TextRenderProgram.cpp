#include "TextRenderProgram.hpp"

#include "gl_compile_program.hpp"
#include "gl_errors.hpp"

Load< TextRenderProgram > text_render_program(LoadTagEarly);

TextRenderProgram::TextRenderProgram() {
	//Compile vertex and fragment shaders using the convenient 'gl_compile_program' helper function:
	program = gl_compile_program(
		//vertex shader:
		"#version 330\n"
		"uniform mat4 OBJECT_TO_CLIP;\n"
		"uniform vec2 OFFSET;\n"
		"in vec4 Position;\n"
		"in vec2 TexCoord;\n"
		"out vec2 texCoord;\n"
		"void main() {\n"
		"	 gl_Position = OBJECT_TO_CLIP * (vec4(OFFSET, 0.0, 0.0) + Position);\n"
		"	 texCoord = TexCoord;\n"
		"}\n"
	,
		//fragment shader:
		"#version 330\n"
		"uniform sampler2D TEX;\n"
		"uniform vec4 COLOR;\n"
		"in vec2 texCoord;\n"
		"out vec4 fragColor;\n"
		"void main() {\n"
		"	fragColor = COLOR.rgba * vec4(1.0, 1.0, 1.0, texelFetch(TEX, ivec2(texCoord.xy), 0).r);\n"
		"}\n"
	);
	//look up the locations of vertex attributes:
	Position_vec2 = glGetAttribLocation(program, "Position");
	TexCoord_vec2 = glGetAttribLocation(program, "TexCoord");

	//look up the locations of uniforms:
	OBJECT_TO_CLIP_mat4 = glGetUniformLocation(program, "OBJECT_TO_CLIP");
	COLOR_vec4 = glGetUniformLocation(program, "COLOR");
	OFFSET_vec2 = glGetUniformLocation(program, "OFFSET");
	GLuint TEX_sampler2D = glGetUniformLocation(program, "TEX");


	//set TEX to always refer to texture binding zero:
	glUseProgram(program); //bind program -- glUniform* calls refer to this program now

	glUniform1i(TEX_sampler2D, 0); //set TEX to sample from GL_TEXTURE0

	glUseProgram(0); //unbind program -- glUniform* calls refer to ??? now
}

TextRenderProgram::~TextRenderProgram() {
	glDeleteProgram(program);
	program = 0;
}

