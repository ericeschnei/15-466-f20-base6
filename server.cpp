
#include "Connection.hpp"

#include "hex_dump.hpp"

#include <chrono>
#include <stdexcept>
#include <iostream>
#include <cassert>
#include <unordered_map>

//Some quick helper functions
//Use them wisely as they could break if not careful
//float char_to_float(std::vector<char> buffer, int index) {
//	return float((uint32_t(buffer[index]) << 24) |
//		(uint32_t(buffer[index + 1]) << 16) |
//		(uint32_t(buffer[index + 2]) << 8) |
//		(uint32_t(buffer[index + 3])));
//}

uint32_t char_to_uint32(std::vector<char> buffer, int index) {
	return ((uint32_t(buffer[index]) << 24) |
		(uint32_t(buffer[index + 1]) << 16) |
		(uint32_t(buffer[index + 2]) << 8) |
		(uint32_t(buffer[index + 3])));
}

// END helper functions

int main(int argc, char **argv) {
#ifdef _WIN32
	//when compiled on windows, unhandled exceptions don't have their message printed, which can make debugging simple issues difficult.
	try {
#endif

	//------------ argument parsing ------------

	if (argc != 2) {
		std::cerr << "Usage:\n\t./server <port>" << std::endl;
		return 1;
	}

	//------------ initialization ------------

	Server server(argv[1]);


	//------------ main loop ------------
	constexpr float ServerTick = 1.0f / 10.0f; //TODO: set a server tick that makes sense for your game

	//server state:
	int32_t total_time = 60000; //60 seconds in milliseconds
	int32_t time_remaining = 60000;

	//per-client state:
	struct PlayerInfo {
		PlayerInfo() {
			static uint32_t next_player_id = 1;
			name = "Player" + std::to_string(next_player_id);
			next_player_id += 1;
		}
		std::string name;

		uint32_t total_characters_pressed = 0; //Can get rid of total in PlayMode now
		uint32_t num_new_characters = 0;
		uint32_t current_score = 0;
		uint32_t manager_command = 0;

	};
	std::unordered_map< Connection *, PlayerInfo > players;

	while (true) {
		static auto next_tick = std::chrono::steady_clock::now() + std::chrono::duration< double >(ServerTick);
		//process incoming data from clients until a tick has elapsed:
		while (true) {
			auto now = std::chrono::steady_clock::now();
			double remain = std::chrono::duration< double >(next_tick - now).count();
			if (remain < 0.0) {
				next_tick += std::chrono::duration< double >(ServerTick);
				break;
			}
			server.poll([&](Connection *c, Connection::Event evt){
				if (evt == Connection::OnOpen) {
					//client connected:

					//create some player info for them:
					players.emplace(c, PlayerInfo());


				} else if (evt == Connection::OnClose) {
					//client disconnected:

					//remove them from the players list:
					auto f = players.find(c);
					assert(f != players.end());
					players.erase(f);


				} else { assert(evt == Connection::OnRecv);
					//got data from client:
					std::cout << "got bytes:\n" << hex_dump(c->recv_buffer); std::cout.flush();

					//look up in players list:
					auto f = players.find(c);
					assert(f != players.end());
					PlayerInfo &player = f->second;

					//handle messages from client:
					//TODO: update for the sorts of messages your clients send
					while (c->recv_buffer.size() >= 3) {
						//expecting three-byte messages 'b', char_count, and score
						char type = c->recv_buffer[0];
						if (type != 'b') {
							std::cout << " message of non-'b' type received from client!" << std::endl;
							//shut down client connection:
							c->close();
							return;
						}
						
					/*	int index = 1;
						player.num_new_characters = char_to_uint32(c->recv_buffer, index);
						player.total_characters_pressed += player.num_new_characters;
						index += 4;
						player.current_score += char_to_uint32(c->recv_buffer, index);
						index += 4;*/
						player.num_new_characters = c->recv_buffer[1];
						player.current_score = c->recv_buffer[2];
						std::cout << "from client: " << player.name << " " <<
							player.num_new_characters << " " << player.current_score << "\n";
						//TODO remove
						//assert(index == 9);

						c->recv_buffer.erase(c->recv_buffer.begin(), c->recv_buffer.begin() + 3);
					}
				}
			}, remain);
		}

		//update current game state
		//TODO: replace with *your* game state update
		

		
		uint32_t command = 0; //TODO rand value here
		time_remaining -= (int32_t)(ServerTick * 10);
		uint32_t p1_new_chars, p1_score, p2_new_chars, p2_score;
		for (auto &[c, player] : players) {
			(void)c; //work around "unused variable" warning on whatever version of g++ github actions is running
			if (!player.name.compare("Player1")) {
				p1_new_chars = player.num_new_characters;
				p1_score = player.current_score;
			}
			else if (!player.name.compare("Player2")) {
				p2_new_chars = player.num_new_characters;
				p2_score = player.current_score;
			}
		}
		//std::cout << status_message << std::endl; //DEBUG

		//send updated game state to all clients
		//TODO: update for your game state
		for (auto &[c, player] : players) {
			(void)player; //work around "unused variable" warning on whatever g++ github actions uses
			//send an update starting with 'm':
			c->send('m');
			std::string msg = std::to_string(command) + "," + std::to_string(time_remaining) + "," + std::to_string(total_time)
				+ "," + std::to_string(p1_new_chars) + "," + std::to_string(p1_score) + "," + std::to_string(p2_new_chars) + "," + std::to_string(p2_score);

			c->send(uint8_t(msg.size() >> 16));
			c->send(uint8_t((msg.size() >> 8) % 256));
			c->send(uint8_t(msg.size() % 256));
			c->send_buffer.insert(c->send_buffer.end(), msg.begin(), msg.end());

			/*c->send(uint8_t(command));
			c->send(uint8_t(time_remaining >> 24));
			c->send(uint8_t((time_remaining >> 16) % 256));
			c->send(uint8_t((time_remaining >> 8) % 256));
			c->send(uint8_t(time_remaining % 256));

			c->send(uint8_t(total_time >> 24));
			c->send(uint8_t((total_time >> 16) % 256));
			c->send(uint8_t((total_time >> 8) % 256));
			c->send(uint8_t(total_time % 256));
			
			c->send(p1_new_chars);
			c->send(p1_score);
			c->send(p2_new_chars);
			c->send(p2_score);*/
			//c->send_buffer.insert(c->send_buffer.end(), status_message.begin(), status_message.end());
		}

	}


	return 0;

#ifdef _WIN32
	} catch (std::exception const &e) {
		std::cerr << "Unhandled exception:\n" << e.what() << std::endl;
		return 1;
	} catch (...) {
		std::cerr << "Unhandled exception (unknown type)." << std::endl;
		throw;
	}
#endif
}
