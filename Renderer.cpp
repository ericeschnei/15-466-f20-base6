#include "Renderer.hpp"
#include "GL.hpp"
#include "Load.hpp"
#include "Mesh.hpp"
#include "TextRenderer.hpp"
#include "data_path.hpp"
#include "gl_errors.hpp"
#include "glm/common.hpp"
#include "LitColorTextureProgram.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <cstdio>
#include <stdexcept>
#include <string>

#include <iostream>

#define PROGRAM lit_color_texture_program
#define PIPELINE lit_color_texture_program_pipeline
GLuint meshes_for_program = 0;

Load< MeshBuffer > meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("game6.pnct"));
	meshes_for_program = ret->make_vao_for_program(PROGRAM->program);
	return ret;
});

Load< Scene > scene_(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("game6.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = PIPELINE;

		drawable.pipeline.vao = meshes_for_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

	});
});

Renderer::Renderer() : scene(*scene_) {

	// OPENGL: initialize textures
	// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
	{
		glGenFramebuffers(2, fbos.data());
		glGenTextures(2, texs.data());

		for (size_t i = 0; i < 2; i++) {
			glBindFramebuffer(GL_FRAMEBUFFER, fbos[i]);

			glBindTexture(GL_TEXTURE_2D, texs[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_size.x, tex_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texs[i], 0);

			glBindTexture(GL_TEXTURE_2D, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			GL_ERRORS();

		}
	}

	// get convenient pointers
	if (scene.cameras.size() != 1) {
		throw std::runtime_error("Expected exactly one camera, but"
			"found " + std::to_string(scene.cameras.size()));
	}
	camera = &scene.cameras.front();

	for (auto &drawable : scene.drawables) {
		const std::string &name = drawable.transform->name;
		if (name == "clock_hand") {
			clock_hand = drawable.transform;
		} else if (name == "lhand.002") {
			lhand2 = drawable.transform;
		} else if (name == "lhand") {
			lhand1 = drawable.transform;
		} else if (name == "rhand.002") {
			rhand2 = drawable.transform;
		} else if (name == "rhand") {
			rhand1 = drawable.transform;
		} else if (name == "screen") {
			drawable.pipeline.textures[0].texture = texs[0];
		} else if (name == "screen.002") {
			drawable.pipeline.textures[0].texture = texs[1];
		}

	}

	{ // initialize renderer
		TextRenderer::load_font(FONT_SIZE, data_path("mononoki.ttf"));
		TextRenderer::get_string("Test string! o_O", manager_verts, -1.0f);
	}

}
Renderer::~Renderer() {}


void Renderer::update(float time_elapsed) {}
void Renderer::update_manager_text(std::string new_text) {}
void Renderer::update_p1(size_t new_chars, int score) {}
void Renderer::update_p2(size_t new_chars, int score) {}

void Renderer::set_time_remaining(float time_remaining) {

	float rot = time_remaining * 2.0f * glm::pi<float>();
	clock_hand->rotation = glm::quat(glm::vec3(0.0f, rot, 0.0f));

}

// draw to the screen.
void Renderer::draw(const glm::uvec2 &drawable_size) {

	// draw screens first
	glBindFramebuffer(GL_FRAMEBUFFER, fbos[0]);
	glViewport(0, 0, tex_size.x, tex_size.y);
	glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, fbos[1]);
	glViewport(0, 0, tex_size.x, tex_size.y);
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// clamp viewport to 16x9 respolution
	glViewport(0, 0, drawable_size.x, drawable_size.y);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::uvec2 size = glm::uvec2(
		glm::min(drawable_size.x, drawable_size.y * 16 / 9),
		glm::min(drawable_size.y, drawable_size.x * 9 / 16)
	);
	glm::uvec2 margin = drawable_size - size;
	glViewport(margin.x / 2, margin.y / 2, size.x, size.y);
	//update camera aspect ratio for drawable:
	camera->aspect = float(size.x) / float(size.y);

	glUseProgram(PROGRAM->program);
	glUniform1i(PROGRAM->LIGHT_TYPE_int, 1);
	GL_ERRORS();
	glUniform3fv(PROGRAM->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
	GL_ERRORS();
	glUniform3fv(PROGRAM->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
	GL_ERRORS();

	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	scene.draw(*camera);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	TextRenderer::render(size, manager_verts, glm::vec2(0.5, 0), 3, glm::u8vec4(255, 255, 0, 255));

	glViewport(0, 0, drawable_size.x, drawable_size.y);
	GL_ERRORS();
}
