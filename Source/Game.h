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
	void keyHandler(const ASGE::SharedEventData data);
	void clickHandler(const ASGE::SharedEventData data);
	void setupResolution();
	bool loadBackgrounds();
	void setUpGameobjects();
	void setUpActive();

	virtual void update(const ASGE::GameTime &) override;
	virtual void render(const ASGE::GameTime &) override;

	int  key_callback_id = -1;	        /**< Key Input Callback ID. */
	int  mouse_callback_id = -1;        /**< Mouse Input Callback ID. */


	bool in_menu = true;
	bool in_tutorial = false;

	int score = 0;
	int enemy_array_size = 3;
	int ammo_array_size = 5;
	vector2 start_pos = { 200, 810 };

	//Add your GameObjects
	GameObject background_layer;
	GameObject menu_layer;

	GameObject enemies[3];
	ASGE::Sprite* enemy_sprite = nullptr;
	
	GameObject ammo[5];
	ASGE::Sprite* ammo_sprite = nullptr;

	GameObject active_ammo;
	ASGE::Sprite* active_ammo_sprite = nullptr;


};