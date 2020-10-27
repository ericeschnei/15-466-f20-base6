#include "TextRenderer.hpp"
#include "TextRenderProgram.hpp"
#include "GL.hpp"
#include "freetype/fttypes.h"
#include "gl_errors.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "hb-ft.h"
#include <algorithm>
#include <cstdio>
#include <stdexcept>
#include <map>
#include <set>

namespace {
	GLuint     atlas     = -1U;
	GLuint     vao       = -1U;
	GLuint     vbo       = -1U;

	hb_font_t  *font     = nullptr;
	size_t     font_size = -1U;

	FT_Library ft_library;
	FT_Face    ft_face;

	// internal atlas storage
	struct Character {
		glm::vec2 size;
		glm::vec2 start;
		glm::vec2 bearing;
	};

	std::map<hb_codepoint_t, Character> tex_locations;
	std::vector<hb_codepoint_t> tex_codepoints;
}

void TextRenderer::load_font(size_t size, const std::string &font_path) {

	{ // Initialize fonts
		if (FT_Init_FreeType(&ft_library)) {
			throw std::runtime_error("FT Library failed to initialize.");
		}
		if (FT_New_Face(ft_library, font_path.c_str(), 0, &ft_face)) {
			throw std::runtime_error("FT Face failed to initialize.");
		}
		if (FT_Set_Char_Size(ft_face, size, (FT_F26Dot6)0, 0, 0)) {
			throw std::runtime_error("FT: Setting char size failed.");
		}

		font_size = size;
		font      = hb_ft_font_create(ft_face, NULL);
	}

	{ // Initialize OpenGL
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glVertexAttribPointer(
			text_render_program->Position_vec2,
			2,
			GL_FLOAT,
			GL_FALSE,
			sizeof(Vertex),
			(GLbyte *)0 + 0
		);
		glEnableVertexAttribArray(text_render_program->Position_vec2);

		glVertexAttribPointer(
			text_render_program->TexCoord_vec2,
			2,
			GL_FLOAT,
			GL_FALSE,
			sizeof(Vertex),
			(GLbyte *)0 + 4*2
		);
		glEnableVertexAttribArray(text_render_program->TexCoord_vec2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		GL_ERRORS();
	}
}

size_t TextRenderer::get_string(
		const char          *string,
		std::vector<Vertex> &vertices,
		float                max_len) {

	(void) max_len;

	hb_buffer_t *buf = hb_buffer_create();
	hb_buffer_add_utf8(buf, string, -1, 0, -1);
	hb_buffer_guess_segment_properties(buf);
	hb_shape(font, buf, NULL, 0);

	size_t buf_len = hb_buffer_get_length(buf);
	hb_glyph_info_t *info = hb_buffer_get_glyph_infos(buf, NULL);

	bool redraw_texture = false;
	for (size_t i = 0; i < buf_len; i++) {
		hb_codepoint_t cp = info[i].codepoint;
		if (tex_locations.find(cp) == tex_locations.end()) {
			redraw_texture = true;
			break;
		}
	}

	// we found new characters, so we have to redraw the texture.
	if (redraw_texture) {
		std::printf("Redrawing string buffer\n");
		std::vector<hb_codepoint_t> codepoints;

		glm::uvec2 tex_size = glm::uvec2(0, 0);

		{ // generate new texture size
			auto add_new_glyph = [&tex_size, &codepoints](hb_codepoint_t codepoint){

				if (std::find(codepoints.begin(), codepoints.end(), codepoint) != codepoints.end()) return;
				codepoints.push_back(codepoint);

				if (FT_Load_Glyph(ft_face, codepoint, FT_LOAD_RENDER)) {
					throw std::runtime_error("Codepoint failed to load.");
				}

				tex_size.x += ft_face->glyph->bitmap.width;
				tex_size.y = std::max(tex_size.y, ft_face->glyph->bitmap.rows);
			};

			for (auto tex_codepoint : tex_codepoints) {
				add_new_glyph(tex_codepoint);
			}
			for (size_t i = 0; i < buf_len; i++) {
				hb_codepoint_t cp = info[i].codepoint;
				add_new_glyph(cp);
			}

			tex_codepoints = codepoints;
		}

		{ // generate texture using tex_size
			if (atlas != -1U) {
				glDeleteTextures(1, &atlas);
			}

			glActiveTexture(GL_TEXTURE0);
			glGenTextures(1, &atlas);
			glBindTexture(GL_TEXTURE_2D, atlas);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				tex_size.x,
				tex_size.y,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				0
			);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			GL_ERRORS();

		}

		{ // populate the texture
			tex_locations.clear();
			size_t x = 0;
			for (hb_codepoint_t cp : codepoints) {

				if (FT_Load_Glyph(ft_face, cp, FT_LOAD_RENDER)) {
					throw std::runtime_error("Codepoint failed to load.");
				}

				glm::uvec2 size = glm::uvec2(
					ft_face->glyph->bitmap.width,
					ft_face->glyph->bitmap.rows
				);

				glm::uvec2 bearing = glm::uvec2(
					ft_face->glyph->bitmap_left,
					ft_face->glyph->bitmap_top
				);

				Character c = {
					size,
					glm::uvec2(x, 0),
					bearing
				};
				tex_locations.emplace(cp, c);

				glTexSubImage2D(
					GL_TEXTURE_2D,
					(GLint)0,
					x,
					(GLint)0,
					size.x,
					size.y,
					GL_RED,
					GL_UNSIGNED_BYTE,
					ft_face->glyph->bitmap.buffer
				);
				x += size.x;

			}

			glBindTexture(GL_TEXTURE_2D, 0);
			GL_ERRORS();

		}
	}

	// now that we know that we have all the elements we care about,
	// we can render the glyphs:
	{
		hb_glyph_position_t *pos = hb_buffer_get_glyph_positions(buf, NULL);

		glm::vec2 current_position = glm::vec2(0.0f, 0.0f);
		for (size_t i = 0; i < buf_len; i++) {
			hb_codepoint_t cp = info[i].codepoint;
			glm::vec2 advance = glm::vec2(
				(float)pos[i].x_advance / 64.0f,
				(float)pos[i].y_advance / 64.0f
			);
			glm::vec2 offset = glm::vec2(
				(float)pos[i].x_offset / 64.0f,
				(float)pos[i].y_offset / 64.0f
			);

			auto c_it = tex_locations.find(cp);
			if (c_it == tex_locations.end()) {
				throw std::runtime_error("get_string found character that doesn't exist");
			}
			Character c = c_it->second;

			glm::vec2 real_pos = current_position + offset;
			glm::vec2 tex_pos = c.start;

			auto add_vert = [&real_pos, &c, &tex_pos, &vertices](float x, float y){
				Vertex v = {
					real_pos + glm::vec2(x, y - c.size.y) + c.bearing,
					tex_pos  + glm::vec2(x, c.size.y - y)
				};

				vertices.push_back(v);
			};

			add_vert(0.0f,     c.size.y);
			add_vert(0.0f,     0.0f);
			add_vert(c.size.x, 0.0f);

			add_vert(0.0f,     c.size.y);
			add_vert(c.size.x, 0.0f);
			add_vert(c.size.x, c.size.y);

			current_position += advance;

		}

		hb_buffer_destroy(buf);
	}

	return -1U;
}

void TextRenderer::render(
		const glm::uvec2          &drawable_size,
		const std::vector<Vertex> &vertices,
		const glm::vec2           &position,
		float                      scale,
		const glm::u8vec4         &color,
		bool                       center) {

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(
		GL_ARRAY_BUFFER,
		vertices.size() * sizeof(Vertex),
		vertices.data(),
		GL_DYNAMIC_DRAW
	);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(text_render_program->program);

	float s = scale / font_size * drawable_size.y;

	glm::mat4 proj = glm::ortho(
			0.0f,
			(float)drawable_size.x,
			0.0f,
			(float)drawable_size.y
		);
	glm::mat4 scale_mat = glm::mat4(
			glm::vec4(   s, 0.0f, 0.0f, 0.0f),
			glm::vec4(0.0f,    s, 0.0f, 0.0f),
			glm::vec4(0.0f, 0.0f,    s, 0.0f),
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
		);

	glm::vec3 len = glm::vec3(proj * scale_mat * glm::vec4(vertices.back().pos, 0.0f, 1.0f));
	glm::vec2 len_2d = glm::vec2(len.x + 1.0f, len.y + 1.0f) / 4.0f;
	if (!center) {
		len_2d = glm::vec2(0.0f, 0.0f);
	}
	len_2d.y = 0.0f;
	glm::vec2 trans_amt = (position - len_2d) * glm::vec2(drawable_size);

	glm::mat4 trans =
		glm::mat4(
			glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
			glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
			glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
			glm::vec4(trans_amt, 0.0f, 1.0f)
		);

	glUniformMatrix4fv(
			text_render_program->OBJECT_TO_CLIP_mat4,
			1,
			GL_FALSE,
			glm::value_ptr(proj * trans * scale_mat)
	);
	glUniform4f(
			text_render_program->COLOR_vec4,
			color.r / 256.0f,
			color.g / 256.0f,
			color.b / 256.0f,
			color.a / 256.0f
	);

	glBindVertexArray(vao);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, atlas);

	glDrawArrays(GL_TRIANGLES, 0, GLsizei(vertices.size()));

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	GL_ERRORS();

}
