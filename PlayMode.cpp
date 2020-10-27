#include "PlayMode.hpp"

#include "DrawLines.hpp"
#include "GL.hpp"
#include "SDL_keycode.h"
#include "SDL_scancode.h"
#include "gl_errors.hpp"
#include "data_path.hpp"
#include "hex_dump.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>
#include <unordered_set>

GLuint keyboard_meshes_for_color_texture_program = 0;

// Some quick helper functions
// Use them wisely as they could break if not careful
float char_to_float(std::vector<char> buffer, int index) {
	return ((float(buffer[index]) << 24) | 
		    (float(buffer[index + 1]) << 16) | 
		    (float(buffer[index + 2]) << 8) | 
		    (float(buffer[index + 3])));
}

uint32_t char_to_uint32(std::vector<char> buffer, int index) {
	return ((uint32_t(buffer[index]) << 24) |
		    (uint32_t(buffer[index + 1]) << 16) |
		    (uint32_t(buffer[index + 2]) << 8) |
		    (uint32_t(buffer[index + 3])));
}

// END helper functions

PlayMode::PlayMode(Client &client_) : client(client_) {
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	// @pablo: i took this out and then ended up not reimplementing anything.
	// you're probably going to have to write SDL_SCANCODE_ a bunch of times.
	// https://wiki.libsdl.org/SDL_KeyboardEvent

	/*
	// There really has to be a better way to do this... -Pablo
	// Maybe an unordered map with the keys being the keycode and values being
	// the corresponding Button? but would still be ugly to set up and this is already done:/ -Pablo
	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.repeat) {
			//Do nothing to ignore repeat
		} 
		else if (evt.key.keysym.sym == SDLK_a) {
			a.downs += 1;
			a.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_b) {
			b.downs += 1;
			b.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_c) {
			c.downs += 1;
			c.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_d) {
			d.downs += 1;
			d.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_e) {
			e.downs += 1;
			e.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_f) {
			f.downs += 1;
			f.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_g) {
			g.downs += 1;
			g.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_h) {
			h.downs += 1;
			h.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_i) {
			i.downs += 1;
			i.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_j) {
			j.downs += 1;
			j.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_k) {
			k.downs += 1;
			k.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_l) {
			l.downs += 1;
			l.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_m) {
			m.downs += 1;
			m.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_n) {
			n.downs += 1;
			n.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_o) {
			o.downs += 1;
			o.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_p) {
			p.downs += 1;
			p.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_q) {
			q.downs += 1;
			q.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_r) {
			r.downs += 1;
			r.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_s) {
			s.downs += 1;
			s.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_t) {
			t.downs += 1;
			t.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_u) {
			u.downs += 1;
			u.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_v) {
			v.downs += 1;
			v.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_w) {
			w.downs += 1;
			w.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_x) {
			x.downs += 1;
			x.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_y) {
			y.downs += 1;
			y.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_z) {
			z.downs += 1;
			z.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_SPACE) {
			space.downs += 1;
			space.pressed = true;
			return true;
		}
	}
	else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_a) {
			a.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_b) {
			b.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_c) {
			c.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_d) {
			d.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_e) {
			e.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_f) {
			f.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_g) {
			g.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_h) {
			h.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_i) {
			i.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_j) {
			j.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_k) {
			k.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_l) {
			l.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_m) {
			m.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_n) {
			n.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_o) {
			o.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_p) {
			p.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_q) {
			q.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_r) {
			r.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_s) {
			s.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_t) {
			t.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_u) {
			u.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_v) {
			v.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_w) {
			w.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_x) {
			x.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_y) {
			y.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_z) {
			z.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_SPACE) {
			space.pressed = false;
			return true;
		}
	}
	*/
	
	// Finally that abomination is gone -Pablo
	SDL_KeyCode key = evt.key.keysym.sym;
	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.repeat) {
			//Do nothing
		}
		else if (buttons.find(key) != buttons.end) {
			buttons[key].downs += 1;
			buttons[key].total_downs += 1; // for funsies value
			buttons[key].pressed = true;
			return true;
		}
	}
	else if (evt.type == SDL_KEYUP) {
		if (buttons.find(key) != buttons.end) {
			buttons[key].pressed = false;
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {
	// Find out score and how many keys were mashed
	for (auto butt_it : buttons) {
		if (butt_it.second.downs) {
			int multiplier = 1;
			if (false/*button is in active cluster*/) {
				// If button is in the active key cluster, multiply points
				multiplier += 1;
			}
			new_score_to_add += (multiplier * butt_it.second.downs);
			num_new_chars_typed += butt_it.second.downs;
			butt_it.second.downs = 0; // reset value
		}
	}
	
	//queue data for sending to server:
	if (num_new_chars_typed || new_score_to_add) {
		//send a nine-byte message of type 'b':
		client.connections.back().send('b');
		client.connections.back().send(num_new_chars_typed);
		client.connections.back().send(new_score_to_add);
	}


	//send/receive data:
	client.poll([this](Connection *c, Connection::Event event){
		if (event == Connection::OnOpen) {
			std::cout << "[" << c->socket << "] opened" << std::endl;
		} else if (event == Connection::OnClose) {
			std::cout << "[" << c->socket << "] closed (!)" << std::endl;
			throw std::runtime_error("Lost connection to server!");
		} else { assert(event == Connection::OnRecv);
			std::cout << "[" << c->socket << "] recv'd data. Current buffer:\n" << hex_dump(c->recv_buffer); std::cout.flush();
			//expecting message(s) like 'm' + 28-bytes:
			while (c->recv_buffer.size() >= 4) {
				std::cout << "[" << c->socket << "] recv'd data. Current buffer:\n" << hex_dump(c->recv_buffer); std::cout.flush();
				char type = c->recv_buffer[0];
				if (type != 'm') {
					throw std::runtime_error("Server sent unknown message type '" + std::to_string(type) + "'");
				}
				/* currently unneeded, may reimplement if I later decide to have the server deliver custom messages instead of canned ones
				uint32_t size = (
					(uint32_t(c->recv_buffer[1]) << 16) | (uint32_t(c->recv_buffer[2]) << 8) | (uint32_t(c->recv_buffer[3]))
				);
				if (c->recv_buffer.size() < 4 + size) break; //if whole message isn't here, can't process
				//whole message *is* here, so set current server message:
				*/
				// server_message = std::string(c->recv_buffer.begin() + 4, c->recv_buffer.begin() + 4 + size);

				// Interpret server_message
				/*uint32_t manager_command = ( // uint32 to leave space for 3 size bits and a uint8 in case plans change
					(uint32_t(c->recv_buffer[1]) << 24) | (uint32_t(c->recv_buffer[2]) << 16) | (uint32_t(c->recv_buffer[3] << 8)) | (uint32_t(c->recv_buffer[4]))
				);*/
				int index = 1; // because I don't trust myself with basic arithmetic
				uint32_t manager_command = char_to_uint32(c->recv_buffer, index); // uint32 to leave space for 3 size bits and a uint8 in case plans change
				index += 4;
				float time_remaining = char_to_float(c->recv_buffer, index);
				index += 4;
				float time_total = char_to_float(c->recv_buffer, index);
				index += 4;
				uint32_t p1_new_chars = char_to_uint32(c->recv_buffer, index);
				index += 4;
				uint32_t p1_score = char_to_uint32(c->recv_buffer, index);
				index += 4;
				uint32_t p2_new_chars = char_to_uint32(c->recv_buffer, index);
				index += 4;
				uint32_t p2_score = char_to_uint32(c->recv_buffer, index);
				index += 4;

				//TODO remove
				assert(index == 29);

				//and consume this part of the buffer:
				c->recv_buffer.erase(c->recv_buffer.begin(), c->recv_buffer.begin() + index);

				// Update and reset game state for client
				num_new_chars_typed = 0;
				new_score_to_add = 0;
				current_command = manager_command;

				// Update renderer for client
				//renderer.set_time_remaining(0.25f);
				if (time_remaining <= .0f) {
					return;
				}
				renderer.set_time_remaining(time_remaining / time_total);

				renderer.update_manager_text(commands[manager_command]);
				renderer.update_p1(size_t(p1_new_chars), p1_score);
				renderer.update_p2(size_t(p2_new_chars), p2_score);

				renderer.update(elapsed);
			}
		}
	}, 0.0);

	

	
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	renderer.draw(drawable_size);
	GL_ERRORS();
}
