#pragma once

#include "GL.hpp"
#include "glm/fwd.hpp"
#include <hb.h>
#include <hb-ft.h>
#include <string>

namespace TextRenderer {

	GLuint atlas    = -1U;
	GLuint vao      = -1U;
	GLuint vbo      = -1U;

	hb_font_t *font = nullptr;
	size_t font_size = -1U;


	FT_Library ft_library;
	FT_Face ft_face;
	
	void load_font(
		const glm::uvec2  & drawable_size,
		const std::string & font_path
	);
}
