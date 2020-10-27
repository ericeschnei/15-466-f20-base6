#include "Mode.hpp"

#include "Connection.hpp"

#include "Renderer.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include <unordered_map>

struct PlayMode : Mode {
	PlayMode(Client &client);
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	// Player updates
	uint32_t num_new_chars_typed = 0;
	uint32_t new_score_to_add = 0;

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
		uint32_t total_downs = 0;
	};

	std::unordered_map<SDL_KeyCode, Button> buttons{
		{SDLK_a, Button()},
		{SDLK_b, Button()},
		{SDLK_c, Button()},
		{SDLK_d, Button()},
		{SDLK_e, Button()},
		{SDLK_f, Button()},
		{SDLK_g, Button()},
		{SDLK_h, Button()},
		{SDLK_i, Button()},
		{SDLK_j, Button()},
		{SDLK_k, Button()},
		{SDLK_l, Button()},
		{SDLK_m, Button()},
		{SDLK_n, Button()},
		{SDLK_o, Button()},
		{SDLK_p, Button()},
		{SDLK_q, Button()},
		{SDLK_r, Button()},
		{SDLK_s, Button()},
		{SDLK_t, Button()},
		{SDLK_u, Button()},
		{SDLK_v, Button()},
		{SDLK_w, Button()},
		{SDLK_x, Button()},
		{SDLK_y, Button()},
		{SDLK_z, Button()},
		{SDLK_SPACE, Button()}
	};


	//last message from server:
	std::string server_message;

	//connection to server:
	Client &client;

	Renderer renderer;

	int current_command = 0;

	enum commands{
		"Corporate wants you to MASH YOUR KEYBOARD",
		"Corporate wants letters from the LEFT SIDE OF THE KEYBOARD",
		"Corporate wants letters from the RIGHT SIDE OF THE KEYBOARD"
	};


};
