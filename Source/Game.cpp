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

	if (!menu_layer.addSpriteComponent(renderer.get(), "Resources\\Textures\\MyAssets\\menu.jpg"))
	{
		return false;
	}

	setUpGameobjects();
	setUpActive();

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
	else
	{
		if (key->key == ASGE::KEYS::KEY_SPACE
			&& key->action == ASGE::KEYS::KEY_RELEASED)
		{
			if (game_state == -1)
			{
				game_state = 0;
				setUpGameobjects();
				setUpActive();
				number_of_enemies = 3;
				ammo_array_size = 5;
				score = 0;
			}
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

	if(click->action == 1)
	{
		if (x_pos >= active_ammo_sprite->xPos() - 50 &&
			x_pos <= (active_ammo_sprite->xPos() + active_ammo_sprite->width()))
		{
			if (y_pos >= active_ammo_sprite->yPos() - 50
				&& y_pos <= (active_ammo_sprite->yPos() + active_ammo_sprite->height()))
			{
				bird_grabbed = true;
			}
		}
	}
	else if(click->action == 0)
	{
		if (bird_grabbed)
		{
			velocity.x = start_pos.x - mouse_x_pos;
			velocity.y = start_pos.y - mouse_y_pos;
			bird_in_air = true;
		}
	}

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
	auto dt_sec = us.delta_time.count() / 1000.0;

	if (!in_menu)
	{
		//Bird Grabbed
		preLaunch(dt_sec);

		//Handles all collision
		collision();

		// Win/Lose State
		gameState();
	}
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

		if (game_state == 0)
		{
			if (in_tutorial)
			{
				howToPlayUI();
			}
			else if (!in_tutorial)
			{
				inGameUI();
			}
		}
		else if (game_state == 1)
		{
			winUI();
		}
		else if (game_state == -1)
		{
			loseUI();
		}
		else if (game_state == 0)
		{
			inGameUI();
		}


		//debug stuff
	/*	renderer->renderText(std::to_string(game_state), 1000, 50, ASGE::COLOURS::BLACK);
		renderer->renderText(std::to_string(velocity.x), 1000, 75, ASGE::COLOURS::BLACK);
		renderer->renderText(std::to_string(velocity.y), 1000, 100, ASGE::COLOURS::BLACK);*/
					
	}
}

//Setup Functions
void AngryBirdsGame::setUpGameobjects()
{
	//Ammo Set Up
	for (int i = 0; i < ammo_array_size; i++)
	{
		ammo[i].addSpriteComponent(renderer.get(), 
			".\\Resources\\Textures\\MyAssets\\angeryrock.png");
		
		float new_x_pos = (i * 12) + 10;

		ammo_sprite = ammo[i].spriteComponent()->getSprite();
		ammo_sprite->xPos(new_x_pos);
		ammo_sprite->yPos(825);
		ammo_sprite->width(48);
		ammo_sprite->height(48);
	}

	//Enemy Set Up
	for (int i = 0; i < enemy_array_size; i++)
	{
		enemies[i].addSpriteComponent(renderer.get(),
			".\\Resources\\Textures\\kenney_physicspack\\PNG\\Aliens\\alienBlue_suit.png");

		float new_x_pos = (i * 140) + 1300;
		float new_y_pos = (i * -150) + 835;

		enemy_sprite = enemies[i].spriteComponent()->getSprite();
		enemy_sprite->xPos(new_x_pos);
		enemy_sprite->yPos(new_y_pos);
		enemy_sprite->width(48);
		enemy_sprite->height(48);
	}

	//Slingshot Set Up
	slingshot.addSpriteComponent(renderer.get(),
		".\\Resources\\Textures\\MyAssets\\slingshot.png");
	slingshot.spriteComponent()->getSprite()->xPos(270);
	slingshot.spriteComponent()->getSprite()->yPos(700);

	//Lose Set Up
	lose_sprite.addSpriteComponent(renderer.get(),
		".\\Resources\\Textures\\MyAssets\\You Lose text.png");
	lose_sprite.spriteComponent()->getSprite()->xPos(100);
	lose_sprite.spriteComponent()->getSprite()->yPos(100);

	//Win Set Up
	win_sprite.addSpriteComponent(renderer.get(),
		".\\Resources\\Textures\\MyAssets\\You win text.png");
	win_sprite.spriteComponent()->getSprite()->xPos(100);
	win_sprite.spriteComponent()->getSprite()->yPos(100);
}

void AngryBirdsGame::setUpActive()
{
	active_ammo.addSpriteComponent(renderer.get(),
		".\\Resources\\Textures\\MyAssets\\angeryrock.png");

	active_ammo_sprite = active_ammo.spriteComponent()->getSprite();
	active_ammo_sprite->xPos(start_pos.x);
	active_ammo_sprite->yPos(start_pos.y);
	active_ammo_sprite->width(42);
	active_ammo_sprite->height(42);
}

//Gameplay Functions
void AngryBirdsGame::preLaunch(float dt_sec)
{
	if (bird_grabbed)
	{
		active_ammo_sprite->xPos(mouse_x_pos);
		active_ammo_sprite->yPos(mouse_y_pos);
	}


	if (bird_in_air)
	{
		fire(dt_sec);
	}
	//Check for slingshot range
	inputs->getCursorPos(mouse_x_pos, mouse_y_pos);
	distance = sqrt(pow(start_pos.x - mouse_x_pos, 2) + pow(start_pos.y - mouse_y_pos, 2));
	if (distance > 200)
	{
		bird_grabbed = false;
	}
}

void AngryBirdsGame::fire(float dt_sec)
{
	bird_grabbed = false;
	float x_pos = active_ammo_sprite->xPos();
	float y_pos = active_ammo_sprite->yPos();

	float direction_x = velocity.x *move_speed_x;
	float direction_y = velocity.y * move_speed_x;

	x_pos += direction_x * (dt_sec);
	y_pos += direction_y * (dt_sec);

	velocity.y -= gravity;
	active_ammo_sprite->xPos(x_pos);
	active_ammo_sprite->yPos(y_pos);
}

void AngryBirdsGame::collision()
{
	active_box = active_ammo.spriteComponent()->getBoundingBox();

	if (active_ammo_sprite->yPos() + active_ammo_sprite->height() > 950)
	{
		reload();
	}
	if (active_ammo_sprite->yPos() < 0)
	{
		reload();
	}
	if (active_ammo_sprite->xPos() < 0)
	{
		reload();
	}


	for (int i = 0; i <= enemy_array_size; i++)
	{

		enemy_box = enemies[i].spriteComponent()->getBoundingBox();
		if (active_box.isInside(enemy_box) && enemies[i].visibility == true)
		{
			enemies[i].visibility = false;
			score += 1000;
			number_of_enemies--;
			reload();
			break;
		}
	}
}

void AngryBirdsGame::reload()
{
	active_ammo_sprite->xPos(start_pos.x);
	active_ammo_sprite->yPos(start_pos.y);
	velocity.x = 0;
	velocity.y = 0;
	ammo_array_size--;
	move_speed_x = 5.0;
	move_speed_y = 5.0;
	bird_in_air = false;

}

void AngryBirdsGame::gameState()
{
	if (number_of_enemies == 0)
	{
		game_state = 1;
	}
	if (ammo_array_size == 0)
	{
		game_state = -1;
	}
	

}

//UI Functions
void AngryBirdsGame::howToPlayUI()
{
	renderer->renderText("HOW TO PLAY", 50, 100, ASGE::COLOURS::BLUE);
	renderer->renderText("Use the mouse to aim the rocks in the slingshot.", 60, 150, ASGE::COLOURS::BLACK);
	renderer->renderText("Try to hit the aliens using the rocks to win the level, complete it in the least number of rocks to earn the most points.", 60, 175, ASGE::COLOURS::BLACK);

	renderer->renderText("Press Space to continue", game_width / 2 - 60, 500, ASGE::COLOURS::BLACK);
}

void AngryBirdsGame::inGameUI()
{
	std::string score_str = "Score: " + std::to_string(score);
	renderer->renderText(score_str.c_str(), 50, 100, ASGE::COLOURS::BLUE);
	if (active_ammo.visibility == true)
	{
		renderer->renderSprite(*active_ammo.spriteComponent()->getSprite());
	}

	renderer->renderSprite(*slingshot.spriteComponent()->getSprite());

	for (int i = 0; i < ammo_array_size; i++)
	{
		renderer->renderSprite(*ammo[i].spriteComponent()->getSprite());
	}

	for (int i = 0; i < enemy_array_size; i++)
	{
		if (enemies[i].visibility == true)
		{
			renderer->renderSprite(*enemies[i].spriteComponent()->getSprite());
		}
	}
}

void AngryBirdsGame::winUI()
{
	renderer->renderSprite(*win_sprite.spriteComponent()->getSprite());
}

void AngryBirdsGame::loseUI()
{
	renderer->renderSprite(*lose_sprite.spriteComponent()->getSprite());
	renderer->renderText("Press SPACE to restart", 100, 200, ASGE::COLOURS::BLACK);
}

