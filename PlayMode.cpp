#include "PlayMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include <random>
#include <bitset>

PlayMode::PlayMode() {
	load_tiles(&ppu);

	//https://en.cppreference.com/w/cpp/numeric/random/rand#:~:text=If%20rand()%20is%20used,of%20values%20on%20successive%20calls.
	std::srand(std::time(nullptr)); 

	//use sprite 0-4 as a "player":


	//makes the outside of tiles 0-16 solid:
	ppu.palette_table[0] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

	/*
		Tile table:
		0-15: player
		20-35: enemy
		36-39: bush
		40: coin
		41-44: grass
		45-55: numbers
	*/
	
	
	/*
		Palette Table (as shown in palettes.png) (bottom to top)
		0: 
		1: number color
		2: grass color
		3: raccoon color
		4: coin color (also victory color)
		5: bush color
		6: second duck color
		7: used for the player (duck color 1)
	*/

	// create grassland
	for (uint32_t y = 0; y < PPU466::BackgroundHeight; ++y) {
		for (uint32_t x = 0; x < PPU466::BackgroundWidth; ++x) {
			map[x+PPU466::BackgroundWidth*y] = (0x02 << 8) | (0x29 + (std::rand() % 4));
		}
	}

	// add in bushes <- later TODO: add in different bush configurations
	for (uint32_t y = 2; y < PPU466::BackgroundHeight; y+=6) {
		for (uint32_t x = 3; x < PPU466::BackgroundWidth; x+=8) {
			map[x+PPU466::BackgroundWidth*y] = (0x05 << 8) | (0x24);
			map[(x + 1)+PPU466::BackgroundWidth*y] = (0x05 << 8) | (0x25);
			map[x+PPU466::BackgroundWidth*(y + 1)] = (0x05 << 8) | (0x26);
			map[(x + 1)+PPU466::BackgroundWidth*(y + 1)] = (0x05 << 8) | (0x27);
		}
	} // 2,3

	// add in time in the bottom left corner
	map[0] = (0x01 << 8) | (0x37); // starts at 10
	map[1] = (0x01 << 8) | (0x2D); // starts at 0

	std::vector<uint32_t> key;

	for (uint32_t coin_index = 0; coin_index < 56; coin_index++){
		coinx[coin_index] = (((std::rand() % 220) + 4) / 8) * 8 + 8;
		coiny[coin_index] = (((std::rand() % 220) + 4) / 8) * 8 + 8;
		
		key = {uint32_t(coinx[coin_index]), uint32_t(coiny[coin_index])};
		coin_pos[key] = coin_index;
	}
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.downs += 1;
			down.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RSHIFT) {
			hide.downs += 1;
			hide.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_SLASH) {
			unhide.downs += 1;
			unhide.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_a) {
			a.downs += 1;
			a.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			d.downs += 1;
			d.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			w.downs += 1;
			w.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			s.downs += 1;
			s.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_e) {
			shoot.downs += 1;
			shoot.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_q) {
			prowl.downs += 1;
			prowl.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RSHIFT) {
			hide.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_SLASH) {
			unhide.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_a) {
			a.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			d.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			w.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			s.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_e) {
			shoot.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_q) {
			prowl.pressed = false;
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {
	bg_fade = std::sin(elapsed *100.0f);
	bg_fade -= std::floor(bg_fade);

	duck_time = std::min(duck_time + elapsed, 10.f);
	raccoon_time = std::min(raccoon_time + elapsed, 10.f);
	map[0] = (0x03 << 8) | (0x37) - (int)raccoon_time;
	map[1] = (0x01 << 8) | (0x37) - (int)duck_time;

	constexpr float PlayerSpeed = 30.0f;

	if((duck_time < 10.f) && !hidden){
		if (left.pressed) {
			player_at.x -= PlayerSpeed * elapsed;
			player_at.x = std::clamp(player_at.x, 0.f, 240.f);
			player_offset = 0;
		}
		if (right.pressed) {
			player_at.x += PlayerSpeed * elapsed;
			player_at.x = std::clamp(player_at.x, 0.f, 240.f);
			player_offset = 4;
		}
		if (down.pressed) {
			player_at.y -= PlayerSpeed * elapsed;
			player_at.y = std::clamp(player_at.y, 0.f, 240.f);
			player_offset = 8;
		}
		if (up.pressed) {
			player_at.y += PlayerSpeed * elapsed;
			player_at.y = std::clamp(player_at.y, 0.f, 240.f);
			player_offset = 12;
		}
		if (hide.pressed) {
			uint16_t posx = (uint16_t)std::round(player_at.x / 8 - 3);
			uint16_t posy = (uint16_t)std::round(player_at.y / 8 - 2);

			if((posx %8 <= 1 | posx %8 >= 7) && (posy %6 <= 1 | posy %6 >= 5)){
				hidden = true;
				player_hide = 0x80;
				player_at.x = (std::round((float)posx/8.f)*8 + 3) * 8.f;
				player_at.y = (std::round((float)posy/6.f)*6 + 2) * 8.f;
				duck_time = 10.f;
			}
		}
	}

	if(duck_time >= 10.f && raccoon_time >= 10.f) {
		if (unhide.pressed && !has_gone) {
			hidden = false;
			player_hide = 0x00;
			duck_time = 0.f;
			has_gone = true;
		}

		if(prowl.pressed && has_gone) {
			raccoon_time = 0.f;
			has_gone = false;
		}
	}

	if(raccoon_time < 10.f){
		if (a.pressed) {
			enemy_at.x -= PlayerSpeed * elapsed;
			enemy_at.x = std::clamp(enemy_at.x, 0.f, 240.f);
			enemy_offset = 0;
		}
		if (d.pressed) {
			enemy_at.x += PlayerSpeed * elapsed;
			enemy_at.x = std::clamp(enemy_at.x, 0.f, 240.f);
			enemy_offset = 4;
		}
		if (s.pressed) {
			enemy_at.y -= PlayerSpeed * elapsed;
			enemy_at.y = std::clamp(enemy_at.y, 0.f, 240.f);
			enemy_offset = 8;
		}
		if (w.pressed) {
			enemy_at.y += PlayerSpeed * elapsed;
			enemy_at.y = std::clamp(enemy_at.y, 0.f, 240.f);
			enemy_offset = 12;
		}
		if (shoot.pressed) {
			if(!hidden){ // duck didn't hide in time
				raccoon_color = 4;
			} else {
				uint16_t eposx = (uint16_t)std::round(enemy_at.x / 8 - 3);
				uint16_t eposy = (uint16_t)std::round(enemy_at.y / 8 - 2);

				if((eposx %8 <= 1 | eposx %8 >= 7) && (eposy %6 <= 1 | eposy %6 >= 5)){
					// check if player is there
					uint16_t posx = (uint16_t)std::round(player_at.x / 8 - 3);
					uint16_t posy = (uint16_t)std::round(player_at.y / 8 - 2);
					float_t px = (std::round((float)posx/8.f)*8 + 3) * 8.f;
					float_t py = (std::round((float)posy/6.f)*6 + 2) * 8.f;
					float_t ex = (std::round((float)eposx/8.f)*8 + 3) * 8.f;
					float_t ey = (std::round((float)eposy/6.f)*6 + 2) * 8.f;
					if(px == ex && py == ey){
						hidden = false;
						player_hide = 0x00;
						raccoon_color = gold_color;
					}
					raccoon_time = 10.f;
				}
			}
		}
	}

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
	hide.downs = 0;
	unhide.downs = 0;

	w.downs = 0;
	a.downs = 0;
	s.downs = 0;
	d.downs = 0;
	shoot.downs = 0;
	prowl.downs = 0;

	// check if duck has collected any coins
	uint32_t playerxrounded = ((uint32_t(player_at.x) + 4) / 8) * 8;
	uint32_t playeryrounded = ((uint32_t(player_at.y) + 4) / 8) * 8;
	std::vector<uint32_t> key;
	for (uint32_t i = 0; i < 2; ++i) {
		for (uint32_t j = 0; j < 2; ++j) {
			key = {uint32_t(playerxrounded + i*8), uint32_t(playeryrounded + j*8)};
			if(coin_pos.count(key) > 0){
				uint32_t coin_index = coin_pos[key];
				coinx[coin_index] = (uint8_t)500;
				coiny[coin_index] = (uint8_t)500;
				coin_pos.erase(key);
				coins_collected++;
			}
		}
	}

	if(coins_collected > coins_needed){
		duck_color = gold_color;
	}
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---

	int32_t r = int32_t((1.f-bg_fade)*0x1C + (bg_fade * 0x8D));
	int32_t g = int32_t((1.f-bg_fade)*0xBD + (bg_fade * 0xC2));
	int32_t b = int32_t((1.f-bg_fade)*0x1C + (bg_fade * 0x48));

	ppu.background_color = glm::u8vec4(r,g,b,0xFF);

	for (uint32_t y = 0; y < PPU466::BackgroundHeight; ++y) {
		for (uint32_t x = 0; x < PPU466::BackgroundWidth; ++x) {
			ppu.background[x+PPU466::BackgroundWidth*y] = map[x+PPU466::BackgroundWidth*y];
		}
	}

	// coin sprites
	for (uint32_t i = 8; i < 64; ++i) {
		ppu.sprites[i].x = coinx[i-8];
		ppu.sprites[i].y = coiny[i-8];
		ppu.sprites[i].index = 40;
		ppu.sprites[i].attributes = gold_color;
	}

	//player sprite:
	for (uint32_t i = 0; i < 4; ++i) {
		ppu.sprites[i].x = int8_t(player_at.x) + (8 * (i & 0x01));
		ppu.sprites[i].y = int8_t(player_at.y) + (8 * ((i >> 1) & 0x01));
		ppu.sprites[i].index = i + player_offset;
		ppu.sprites[i].attributes = duck_color | player_hide;
	}

	// enemy sprite:
	for (uint32_t i = 4; i < 8; ++i) {
		ppu.sprites[i].x = int8_t(enemy_at.x) + (8 * (i & 0x01));
		ppu.sprites[i].y = int8_t(enemy_at.y) + (8 * ((i>> 1) & 0x01));
		ppu.sprites[i].index = 16 + i + enemy_offset;
		ppu.sprites[i].attributes = raccoon_color;
	}

	//std::cout << int32_t(player_at.x) << " " << int32_t(player_at.y) << std::endl;

	//--- actually draw ---
	ppu.draw(drawable_size);
}

