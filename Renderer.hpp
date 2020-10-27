#pragma once

#include "Scene.hpp"
#include "TextRenderer.hpp"
#include "glm/fwd.hpp"
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

	// screen textures/fbos
	const glm::uvec2 tex_size = glm::uvec2(960, 540);
	std::array<GLuint, 2> fbos;
	std::array<GLuint, 2> texs;

	// text vertices
	std::vector<TextRenderer::Vertex> manager_verts;
	std::list<std::vector<TextRenderer::Vertex>> text_p1;
	std::list<std::vector<TextRenderer::Vertex>> text_p2;
	std::vector<TextRenderer::Vertex> score_p1;
	std::vector<TextRenderer::Vertex> score_p2;


	// functions/variables to change to update render behavior
	// 0 = all time remaining, 1 = no time remaining
	void set_time_remaining(float time_remaining);

	void update_p1(size_t new_chars, int score);
	void update_p2(size_t new_chars, int score);

	void update_manager_text(std::string new_text);

};
