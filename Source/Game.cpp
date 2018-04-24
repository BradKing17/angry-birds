#include <string>

#include <Engine/Keys.h>
#include <Engine/Input.h>
#include <Engine/InputEvents.h>
#include <Engine/Sprite.h>

#include "Game.h"

/**
*   @brief   Default Constructor.
*   @details Consider setting the game's width and height
             and even seeding the random number generator.
*/
AngryBirdsGame::AngryBirdsGame()
{
	std::srand(time(NULL));
}

/**
*   @brief   Destructor.
*   @details Remove any non-managed memory and callbacks.
*/
AngryBirdsGame::~AngryBirdsGame()
{
	this->inputs->unregisterCallback(key_callback_id);
	this->inputs->unregisterCallback(mouse_callback_id);
}

/**
*   @brief   Initialises the game.
*   @details The game window is created and all assets required to
			 run the game are loaded. The keyHandler and clickHandler
			 callback should also be set in the initialise function.
*   @return  True if the game initialised correctly.
*/
bool AngryBirdsGame::init()
{
	setupResolution();
	if (!initAPI())
	{
		return false;
	}

	toggleFPS();
	renderer->setWindowTitle("Angry Birds!");
	renderer->setWindowedMode(ASGE::Renderer::WindowMode::WINDOWED);
	renderer->setClearColour(ASGE::COLOURS::BLACK);

	// input handling functions
	inputs->use_threads = false;

	key_callback_id = inputs->addCallbackFnc(
		ASGE::E_KEY, &AngryBirdsGame::keyHandler, this);
	
	mouse_callback_id =inputs->addCallbackFnc(
		ASGE::E_MOUSE_CLICK, &AngryBirdsGame::clickHandler, this);

	if (!loadBackgrounds())
	{
		return false;
	}

	if (!menu_layer.addSpriteComponent(renderer.get(), "Resources\\Textures\\menu.jpg"))
	{
		return false;
	}

	setUpGameobjects();

	return true;
}

bool AngryBirdsGame::loadBackgrounds()
{
	std::string filename = "Resources\\Textures\\lvl";
	filename += std::to_string(std::rand() % 3 + 1);
	filename += ".png";

	if (!background_layer.addSpriteComponent(renderer.get(), filename))
	{
		return false;
	}

	return true;
}

/**
*   @brief   Sets the game window resolution
*   @details This function is designed to create the window size, any 
             aspect ratio scaling factors and safe zones to ensure the 
			 game frames when resolutions are changed in size.
*   @return  void
*/
void AngryBirdsGame::setupResolution()
{
	// how will you calculate the game's resolution?
	// will it scale correctly in full screen? what AR will you use?
	// how will the game be framed in native 16:9 resolutions?
	// here are some abritrary values for you to adjust as you see fit
	// https://www.gamasutra.com/blogs/KenanBolukbasi/20171002/306822/Scaling_and_MultiResolution_in_2D_Games.php
	game_width = 1920;
	game_height = 1080;
}

/**
*   @brief   Processes any key inputs
*   @details This function is added as a callback to handle the game's
			 keyboard input. For this game, calls to this function
			 are thread safe, so you may alter the game's state as you
			 see fit.
*   @param   data The event data relating to key input.
*   @see     KeyEvent
*   @return  void
*/
void AngryBirdsGame::keyHandler(const ASGE::SharedEventData data)
{
	auto key = static_cast<const ASGE::KeyEvent*>(data.get());
	
	if (key->key == ASGE::KEYS::KEY_ESCAPE)
	{
		signalExit();
	}
	
	else if (key->key == ASGE::KEYS::KEY_ENTER && 
		     key->action == ASGE::KEYS::KEY_PRESSED &&
		     key->mods == 0x0004)
	{
		if (renderer->getWindowMode() == ASGE::Renderer::WindowMode::WINDOWED)
		{
			renderer->setWindowedMode(ASGE::Renderer::WindowMode::FULLSCREEN);
		}
		else
		{
			renderer->setWindowedMode(ASGE::Renderer::WindowMode::WINDOWED);
		}
	}
	
	else if (in_menu)
	{
		if (key->key == ASGE::KEYS::KEY_SPACE
			&& key->action == ASGE::KEYS::KEY_RELEASED)
		{
			in_menu = false;
			in_tutorial = true;
		}
	}
	else if (in_tutorial)
	{
		if (key->key == ASGE::KEYS::KEY_SPACE
			&& key->action == ASGE::KEYS::KEY_RELEASED)
		{
			in_tutorial = false;
		}
	}
}

/**
*   @brief   Processes any click inputs
*   @details This function is added as a callback to handle the game's
		     mouse button input. For this game, calls to this function
             are thread safe, so you may alter the game's state as you
             see fit.
*   @param   data The event data relating to key input.
*   @see     ClickEvent
*   @return  void
*/
void AngryBirdsGame::clickHandler(const ASGE::SharedEventData data)
{
	auto click = static_cast<const ASGE::ClickEvent*>(data.get());

	double x_pos, y_pos;
	inputs->getCursorPos(x_pos, y_pos);
}


/**
*   @brief   Updates the scene
*   @details Prepares the renderer subsystem before drawing the
		     current frame. Once the current frame is has finished
		     the buffers are swapped accordingly and the image shown.
*   @return  void
*/
void AngryBirdsGame::update(const ASGE::GameTime& us)
{
	if (!in_menu)
	{

	}

	auto dt_sec = us.delta_time.count() / 1000.0;

	//make sure you use delta time in any movement calculations!


}

/**
*   @brief   Renders the scene
*   @details Renders all the game objects to the current frame.
	         Once the current frame is has finished the buffers are
			 swapped accordingly and the image shown.
*   @return  void
*/
void AngryBirdsGame::render(const ASGE::GameTime &)
{
	renderer->setFont(0);
	
	if (in_menu)
	{
		renderer->renderSprite(*menu_layer.spriteComponent()->getSprite());
	}
	else
	{
		renderer->renderSprite(*background_layer.spriteComponent()->getSprite());
		if (in_tutorial)
		{
			renderer->renderText("HOW TO PLAY", 50, 100, ASGE::COLOURS::BLUE);
			renderer->renderText("Use the mouse to aim the rocks in the slingshot.", 60, 150, ASGE::COLOURS::BLACK);
			renderer->renderText("Try to hit the aliens using the rocks to win the level, complete it in the least number of rocks to earn the most points.", 60, 175, ASGE::COLOURS::BLACK);
		
			renderer->renderText("Press Space to continue", game_width/2 - 60, 500, ASGE::COLOURS::BLACK);

		}
		else
		{
			std::string score_str = "Score: " + std::to_string(score);
			renderer->renderText(score_str.c_str(), 50, 100, ASGE::COLOURS::BLUE);

			for (int i = 0; i < ammo_array_size; i++)
			{	
				renderer->renderSprite(*ammo[i].spriteComponent()->getSprite());

			}

			for (int i = 0; i < enemy_array_size; i++)
			{
				renderer->renderSprite(*enemies[i].spriteComponent()->getSprite());

			}
		}
	}
}

void AngryBirdsGame::setUpGameobjects()
{
	for (int i = 0; i < ammo_array_size; i++)
	{
		ammo[i].addSpriteComponent(renderer.get(), 
			".\\Resources\\Textures\\MyAssets\\angeryrock.png");
		
		float new_x_pos = (i * 44) + 100;

		ammo_sprite = ammo[i].spriteComponent()->getSprite();
		ammo_sprite->xPos(new_x_pos);
		ammo_sprite->yPos(825);
		ammo_sprite->width(48);
		ammo_sprite->height(48);
	}

	for (int i = 0; i < enemy_array_size; i++)
	{
		enemies[i].addSpriteComponent(renderer.get(),
			".\\Resources\\Textures\\kenney_physicspack\\PNG\\Aliens\\alienBlue_suit.png");

		float new_x_pos = (i * 60) + 1300;

		enemy_sprite = enemies[i].spriteComponent()->getSprite();
		enemy_sprite->xPos(new_x_pos);
		enemy_sprite->yPos(825);
		enemy_sprite->width(52);
		enemy_sprite->height(52);
	}

	setUpActive();
}

void AngryBirdsGame::setUpActive()
{

}
