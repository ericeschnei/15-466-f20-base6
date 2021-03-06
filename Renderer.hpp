#pragma once

#include "Scene.hpp"
#include "TextRenderer.hpp"
#include <fstream>
#include <glm/glm.hpp>
#include <array>

struct Renderer {

	Renderer();
	~Renderer();

	Scene scene;

	void draw(const glm::uvec2 &drawable_size);

	Scene::Camera *camera = nullptr;
	Scene::Transform *clock_hand = nullptr;
	Scene::Transform *lhand1 = nullptr;
	Scene::Transform *lhand2 = nullptr;
	Scene::Transform *rhand1 = nullptr;
	Scene::Transform *rhand2 = nullptr;

	// text
	static constexpr size_t FONT_SIZE = 128 << 6;

	// screen textures/fbos
	const glm::uvec2 tex_size = glm::uvec2(960, 540);
	std::array<GLuint, 2> fbos;
	std::array<GLuint, 2> texs;

	// text vertices
	std::string words_p1;
	std::string words_p2;
	size_t words_p1_index = 0;
	size_t words_p2_index = 0;
	static constexpr size_t MAX_LETTERS_PER_LINE = 24;
	static constexpr size_t MAX_LINES = 8;
	std::list<char> current_p1;
	std::list<char> current_p2;
	std::vector<TextRenderer::Vertex> current_verts_p1;
	std::vector<TextRenderer::Vertex> current_verts_p2;

	std::vector<TextRenderer::Vertex> manager_verts;
	std::list<std::vector<TextRenderer::Vertex>> text_p1;
	std::list<std::vector<TextRenderer::Vertex>> text_p2;
	std::vector<TextRenderer::Vertex> score_p1;
	std::vector<TextRenderer::Vertex> score_p2;

	// hand stuff
	float p1_hand_pos = 0.0f;
	float p2_hand_pos = 0.0f;
	float p1_hand_vel = 0.0f;
	float p2_hand_vel = 0.0f;
	static constexpr float DAMP_AMT = 12.5f;
	static constexpr float INCR_AMT = 0.2f;
	static constexpr float HAND_HEIGHT = 0.1f;

	// functions/variables to change to update render behavior
	// 0 = all time remaining, 1 = no time remaining
	void set_time_remaining(float time_remaining);

	// update scores and number of characters.
	// new_chars is the number of NEW characters.
	// score is the TOTAL SCORE.
	void update_p1(size_t new_chars, const std::string &score);
	void update_p2(size_t new_chars, const std::string &score);

	// update the manager text. NO PREFIX is assumed--add that yourself
	void update_manager_text(std::string new_text);

	// call in the PlayMode update function
	void update(float time_elapsed);

};
