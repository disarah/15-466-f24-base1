#include "PPU466.hpp"
#include "Mode.hpp"
#include "asset_pipeline.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include <map>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up, hide, unhide, w, a, s, d, shoot, prowl;

	// game variables
	bool hidden = false; // hidden state of duck
	bool has_gone = true; // prevents duck/raccoon from taking multiple turns
	float duck_time = 0; // 10 second increments
	float raccoon_time = 10; // 10 second increments

	uint16_t coins_collected = 0;
	uint16_t coins_needed = 50;

	uint16_t raccoon_color = 3;
	uint16_t gold_color = 4;
	uint16_t duck_color = 7;

	//some weird background animation:
	float bg_fade = 0.0f;

	std::array< uint16_t, PPU466::BackgroundWidth * PPU466::BackgroundHeight > map;
	std::array< uint8_t, 56 > coinx; // Todo: vary # of coins
	std::array< uint8_t, 56 > coiny; // Todo: vary # of coins
	std::map<std::vector<uint32_t>, uint32_t> coin_pos; // map of coin location to index

	//player position:
	glm::vec2 player_at = glm::vec2(PPU466::ScreenWidth/2, PPU466::ScreenHeight/2);
	glm::vec2 enemy_at = glm::vec2(PPU466::ScreenWidth/2 - 16, PPU466::ScreenHeight/2);
	uint8_t player_offset = 0;
	uint8_t player_hide = 0;
	uint8_t enemy_offset = 0;

	//----- drawing handled by PPU466 -----

	PPU466 ppu;
};
