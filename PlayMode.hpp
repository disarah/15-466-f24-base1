#include "PPU466.hpp"
#include "Mode.hpp"
#include "asset_pipeline.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

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
	} left, right, down, up, hide, w, a, s, d, shoot;

	//some weird background animation:
	float bg_fade = 0.0f;

	std::array< uint16_t, PPU466::BackgroundWidth * PPU466::BackgroundHeight > map;

	//player position:
	glm::vec2 player_at = glm::vec2(PPU466::ScreenWidth/2, PPU466::ScreenHeight/2);
	glm::vec2 enemy_at = glm::vec2(PPU466::ScreenWidth/2 - 16, PPU466::ScreenHeight/2);
	uint8_t player_offset = 0;
	uint8_t player_hide = 0;
	uint8_t enemy_offset = 0;

	//----- drawing handled by PPU466 -----

	PPU466 ppu;
};
