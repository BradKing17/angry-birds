#pragma once
#include <string>
#include <Engine/OGLGame.h>

#include "GameObject.h"
#include "Rect.h"


/**
*  An OpenGL Game based on ASGE.
*/
class AngryBirdsGame :
	public ASGE::OGLGame
{
public:
	AngryBirdsGame();
	~AngryBirdsGame();
	virtual bool init() override;
	

private:
	//Set up Functions
	void keyHandler(const ASGE::SharedEventData data);
	void clickHandler(const ASGE::SharedEventData data);
	void setupResolution();
	bool loadBackgrounds();
	void setUpGameobjects();
	void setUpActive();

	//Game Functions
	void fire(float dt_sec);
	void collision();
	void reload();

	//UI Functions
	void howToPlayUI();
	void inGameUI();
	void winUI();
	void loseUI();
	

	virtual void update(const ASGE::GameTime &) override;
	virtual void render(const ASGE::GameTime &) override;

	int  key_callback_id = -1;	        /**< Key Input Callback ID. */
	int  mouse_callback_id = -1;        /**< Mouse Input Callback ID. */


	bool in_menu = true;
	bool in_tutorial = false;
	bool bird_grabbed = false;
	bool bird_in_air = false;

	int score = 0;
	vector2 start_pos = { 300, 700 };
	double distance;
	vector2 velocity = { 0,0 };
	float move_speed_x = 5.0;
	float move_speed_y = 5.0;
	float gravity = 0.005;
	int game_state = 0;

	//Add your GameObjects
	GameObject background_layer;
	GameObject menu_layer;

	GameObject enemies[3];
	int enemy_array_size = 3;
	ASGE::Sprite* enemy_sprite = nullptr;
	rect enemy_box;
	int number_of_enemies = enemy_array_size;
	
	
	GameObject ammo[5];
	int ammo_array_size = 5;
	ASGE::Sprite* ammo_sprite = nullptr;

	GameObject active_ammo;
	ASGE::Sprite* active_ammo_sprite = nullptr;
	rect active_box;

	double mouse_x_pos, mouse_y_pos;

};