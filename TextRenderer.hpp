#pragma once

#include "GL.hpp"
#include <glm/glm.hpp>
#include <hb.h>
#include <hb-ft.h>
#include <string>
#include <vector>

namespace TextRenderer {

	struct Vertex {
		glm::vec2 pos;
		glm::vec2 uv;
	};

	void load_font(
		size_t size,
		const std::string & font_path
	);

	size_t get_string(
		const char          *string,
		std::vector<Vertex> &vertices,
		float                max_len
	);

	void render(
		const glm::uvec2 &drawable_size,
		const std::vector<Vertex> &vertices,

		// properties
		const glm::vec2  &position,
		float scale,
		const glm::u8vec4 &color,
		bool  center
	);

} // namespace TextRenderer

