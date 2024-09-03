#include "PlayMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include <random>

PlayMode::PlayMode() {
	load_tiles(&ppu);

	//use sprite 0-4 as a "player":


	//makes the outside of tiles 0-16 solid:
	ppu.palette_table[0] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

	//makes the center of tiles 0-16 solid:
	ppu.palette_table[1] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

	/*
		Palette Table (as shown in palettes.png) (bottom to top)
		0:
		1:
		2: grass color
		3: raccoon color
		4: coin color
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

	// add in bushes <- later todo: add in different bush configurations
	for (uint32_t y = 2; y < PPU466::BackgroundHeight; y+=6) {
		for (uint32_t x = 3; x < PPU466::BackgroundWidth; x+=8) {
			map[x+PPU466::BackgroundWidth*y] = (0x05 << 8) | (0x24);
			map[(x + 1)+PPU466::BackgroundWidth*y] = (0x05 << 8) | (0x25);
			map[x+PPU466::BackgroundWidth*(y + 1)] = (0x05 << 8) | (0x26);
			map[(x + 1)+PPU466::BackgroundWidth*(y + 1)] = (0x05 << 8) | (0x27);
		}
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
		} else if (evt.key.keysym.sym == SDLK_SLASH) {
			hide.downs += 1;
			hide.pressed = true;
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
		} else if (evt.key.keysym.sym == SDLK_SLASH) {
			hide.pressed = false;
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
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {

	//slowly rotates through [0,1):
	// (will be used to set background color)
	bg_fade = std::sin(elapsed *100.0f);
	bg_fade -= std::floor(bg_fade);

	constexpr float PlayerSpeed = 30.0f;
	if (left.pressed) {
		player_at.x -= PlayerSpeed * elapsed;
		player_offset = 0;
	}
	if (right.pressed) {
		player_at.x += PlayerSpeed * elapsed;
		player_offset = 4;
	}
	if (down.pressed) {
		player_at.y -= PlayerSpeed * elapsed;
		player_offset = 8;
	}
	if (up.pressed) {
		player_at.y += PlayerSpeed * elapsed;
		player_offset = 12;
	}
	if (hide.pressed) {
		player_hide ^= 0x80;
	}
	if (a.pressed) {
		enemy_at.x -= PlayerSpeed * elapsed;
		enemy_offset = 0;
	}
	if (d.pressed) {
		enemy_at.x += PlayerSpeed * elapsed;
		enemy_offset = 4;
	}
	if (s.pressed) {
		enemy_at.y -= PlayerSpeed * elapsed;
		enemy_offset = 8;
	}
	if (w.pressed) {
		enemy_at.y += PlayerSpeed * elapsed;
		enemy_offset = 12;
	}
	if (shoot.pressed) {
		
	}

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
	hide.downs = 0;

	w.downs = 0;
	a.downs = 0;
	s.downs = 0;
	d.downs = 0;
	shoot.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---

	int32_t r = int32_t((1.f-bg_fade)*0x1C + (bg_fade * 0x8D));
	int32_t g = int32_t((1.f-bg_fade)*0xBD + (bg_fade * 0xC2));
	int32_t b = int32_t((1.f-bg_fade)*0x1C + (bg_fade * 0x48));

	ppu.background_color = glm::u8vec4(r,g,b,0xFF);

	// just do a grid of grass for now
	for (uint32_t y = 0; y < PPU466::BackgroundHeight; ++y) {
		for (uint32_t x = 0; x < PPU466::BackgroundWidth; ++x) {
			ppu.background[x+PPU466::BackgroundWidth*y] = map[x+PPU466::BackgroundWidth*y];
		}
	}

	//player sprite:
	for (uint32_t i = 0; i < 4; ++i) {
		ppu.sprites[i].x = int8_t(player_at.x) + (8 * (i & 0x01));
		ppu.sprites[i].y = int8_t(player_at.y) + (8 * ((i >> 1) & 0x01));
		ppu.sprites[i].index = i + player_offset;
		ppu.sprites[i].attributes = 7 | player_hide;
	}

	// enemy sprite:
	for (uint32_t i = 4; i < 8; ++i) {
		ppu.sprites[i].x = int8_t(enemy_at.x) + (8 * (i & 0x01));
		ppu.sprites[i].y = int8_t(enemy_at.y) + (8 * ((i>> 1) & 0x01));
		ppu.sprites[i].index = 16 + i + enemy_offset;
		ppu.sprites[i].attributes = 3;
	}

	// coin sprites
	for (uint32_t i = 8; i < 63; ++i) {
		ppu.sprites[i].x = int8_t(PPU466::ScreenWidth/56 * (i-8));
		ppu.sprites[i].y = int8_t(PPU466::ScreenHeight/ 56 * (i-8));
		ppu.sprites[i].index = 40;
		ppu.sprites[i].attributes = 4;
	}

	//--- actually draw ---
	ppu.draw(drawable_size);
}

