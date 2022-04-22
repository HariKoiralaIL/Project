#include "Dependencies/SDL2_image/include/SDL_image.h"
#include "Dependencies/SDL2_ttf/include/SDL_ttf.h"
#include "Dependencies/SDL2_mixer/include/SDL_mixer.h"
#include "Dependencies/SDL2/include/SDL.h"
#include <iostream>
#include <chrono> // Used for time that is used to move ball around in window
#include "Ball.h"
#include "Paddle.h"
#include "PlayerScore.h"
#include "Food.h"

SDL_Window* gWindow;
SDL_Renderer* gRenderer;
TTF_Font* gFont;
bool isRunning;
SDL_Event event;
Mix_Chunk* wallHitSound = NULL;
Mix_Chunk* paddleHitSound = NULL;
Mix_Music* backgroundMusic = NULL;
Mix_Chunk* coinPickup = NULL;

// Paddle Key State Pressed
enum Buttons { leftUP = 0, leftDOWN, rightUP, rightDOWN};
bool buttons[4] = {};

const float paddleSpeed = 1.0f;
Contact checkPaddleCollision(Ball const& ball, Paddle const& paddle);
void CollideWithPaddle(Contact const& contact);
Contact checkWallCollision(Ball const& ball);
bool checkFoodCollision(Ball const& ball, Food const& food);

void CleanUp();
void drawNet();

int main(int argc, char* argv[])
{
	/* 
		Initializes SDL : Gotta Do this before we can use any SDL Features.
		Here SDL_Init() returns -1 if error occurs so we check just in case SDL is not properly setup.
	*/
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cout << "SDL Initialzation Error: " << SDL_GetError();
		return 0;
	}

	/*
		Initializes the Font: We use this to draw text upon the screen.
	*/
	if (TTF_Init() < 0)
	{
		std::cout << "Failed to Initialize Font: " << TTF_GetError();
		return 0;
	}
	/*
		Next Step is to create our window and renderer. The window is where everything is going to get displayed to.
		The Renderer will take Textures and render them to the window. We could have created Window and Renderer seperately
		using SDL_CreateWindow() and SDL_CreateRenderer() but this much easier since you can do at same time.
	*/
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		std::cout << "Failed to Initialize SDL Audio Subsystem: " << Mix_GetError();
		return 0;
	}

	SDL_CreateWindowAndRenderer(S_WIDTH, S_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN, &gWindow, &gRenderer);
	SDL_SetWindowTitle(gWindow, "Pong - Classic");
	if (gWindow == NULL || gRenderer == NULL)
	{
		std::cout << "SDL Window or Renderer Creation Error: " << SDL_GetError();
		return 0;
	}

	gFont = TTF_OpenFont("OpenSans-Regular.ttf", 40);
	if (gFont == NULL)
	{
		std::cout << "Opening Font Error: " << TTF_GetError();
		return 0;
	}

	SDL_Surface* rectImg = IMG_Load("Sprite-0001.png");
	SDL_Texture* rectText = SDL_CreateTextureFromSurface(gRenderer, rectImg);
	SDL_FreeSurface(rectImg);

	SDL_Surface* appleImg = IMG_Load("Sprite-0002.png");
	SDL_Texture* appleText = SDL_CreateTextureFromSurface(gRenderer, appleImg);
	SDL_FreeSurface(appleImg);

	// Scaling Window
	SDL_RenderSetLogicalSize(gRenderer, S_WIDTH, S_HEIGHT);

	// Create Ball Object: It takes a Vector2D Object: So we pass it its X and Y Pos resulting in Middle of Window
	Ball ball(Vector2D((S_WIDTH / 2.0f), (S_HEIGHT / 2.0f)), Vector2D(ballSpeed, 0.0f));;
	Food food(Vector2D(S_WIDTH / 4, S_HEIGHT / 4));

	Paddle leftPaddle( Vector2D(50.0f, (S_HEIGHT / 2.0f)), Vector2D(0.0f,0.0f));
	Paddle rightPaddle( Vector2D(S_WIDTH - 50.0f, (S_HEIGHT / 2.0f)), Vector2D(0.0f,0.0f));

	PlayerScore leftScore(Vector2D(S_WIDTH / 4, 20), gRenderer, gFont);
	PlayerScore rightScore(Vector2D(3 * S_WIDTH / 4, 20), gRenderer, gFont);

	/*
		Basic Main Game Loop. We have a Sentinial Value isRunning just checking if some event triggres it to be false.
		So Main Idea is: Get Input, Process Input, Update Game Based on that
	*/
	int playerOneScore = 0;
	int playerTwoScore = 0;

	isRunning = true;

	float dt = 0.0f;

	backgroundMusic = Mix_LoadMUS("backgroundMusic.wav");
	if (backgroundMusic == NULL)
	{
		std::cout << "Failed to Load Music: Game still works tho!" << Mix_GetError();
	}
	if (Mix_PlayingMusic() == 0) {
		Mix_PlayMusic(backgroundMusic, -1);
	}

	while (isRunning)
	{
		auto startTime = std::chrono::high_resolution_clock::now();// Starts Clock from here
		while (SDL_PollEvent(&event))
		{
			// Just Pressing Big Red Close on Window
			if (event.type == SDL_QUIT)
			{
				isRunning = false;
			}
			else if (event.type == SDL_KEYDOWN) // Pressing down on the keyboard - i.e. You press a key
			{
				if (event.key.keysym.sym == SDLK_ESCAPE) // If Key Symbol is the escape key
				{
					isRunning = false;
				}
				else if (event.key.keysym.sym == SDLK_w)
				{
					buttons[Buttons::leftUP] = true;
				}
				else if (event.key.keysym.sym == SDLK_s)
				{
					buttons[Buttons::leftDOWN] = true;
				}
				else if (event.key.keysym.sym == SDLK_UP)
				{
					buttons[Buttons::rightUP] = true;
				}
				else if (event.key.keysym.sym == SDLK_DOWN)
				{
					buttons[Buttons::rightDOWN] = true;
				}
			}
			else if (event.type == SDL_KEYUP) // You Stop pressing the key - hence we set all to false since you are not doing anything
			{
				if (event.key.keysym.sym == SDLK_w)
				{
					buttons[Buttons::leftUP] = false;
				}
				else if (event.key.keysym.sym == SDLK_s)
				{
					buttons[Buttons::leftDOWN] = false;
				}
				else if (event.key.keysym.sym == SDLK_UP)
				{
					buttons[Buttons::rightUP] = false;
				}
				else if (event.key.keysym.sym == SDLK_DOWN)
				{
					buttons[Buttons::rightDOWN] = false;
				}
			}
		}

		// Unless Press UP or DOWN for the left paddle, the paddle does not move : Left Player Paddle
		if (buttons[Buttons::leftUP]) { leftPaddle.mVelocity.mY = -paddleSpeed; }
		else if (buttons[Buttons::leftDOWN]) { leftPaddle.mVelocity.mY = +paddleSpeed; }
		else { leftPaddle.mVelocity.mY = 0.0f; }

		if (buttons[Buttons::rightUP]) { rightPaddle.mVelocity.mY = -paddleSpeed; }
		else if (buttons[Buttons::rightDOWN]) { rightPaddle.mVelocity.mY = paddleSpeed; }
		else { rightPaddle.mVelocity.mY = 0.0f; }

		leftPaddle.Update(dt);
		rightPaddle.Update(dt);

		ball.Update(dt);


		// Check Collisions
		Contact contact;
		if (contact = checkPaddleCollision(ball, leftPaddle),contact.type != CollisionType::None)
		{
			paddleHitSound = Mix_LoadWAV("paddleCollide.wav");
			ball.CollideWithPaddle(contact);
			Mix_PlayChannel(-1, paddleHitSound, 1);
		}
		else if (contact = checkPaddleCollision(ball, rightPaddle), contact.type != CollisionType::None)
		{
			paddleHitSound = Mix_LoadWAV("paddleCollide.wav");
			ball.CollideWithPaddle(contact);
			Mix_PlayChannel(-1, paddleHitSound, 0);
		}
		else if (contact = checkWallCollision(ball), contact.type != CollisionType::None)
		{
			ball.CollideWithWall(contact);
			if (contact.type == CollisionType::Left)
			{
				++playerTwoScore;
				rightScore.setScore(playerTwoScore);
			}
			else if (contact.type == CollisionType::Right)
			{
				++playerOneScore;
				leftScore.setScore(playerOneScore);
			}
			else
			{
				wallHitSound = Mix_LoadWAV("wallCollide.wav");
				Mix_PlayChannel(-1, wallHitSound, 0);
			}
		} 
		else if (checkFoodCollision(ball,food))
		{	// Going to the Right
			if (ball.Velocity.mX > 0)
			{
				++playerOneScore;
				leftScore.setScore(playerOneScore);
				coinPickup = Mix_LoadWAV("coinPickup.wav");
				Mix_PlayChannel(-1, coinPickup, 0);
				food.Draw(gRenderer, appleText);
				food.Update();
			}
			else if (ball.Velocity.mX < 0)
			{
				++playerTwoScore;
				rightScore.setScore(playerTwoScore);
				coinPickup = Mix_LoadWAV("coinPickup.wav");
				Mix_PlayChannel(-1, coinPickup, 0);
				food.Draw(gRenderer, appleText);
				food.Update();
			}
		food.Update();
		}

		// Just sets the Window to color passed - that is 0x00 0x00 0x00 which is Black
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(gRenderer);

		drawNet();
		ball.Draw(gRenderer);
		food.Draw(gRenderer, appleText);
		leftPaddle.Draw(gRenderer, rectText);
		rightPaddle.Draw(gRenderer, rectText);
		leftScore.Draw();
		rightScore.Draw();

		// Updates the Surface so we get what we rendered displayed - Renders stuff on the Back Buffer
		SDL_RenderPresent(gRenderer);

		// Frame Time
		auto stopTime = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration<float, std::chrono::milliseconds::period>(stopTime - startTime).count();
	}
	CleanUp();
	return 0;
}

void CleanUp()
{
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;
	
	Mix_FreeChunk(wallHitSound);
	wallHitSound = NULL;

	Mix_FreeChunk(paddleHitSound);
	paddleHitSound = NULL;

	Mix_FreeMusic(backgroundMusic);
	backgroundMusic = NULL;

	Mix_FreeChunk(coinPickup);
	coinPickup = NULL;
	
	TTF_CloseFont(gFont);

	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void drawNet()
{
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF); // White
	
	// Each 10 Pixel, It draws a Point. After drawing it all, it looks a dashed line
	/*
		Important thing is the coordinate system in computer graphics. Here the Origin is Top Left (0,0). So the Y-value gets
		larger as you go down and X-value gets larger as it goes to the right.
	*/
	for (int i = 0; i < S_HEIGHT; i++)
	{
		if (i % 10)
		{
			// The X Position is Width/2, so the middle and it never changes. The Y Position depends on i. Here i Increases and if 
			// it is multiple of 10 then we draw a point at (Width/2 , i). Since Y grows as it goes down, this works.
			SDL_RenderDrawPoint(gRenderer, S_WIDTH / 2,i);
		}
	}
}

Contact checkPaddleCollision(Ball const& ball, Paddle const& paddle)
{
	// get four sides of the rectangle - since collision is based on Checking overlapping of lines
	float ballLeft = ball.Position.mX;
	float ballRight = ball.Position.mX + BALL_WIDTH;
	float ballTop = ball.Position.mY;
	float ballBottom = ball.Position.mY + BALL_HEIGHT;

	float paddleLeft = paddle.mPosition.mX;
	float paddleRight = paddle.mPosition.mX + PADDLE_WIDTH;
	float paddleTop = paddle.mPosition.mY;
	float paddleBottom = paddle.mPosition.mY + PADDLE_HEIGHT;

	Contact contact{};

	if (ballLeft >= paddleRight) { return contact; }
	if (ballRight <= paddleLeft) { return contact; }
	if (ballTop >= paddleBottom) { return contact; }
	if (ballBottom <= paddleTop) { return contact; }

	float paddleUpperRange = paddleBottom - (2.0f * PADDLE_HEIGHT / 3.0f);
	float paddleLowerRange = paddleBottom - (PADDLE_HEIGHT / 3.0f);

	if (ball.Velocity.mX < 0)
	{
		contact.penetration = paddleRight - paddleLeft;
	}
	else if (ball.Velocity.mX > 0)
	{
		contact.penetration = paddleLeft - paddleRight;
	}

	if ((ballBottom > paddleTop) && (ballBottom < paddleUpperRange))
	{
		contact.type = CollisionType::Top;
	}
	else if ((ballBottom > paddleUpperRange) && (ballBottom < paddleLowerRange))
	{
		contact.type = CollisionType::Middle;
	}
	else
	{
		contact.type = CollisionType::Bottom;
	}
	return contact;
}

Contact checkWallCollision(Ball const& ball)
{
	float ballLeft = ball.Position.mX;
	float ballRight = ball.Position.mX + BALL_WIDTH;
	float ballTop = ball.Position.mY;
	float ballBottom = ball.Position.mY + BALL_HEIGHT;

	Contact contact{};
	if (ballLeft < 0.0f)
	{
		contact.type = CollisionType::Left;
	}
	else if (ballRight > S_WIDTH)
	{
		contact.type = CollisionType::Right;
	}
	else if (ballTop < 0.0f)
	{
		contact.type = CollisionType::Top;
		contact.penetration = -ballTop;
	}
	else if (ballBottom > S_HEIGHT)
	{
		contact.type = CollisionType::Bottom;
		contact.penetration = S_HEIGHT - ballBottom;
	}
	return contact;
}

bool checkFoodCollision(Ball const& ball, Food const& food)
{
	// Since it was rectangle of same size, just use HasIntersection(). This is kind of buggy for the reason that for each dt
	// it returns true, so when it hits, the score can get increase more than once and so i count that as a feature.

	if (SDL_HasIntersection(&ball.rect, &food.mRect))
	{
		return true;
	}
	return false;
}