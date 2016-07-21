#include <SDL.h>
#include <SDL_image.h>
//#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>

//SCREEN
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 640;

//LEVEL
const int LEVEL_WIDTH = 1850;
const int LEVEL_HEIGHT = 1150;

const int BLOCK_WIDTH = 50;
const int BLOCK_HEIGHT = 50;
const int TOTAL_BLOCKS = 851;
const int TOTAL_BLOCK_SPRITES = 3;

int BLOCK_EMPTY = 0;
int BLOCK_BRICK = 1;
const int BLOCK_WALL = 2;

Uint32 callback(Uint32 interval, void* param);

class LTexture
{
public:

	LTexture();

	~LTexture();

	bool loadFromFile(std::string path);

#ifdef _SDL_TTF_H

	bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
#endif

	void free();

	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	void setBlendMode(SDL_BlendMode blending);

	void setAlpha(Uint8 alpha);

	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

	int getWidth();
	int getHeight();

private:

	SDL_Texture* mTexture;

	int mWidth;
	int mHeight;
};

class Block
{
public:

	Block(int x, int y, int blockType);

	void render(SDL_Rect& camera);

	int getType();

	void setType(int t);

	int getX();

	int getY();

	SDL_Rect getBox();

private:

	SDL_Rect mBox;

	int mType;
};

class CPlayer2
{
public:

	static const int PLAYER2_WIDTH = 44;
	static const int PLAYER2_HEIGHT = 44;

	static const int PLAYER2_VEL = 5;

	CPlayer2();

	void handleEvent(SDL_Event& e);

	void move(Block *blocks[]);

	void setCamera(SDL_Rect& camera);

	void render(SDL_Rect& camera);

	void animation(SDL_Rect &camera, int &f);

	int getX();
	int getY();

private:

	SDL_Rect mBox;

	int mVelX, mVelY;
};
/*
class Particle
{
public:
	Particle(int x, int y);
	~Particle(){
	}

	void render();

	bool isDead();

public:
	int mPosX, mPosY;

	int mFrame;

	LTexture *mTexture;
};

Particle::Particle(int x, int y)
{
	mPosX = x;
	mPosY = y;

	mFrame = 0;
}

bool Particle::isDead()
{
	return mFrame > 10;
}

void Particle::render()
{
	mTexture->render(mPosX, mPosY);
	mFrame++;
}
*/
LTexture gDeadPlayerTexture;

class CPlayer
{
public:

	static const int PLAYER_WIDTH = 44;
	static const int PLAYER_HEIGHT = 44;

	static const int PLAYER_VEL = 5;

	CPlayer();

	~CPlayer(){
	}

	void handleEvent(SDL_Event& e);

	void move(Block *blocks[]);

	void setCamera(SDL_Rect& camera);

	void render(SDL_Rect& camera);

	void renderDead(SDL_Rect& camera);

	void animation(SDL_Rect &camera, int &f);

	int getX();
	int getY();

	SDL_Rect getBox();
private:

	SDL_Rect mBox;

	int mVelX, mVelY;
};



const int WALKING_ANIMATION_FRAMES2 = 8;
SDL_Rect gPlayer2[WALKING_ANIMATION_FRAMES2];
LTexture gPlayer2SheetTexture;


SDL_Rect gBomb[4];
LTexture gBombSheetTexture;

LTexture gExpTexture;

LTexture gBlockTexture;
SDL_Rect gBlockClips[TOTAL_BLOCK_SPRITES];

LTexture gDeadPlayerSheetTexture;
SDL_Rect gDeadPlayer[4];



class CBomb
{
public:
	//The dimensions of the CPlayer
	static const int BOMB_WIDTH = 50;
	static const int BOMB_HEIGHT = 50;

	//Initializes the variables

	CBomb();

	void SetBomb(int x, int y);

	//Moves the CPlayer and check collision against Blocks
	void explode(Block *blocks[]);

	void animation(SDL_Rect  &camera, int &f);

	int getX();
	int getY();

	SDL_Rect getBox();

	//Shows the CPlayer on the screen
	void render(SDL_Rect& camera);

private:
	//Collision box of the CPlayer
	SDL_Rect mBox;
};


CBomb::CBomb()
{
	//Initialize the collision box
	mBox.w = BOMB_WIDTH;
	mBox.h = BOMB_HEIGHT;
}

void CBomb::SetBomb(/*CPlayer& a*/ int x, int y)
{
	mBox.x = x;
	mBox.y = y;
}

int CBomb::getX()
{
	return mBox.x;
}

int CBomb::getY()
{
	return mBox.y;
}


void CBomb::animation(SDL_Rect &camera, int &f)
{
	SDL_Rect* currentClip = &gBomb[f / 4];
	gBombSheetTexture.render(mBox.x - camera.x, mBox.y - camera.y, currentClip);


	//Go to next frame

	++f;

	//Cycle animation
	if (f / 4 >= 8)
	{
		f = 0;
	}
}

SDL_Rect CBomb::getBox()
{
	return mBox;
}

void CBomb::render(SDL_Rect& camera)
{
	//Show the CPlayer
	//gBombTexture.render(mBox.x - camera.x, mBox.y - camera.y);
}

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia(Block* blocks[]);

//Frees media and shuts down SDL
void close(Block* blocks[]);

//Box collision detector
bool checkCollision(SDL_Rect a, SDL_Rect b);

bool BcheckCollision(SDL_Rect a, SDL_Rect b);

//Checks collision box against set of Blocks
bool touchesWall(SDL_Rect box, Block* blocks[]);

bool setBlocks(Block *blocks[]);

Uint32 callback(Uint32 interval, void* param);

SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer = NULL;

//Scene textures




LTexture::LTexture()
{
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	free();
}

bool LTexture::loadFromFile(std::string path)
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

#ifdef _SDL_TTF_H
bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor)
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
	if (textSurface != NULL)
	{
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	}
	else
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}


	//Return success
	return mTexture != NULL;
}
#endif

void LTexture::free()
{
	//Free texture if it exists
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}


Block::Block(int x, int y, int BlockType)
{
	//Get the offsets
	mBox.x = x;
	mBox.y = y;

	//Set the collision box
	mBox.w = BLOCK_WIDTH;
	mBox.h = BLOCK_HEIGHT;

	//Get the Block type
	mType = BlockType;
}

void Block::render(SDL_Rect& camera)
{
	//If the Block is on screen
	if (checkCollision(camera, mBox))
	{
		//Show the Block
		gBlockTexture.render(mBox.x - camera.x, mBox.y - camera.y, &gBlockClips[mType]);
	}
}

int Block::getType()
{
	return mType;
}

void Block::setType(int t)
{
	mType = t;
}

SDL_Rect Block::getBox()
{
	return mBox;
}

int Block::getX()
{
	return mBox.x;
}

int Block::getY()
{
	return mBox.y;
}

const int WALKING_ANIMATION_FRAMES = 8;
SDL_Rect gPlayer[WALKING_ANIMATION_FRAMES];
LTexture gPlayerSheetTexture;

CPlayer2::CPlayer2()
{
	//Initialize the collision box
	mBox.x = 150;
	mBox.y = 50;
	mBox.w = PLAYER2_WIDTH;
	mBox.h = PLAYER2_HEIGHT;

	//Initialize the velocity
	mVelX = 0;
	mVelY = 0;
}

void CPlayer2::handleEvent(SDL_Event& e)
{

	//If a key was pressed
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: mVelY -= PLAYER2_VEL; break;
		case SDLK_DOWN: mVelY += PLAYER2_VEL; break;
		case SDLK_LEFT: mVelX -= PLAYER2_VEL; break;
		case SDLK_RIGHT: mVelX += PLAYER2_VEL; break;
		}
	}
	//If a key was released
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: mVelY += PLAYER2_VEL; break;
		case SDLK_DOWN: mVelY -= PLAYER2_VEL; break;
		case SDLK_LEFT: mVelX += PLAYER2_VEL; break;
		case SDLK_RIGHT: mVelX -= PLAYER2_VEL; break;
		}
	}
}

void CPlayer2::move(Block *blocks[])
{
	//Move the CPlayer left or right
	mBox.x += mVelX;

	//If the CPlayer went too far to the left or right or touched a wall
	if ((mBox.x < 0) || (mBox.x + PLAYER2_WIDTH > LEVEL_WIDTH) || touchesWall(mBox, blocks))
	{
		//move back
		mBox.x -= mVelX;
	}

	//Move the CPlayer up or down
	mBox.y += mVelY;

	//If the CPlayer went too far up or down or touched a wall
	if ((mBox.y < 0) || (mBox.y + PLAYER2_HEIGHT > LEVEL_HEIGHT) || touchesWall(mBox, blocks))
	{
		//move back
		mBox.y -= mVelY;
	}
}

void CPlayer2::animation(SDL_Rect &camera, int &f)
{
	SDL_Rect* currentClip = &gPlayer2[f / 4];
	gPlayer2SheetTexture.render(mBox.x - camera.x, mBox.y - camera.y, currentClip);

	//Update screen

	//Go to next frame

	++f;

	//Cycle animation
	if (f / 4 >= 8)
	{
		f = 0;
	}
}

void CPlayer2::setCamera(SDL_Rect& camera)
{
	//Center the camera over the CPlayer
	camera.x = (mBox.x + PLAYER2_WIDTH / 2) - SCREEN_WIDTH / 2;
	camera.y = (mBox.y + PLAYER2_HEIGHT / 2) - SCREEN_HEIGHT / 2;

	//Keep the camera in bounds
	if (camera.x < 0)
	{
		camera.x = 0;
	}
	if (camera.y < 0)
	{
		camera.y = 0;
	}
	if (camera.x > LEVEL_WIDTH - camera.w)
	{
		camera.x = LEVEL_WIDTH - camera.w;
	}
	if (camera.y > LEVEL_HEIGHT - camera.h)
	{
		camera.y = LEVEL_HEIGHT - camera.h;
	}
}

void CPlayer2::render(SDL_Rect& camera)
{
	//gPlayerSheetTexture.render(mBox.x - camera.x, mBox.y - camera.y);
	//Show the CPlayer
	//gPlayerTexture.render(mBox.x - camera.x, mBox.y - camera.y);
}

int CPlayer2::getX()
{
	return mBox.x;
}

int CPlayer2::getY()
{
	return mBox.y;
}


CPlayer::CPlayer()
{
	//Initialize the collision box
	mBox.x = 50;
	mBox.y = 50;
	mBox.w = PLAYER_WIDTH;
	mBox.h = PLAYER_HEIGHT;
	
	//Initialize the velocity
	mVelX = 0;
	mVelY = 0;
}



void CPlayer::handleEvent(SDL_Event& e)
{

	//If a key was pressed
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_w: mVelY -= PLAYER_VEL; break;
		case SDLK_s: mVelY += PLAYER_VEL; break;
		case SDLK_a: mVelX -= PLAYER_VEL; break;
		case SDLK_d: mVelX += PLAYER_VEL; break;
		}
	}
	//If a key was released
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_w: mVelY += PLAYER_VEL; break;
		case SDLK_s: mVelY -= PLAYER_VEL; break;
		case SDLK_a: mVelX += PLAYER_VEL; break;
		case SDLK_d: mVelX -= PLAYER_VEL; break;
		}
	}
}

void CPlayer::move(Block *blocks[])
{
	//Move the CPlayer left or right
	mBox.x += mVelX;

	//If the CPlayer went too far to the left or right or touched a wall
	if ((mBox.x < 0) || (mBox.x + PLAYER_WIDTH > LEVEL_WIDTH) || touchesWall(mBox, blocks))
	{
		//move back
		mBox.x -= mVelX;
	}

	//Move the CPlayer up or down
	mBox.y += mVelY;

	//If the CPlayer went too far up or down or touched a wall
	if ((mBox.y < 0) || (mBox.y + PLAYER_HEIGHT > LEVEL_HEIGHT) || touchesWall(mBox, blocks))
	{
		//move back
		mBox.y -= mVelY;
	}
}

void CPlayer::renderDead(SDL_Rect &camera)
{
	gDeadPlayerTexture.render(mBox.x - camera.x, mBox.y - camera.y);
}

void CPlayer::animation(SDL_Rect &camera, int &f)
{
	SDL_Rect* currentClip = &gPlayer[f / 4];
	gPlayerSheetTexture.render(mBox.x - camera.x, mBox.y - camera.y, currentClip);

	//Update screen

	//Go to next frame

	++f;

	//Cycle animation
	if (f / 4 >= 8)
	{
		f = 0;
	}
}

void CPlayer::setCamera(SDL_Rect& camera)
{
	//Center the camera over the CPlayer
	camera.x = (mBox.x + PLAYER_WIDTH / 2) - SCREEN_WIDTH / 2;
	camera.y = (mBox.y + PLAYER_HEIGHT / 2) - SCREEN_HEIGHT / 2;

	//Keep the camera in bounds
	if (camera.x < 0)
	{
		camera.x = 0;
	}
	if (camera.y < 0)
	{
		camera.y = 0;
	}
	if (camera.x > LEVEL_WIDTH - camera.w)
	{
		camera.x = LEVEL_WIDTH - camera.w;
	}
	if (camera.y > LEVEL_HEIGHT - camera.h)
	{
		camera.y = LEVEL_HEIGHT - camera.h;
	}
}

void CPlayer::render(SDL_Rect& camera)
{
	//gPlayerSheetTexture.render(mBox.x - camera.x, mBox.y - camera.y);
	//Show the CPlayer
	//gPlayerTexture.render(mBox.x - camera.x, mBox.y - camera.y);
}

int CPlayer::getX()
{
	return mBox.x;
}

int CPlayer::getY()
{
	return mBox.y;
}

SDL_Rect CPlayer::getBox()
{
	return mBox;
}

class LTimer
{
public:
	//Initializes variables
	LTimer();

	//The various clock actions
	void start();
	void stop();
	void pause();
	void unpause();

	//Gets the timer's time
	Uint32 getTicks();

	//Checks the status of the timer
	bool isStarted();
	bool isPaused();

private:
	//The clock time when the timer started
	Uint32 mStartTicks;

	//The ticks stored when the timer was paused
	Uint32 mPausedTicks;

	//The timer status
	bool mPaused;
	bool mStarted;
};

LTimer::LTimer()
{
	//Initialize the variables
	mStartTicks = 0;
	mPausedTicks = 0;

	mPaused = false;
	mStarted = false;
}



void LTimer::start()
{
	//Start the timer
	mStarted = true;

	//Unpause the timer
	mPaused = false;

	//Get the current clock time
	mStartTicks = SDL_GetTicks();
	mPausedTicks = 0;
}

void LTimer::stop()
{
	//Stop the timer
	mStarted = false;

	//Unpause the timer
	mPaused = false;

	//Clear tick variables
	mStartTicks = 0;
	mPausedTicks = 0;
}

void LTimer::pause()
{
	//If the timer is running and isn't already paused
	if (mStarted && !mPaused)
	{
		//Pause the timer
		mPaused = true;

		//Calculate the paused ticks
		mPausedTicks = SDL_GetTicks() - mStartTicks;
		mStartTicks = 0;
	}
}

void LTimer::unpause()
{
	//If the timer is running and paused
	if (mStarted && mPaused)
	{
		//Unpause the timer
		mPaused = false;

		//Reset the starting ticks
		mStartTicks = SDL_GetTicks() - mPausedTicks;

		//Reset the paused ticks
		mPausedTicks = 0;
	}
}

Uint32 LTimer::getTicks()
{
	//The actual timer time
	Uint32 time = 0;

	//If the timer is running
	if (mStarted)
	{
		//If the timer is paused
		if (mPaused)
		{
			//Return the number of ticks when the timer was paused
			time = mPausedTicks;
		}
		else
		{
			//Return the current time minus the start time
			time = SDL_GetTicks() - mStartTicks;
		}
	}

	return time;
}

bool LTimer::isStarted()
{
	//Timer is running and paused or unpaused
	return mStarted;
}

bool LTimer::isPaused()
{
	//Timer is running and paused
	return mPaused && mStarted;
}

class CExplosion
{

public:
	CExplosion(){};

	void render();
	void SetExplosion(SDL_Rect center);
	void SetMBox();

	SDL_Rect getMBox(int i);
	SDL_Rect getMBoxES();

	void setShape(int what, SDL_Rect S);
	
	void setTimer();
	void stopTimer();

	int getTimer();

	//void render(SDL_Rect& camera, Block *blocks[]);
	void render(SDL_Rect& camera, SDL_Rect box);

public:
	SDL_Rect mBox[9];
			  //8
			  //4
	  //7 /3/ /0/ /1/ /5/
	          //2
	          //6

	LTimer timer;
};


void CExplosion::SetExplosion(SDL_Rect center)
{
	mBox[0] = center;
}

void CExplosion::SetMBox()
{	
	for (int i = 1; i < 9; i++)
	{
		mBox[i] = mBox[0];
	}
	mBox[1].x = mBox[0].x + 50;
	mBox[2].y = mBox[0].y + 50;
	mBox[3].x = mBox[0].x - 50;
	mBox[4].y = mBox[0].y - 50;
	mBox[5].x = mBox[0].x + 100;
	mBox[6].y = mBox[0].y + 100;
	mBox[7].x = mBox[0].x - 100;
	mBox[8].y = mBox[0].y - 100;
}

SDL_Rect CExplosion::getMBox(int i)
{
	return mBox[i];
}

void CExplosion::setShape(int what, SDL_Rect S)
{
	mBox[what] = S;
}

/////////////////////////

void CExplosion::setTimer()
{ 
	timer.start();
}

void CExplosion::stopTimer()
{
	timer.stop();
}

int CExplosion::getTimer()
{
	return timer.getTicks();
}

/////////////////////////


/*
void CExplosion::render(SDL_Rect& camera, SDL_Rect box)
{
	for (int i = 0; i < 9; i++)
	{	
			gExpTexture.render(mBox[i].x - camera.x, mBox[i].y - camera.y);
	}
}
*/

void CExplosion::render(SDL_Rect& camera, SDL_Rect box)
{
		gExpTexture.render(box.x - camera.x, box.y - camera.y);
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("BOMBERMAN", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia(Block* blocks[])
{
	bool success = true;

	/*if (!gBombTexture.loadFromFile("textures/Bomb.bmp"))
	{
		printf("Failed to load CPlayer texture!\n");
		success = false;
	}
	*/
	if (!gBombSheetTexture.loadFromFile("textures/Bomba.bmp"))
	{
	printf("Failed to load CPlayer texture!\n");
	success = false;
	}
	else
	{
	for (int i = 0; i < 4; i++)
	{
	gBomb[i].x = i * 50;
	gBomb[i].y = 0;
	gBomb[i].w = 50;
	gBomb[i].h = 50;
	}
	}
	

	if (!gBlockTexture.loadFromFile("textures/plytki.png"))
	{
		printf("Failed to load Block set texture!\n");
		success = false;
	}
	if (!gExpTexture.loadFromFile("textures/exp2.bmp"))
	{
		printf("Failed to load Block set texture!\n");
		success = false;
	}
	//Load Block map
	if (!setBlocks(blocks))
	{
		printf("Failed to load Block set!\n");
		success = false;
	}
	if (!gPlayer2SheetTexture.loadFromFile("textures/PlayerC.bmp"))
	{
		printf("Failed to load Block set texture!\n");
		success = false;
	}
	else
	{
		for (int i = 0; i < 8; i++)
		{
			gPlayer2[i].x = i * 46;
			gPlayer2[i].y = 0;
			gPlayer2[i].w = 45;
			gPlayer2[i].h = 45;
		}

	}
	if (!gPlayerSheetTexture.loadFromFile("textures/PlayerB.bmp"))
	{
		printf("Failed to load Block set texture!\n");
		success = false;
	}
	else
	{
		for (int i = 0; i < 8; i++)
		{
			gPlayer[i].x = i * 46;
			gPlayer[i].y = 0;
			gPlayer[i].w = 45;
			gPlayer[i].h = 45;
		}

	}
	if (!gDeadPlayerTexture.loadFromFile("textures/Dead.bmp"))
	{
		printf("Failed to load Block set texture!\n");
		success = false;
	}
	return success;
}

void close(Block* blocks[])
{
	//Deallocate Blocks
	for (int i = 0; i < TOTAL_BLOCKS; ++i)
	{
		if (blocks[i] == NULL)
		{
			delete blocks[i];
			blocks[i] = NULL;
		}
	}

	//Free loaded images
	//gPlayerTexture.free();
	gBlockTexture.free();
	//gBombTexture.free();
	gPlayer2SheetTexture.free();
	gPlayerSheetTexture.free();
	gDeadPlayerTexture.free();
	gExpTexture.free();
	gBombSheetTexture.free();

	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

bool checkCollision(SDL_Rect a, SDL_Rect b)
{
	//The sides of the rectangles
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	//Calculate the sides of rect A
	leftA = a.x;
	rightA = a.x + a.w;
	topA = a.y;
	bottomA = a.y + a.h;

	//Calculate the sides of rect B
	leftB = b.x;
	rightB = b.x + b.w;
	topB = b.y;
	bottomB = b.y + b.h;

	//If any of the sides from A are outside of B
	if (bottomA <= topB)
	{
		return false;
	}

	if (topA >= bottomB)
	{
		return false;
	}

	if (rightA <= leftB)
	{
		return false;
	}

	if (leftA >= rightB)
	{
		return false;
	}

	//If none of the sides from A are outside B
	return true;
}

bool BcheckCollision(SDL_Rect a, SDL_Rect b)
{
	//The sides of the rectangles
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	//Calculate the sides of rect A
	leftA = a.x;
	rightA = a.x + a.w;
	topA = a.y;
	bottomA = a.y + a.h;

	//Calculate the sides of rect B
	leftB = b.x;
	rightB = b.x + b.w;
	topB = b.y;
	bottomB = b.y + b.h;

	//If any of the sides from A are outside of B
	if (bottomA >= topB)
	{
		return false;
	}

	if (topA <= bottomB)
	{
		return false;
	}

	if (rightA >= leftB)
	{
		return false;
	}

	if (leftA <= rightB)
	{
		return false;
	}

	//If none of the sides from A are outside B
	return true;
}

bool setBlocks(Block* blocks[])
{
	bool blocksLoaded = true;

	int x = 0, y = 0;

	std::fstream map;
	map.open("lazy3.map", std::ios::in);

	if (map.is_open() == false)
	{
		printf("Unable to load map file!\n");
		blocksLoaded = false;
	}
	else
	{
		for (int i = 0; i < TOTAL_BLOCKS; ++i)
		{
			int blockType = -1;

			map >> blockType;

			if (map.fail())
			{
				printf("Error loading map: Unexpected end of file!\n");
				blocksLoaded = false;
				break;
			}

			if ((blockType >= 0) && (blockType < TOTAL_BLOCK_SPRITES))
			{
				blocks[i] = new Block(x, y, blockType);
			}
			else
			{
				printf("Error loading map: Invalid Block type at %d!\n", i);
				blocksLoaded = false;
				break;
			}

			x += BLOCK_WIDTH;

			//If we've gone too far
			if (x >= LEVEL_WIDTH)
			{
				//Move back
				x = 0;

				//Move to the next row
				y += BLOCK_HEIGHT;
			}
		}

		//Clip the sprite sheet
		if (blocksLoaded)
		{
			gBlockClips[BLOCK_EMPTY].x = 0;
			gBlockClips[BLOCK_EMPTY].y = 0;
			gBlockClips[BLOCK_EMPTY].w = BLOCK_WIDTH;
			gBlockClips[BLOCK_EMPTY].h = BLOCK_HEIGHT;

			gBlockClips[BLOCK_BRICK].x = 50;
			gBlockClips[BLOCK_BRICK].y = 0;
			gBlockClips[BLOCK_BRICK].w = BLOCK_WIDTH;
			gBlockClips[BLOCK_BRICK].h = BLOCK_HEIGHT;

			gBlockClips[BLOCK_WALL].x = 100;
			gBlockClips[BLOCK_WALL].y = 0;
			gBlockClips[BLOCK_WALL].w = BLOCK_WIDTH;
			gBlockClips[BLOCK_WALL].h = BLOCK_HEIGHT;
		}
	}

	map.close();

	//If the map was loaded fine
	return blocksLoaded;
}


bool touchesWall(SDL_Rect box, Block* blocks[])            
{
	if (box.h == 50)
	{
		for (int i = 0; i < TOTAL_BLOCKS; i++)
		{
			//If the Block is a wall type Block
			if ((blocks[i]->getType() == BLOCK_BRICK)) //&& ( Blocks[ i ]->getType() <= Block_WALL ) )
			{
				//If the collision box touches the brick Block
				if (checkCollision(box, blocks[i]->getBox()))
				{
					blocks[i]->setType(0);
					return true;
				}
			}
			else if (blocks[i]->getType() == BLOCK_WALL)
			{
				if (checkCollision(box, blocks[i]->getBox()))
				{
					//Blocks[i]->setType(0);
					return true;
				}
			}
			
		}
	}
	else
	{
		for (int i = 0; i < TOTAL_BLOCKS; i++)
		{
			//If the Block is a wall type Block
			if ((blocks[i]->getType() >= BLOCK_BRICK) && (blocks[i]->getType() <= BLOCK_WALL))
			{
				//If the collision box touches the wall Block
				if (checkCollision(box, blocks[i]->getBox()))
				{
					return true;
				}
			}
		}
	}
	//If no wall Blocks were touched
	return false;
}


//std::vector<CExplosion*> ep;
//CExplosion *E;
std::vector<CBomb*> bombs;

std::vector<CExplosion*> epl;

//CExplosion *E2;
std::vector<CBomb*> bombs2;
//std::vector<CExplosion*> exp;


/*
void Boom(CExplosion &E, Block *blocks[])
{
	for (int j = 0; j < 9; j++)
	{
		for (int i = 0; i < TOTAL_BLOCKS; i++)
		{
			if (((checkCollision(E.mBox[1], blocks[i]->getBox())) && (blocks[i]->getType() == BLOCK_WALL)))
			{
				//E.mBox[5] = E.mBox[0];
				E.setShape(1, E.mBox[0]);
				E.setShape(5, E.mBox[0]);
			}
			if (checkCollision(E.mBox[2], blocks[i]->getBox()) && blocks[i]->getType() == BLOCK_WALL)
			{
				//E.mBox[6] = E.mBox[0];
				E.setShape(2, E.mBox[0]);
				E.setShape(6, E.mBox[0]);
			}
			if (checkCollision(E.mBox[3], blocks[i]->getBox()) && blocks[i]->getType() == BLOCK_WALL)
			{
				//E.mBox[7] = E.mBox[0];
				E.setShape(3, E.mBox[0]);
				E.setShape(7, E.mBox[0]);
			}
			if (checkCollision(E.mBox[4], blocks[i]->getBox()) && blocks[i]->getType() == BLOCK_WALL)
			{
				//E.mBox[8] = E.mBox[0];
				E.setShape(4, E.mBox[0]);
				E.setShape(8, E.mBox[0]);
			}
			else
			{
				touchesWall(E.mBox[j], blocks);
			}
		}
	}
	//return false;
}
*/

void Boom(CExplosion &E, Block *blocks[])
{
	for (int j = 0; j < 9; j++)
	{
		for (int i = 0; i < TOTAL_BLOCKS; i++)
		{
			if (((checkCollision(E.mBox[1], blocks[i]->getBox())) && (blocks[i]->getType() == BLOCK_WALL)))
			{
				//E.mBox[5] = E.mBox[0];
				E.setShape(1, E.mBox[0]);
				E.setShape(5, E.mBox[0]);
			}
			if (checkCollision(E.mBox[2], blocks[i]->getBox()) && blocks[i]->getType() == BLOCK_WALL)
			{
				//E.mBox[6] = E.mBox[0];
				E.setShape(2, E.mBox[0]);
				E.setShape(6, E.mBox[0]);
			}
			if (checkCollision(E.mBox[3], blocks[i]->getBox()) && blocks[i]->getType() == BLOCK_WALL)
			{
				//E.mBox[7] = E.mBox[0];
				E.setShape(3, E.mBox[0]);
				E.setShape(7, E.mBox[0]);
			}
			if (checkCollision(E.mBox[4], blocks[i]->getBox()) && blocks[i]->getType() == BLOCK_WALL)
			{
				//E.mBox[8] = E.mBox[0];
				E.setShape(4, E.mBox[0]);
				E.setShape(8, E.mBox[0]);
			}
			if (checkCollision(E.mBox[j], blocks[i]->getBox()) && blocks[i]->getType() == BLOCK_WALL)
			{
				//E.mBox[8] = E.mBox[0];
				E.setShape(j, E.mBox[0]);
			}
			else
			{
				touchesWall(E.mBox[j], blocks);
			}
		}
	}
	//return false;
}


void explosion(/*std::vector<CExplosion*> epl*/ CExplosion &E, Block *blocks[])
{
	SDL_Rect xxx = { 0, 0, LEVEL_WIDTH , LEVEL_HEIGHT };
	//for (int e = 0; e < epl.size(); e++ )
	//{

		for (int i = 0; i < TOTAL_BLOCKS; i++)
		{
			
			for (std::vector<CBomb*>::iterator itr = bombs.begin(); itr != bombs.end();)
			{
					if (checkCollision(xxx, (*itr)->getBox()))
					{
						delete *itr;
						itr = bombs.erase(itr);
						Boom(E, blocks);
						//Boom(*epl[e], blocks);
					}
					else
					{
						++itr;
					}
				}

		//}
	}
}

void dead(CPlayer player, CExplosion *E, SDL_Rect camera)
{
	for (int i = 0; i < 9; i++)
	{
		if (BcheckCollision(player.getBox(), E->mBox[i]))
		{
			player.renderDead(camera);
		}
		else
		{
		
		}
	}

}

void RenderLevel(SDL_Rect camera, Block *blockSet[])
{
	for (int i = 0; i < TOTAL_BLOCKS; ++i)
	{
		blockSet[i]->render(camera);
	}
}

void PlayerOneBombsRender(SDL_Rect camera, int frameBomb[])
{
	for (int i = 0; i < 5; i++)
	{
		for (std::vector<CBomb*>::iterator itr = bombs.begin(); itr != bombs.end(); ++itr)
		{
			(*itr)->animation(camera, frameBomb[i]);
			(*itr)->render(camera);
		}
	}
}

void PlayerTwoBombsRender(SDL_Rect camera, int frameBomb2[])
{
	for (int i = 0; i < 5; i++)
	{
		for (std::vector<CBomb*>::iterator itr = bombs2.begin(); itr != bombs2.end(); ++itr)
		{
			(*itr)->animation(camera, frameBomb2[i]);
			(*itr)->render(camera);
		}
	}
}

int main(int argc, char* args[] )
{
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//The level Blocks
		Block* blockSet[TOTAL_BLOCKS];

		//Load media
		if (!loadMedia(blockSet))
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;
			SDL_Event e2;

			//LTimer timer[5];
			LTimer timer2;
			//LTimer timer;

			int framePlayer2 = 0;
			int framePlayer = 0;


			int frameBomb[5];
			for (int i = 0; i < 5; i++)
			{
				frameBomb[i] = 0;
			}

			int frameBomb2[5];
			for (int i = 0; i < 5; i++)
			{
				frameBomb2[i] = 0;
			}

			//Level camera
			SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
			SDL_Rect yyy = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

			//The CPlayer that will be moving around on the screen
			CPlayer player2;
			CPlayer player;

			
			//SDL_TimerID timerID = SDL_AddTimer(8 * 1000, timer.callback, "something");

			int Xb2 = 0, Yb2 = 0;
			int Xb = 0, Yb = 0;
			//While application is running
			while (!quit)
			{
				
				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{

					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}

					if ((e.type == SDL_KEYDOWN && e.key.repeat == 0))
					{

						switch (e.key.keysym.sym)
						{
						case SDLK_SPACE:
						{
							//CExplosion *E;
							//timer.start();
							for (int i = 0; i < TOTAL_BLOCKS; i++)
							{
								Xb = player.getX() + 25;
								Yb = player.getY() + 25;
								if (Xb >= blockSet[i]->getX() && Xb <= blockSet[i]->getX() + 50 && Yb >= blockSet[i]->getY() && Yb <= blockSet[i]->getY() + 50)
								{
									Xb = blockSet[i]->getX();
									Yb = blockSet[i]->getY();
									CBomb* bomba1 = new CBomb;
									if (bombs.size() < 5)
									{
										bombs.push_back(bomba1);
									}
									bomba1->SetBomb(Xb, Yb);
									CExplosion *E = new CExplosion;
									epl.push_back(E);
									E->SetExplosion(bomba1->getBox());
									E->SetMBox();
									E->setTimer();  //timer start
								}
								
							}
							break;
						}
						case SDLK_ESCAPE:
						{
							
						}
						}
					}
					//Handle input for the CPlayer
					player.handleEvent(e);
				}

				//Move the CPlayer and set the camera
				player.move(blockSet);
				player.setCamera(camera);

				//Clear screen
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

				//Render level
				RenderLevel(camera, blockSet);

				//render bombs
				PlayerOneBombsRender(camera, frameBomb);
				PlayerTwoBombsRender(camera, frameBomb);

				
					for (int i = 0; i < 9; i++)
					{
						for (int e = 0; e < epl.size(); e++)
						{
						//if (timer.getTicks() / 1000 == 3.000)
						if (epl[e]->getTimer() / 1000 == 3.000)
						{
							//1 Explosions
							//explosion(*epl[e], blockSet);
							explosion(*epl[e], blockSet);
							epl[e]->render(camera, epl[e]->getMBox(i));
							dead(player, epl[e], yyy);
						}
						else
						{
							
								
							
						}
					}
				}

				player2.animation(camera, framePlayer2);
				player2.render(camera);

				player.animation(camera, framePlayer);
				player.render(camera);

				//SDL_Flip(screen);

				//Update screen
				SDL_RenderPresent(gRenderer);
			}
			//SDL_RemoveTimer(timerID);
		}

		//Free resources and close SDL
		close(blockSet);
	}
	return 0;
}
