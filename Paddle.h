#ifndef _PADDLE_H
#define _PADDLE_H

#include "Vector2D.h"
#include <SDL.h>
#include <SDL_image.h>

#define PADDLE_WIDTH 10
#define PADDLE_HEIGHT 100

class Paddle {

	public:
		Paddle(Vector2D Position, Vector2D Velocity) : mPosition(Position), mVelocity(Velocity)
		{
			rect.x = static_cast<int>(Position.mX);
			rect.y = static_cast<int>(Position.mY);
			rect.w = PADDLE_WIDTH;
			rect.h = PADDLE_HEIGHT;
		}

		void Draw(SDL_Renderer* rend, SDL_Texture* text)
		{
			rect.y = static_cast<int>(mPosition.mY);
			SDL_RenderFillRect(rend, &rect);
			SDL_RenderCopy(rend, text, NULL, &rect);
		}

		void Update(float deltaTime)
		{
			mPosition += mVelocity * deltaTime;
			if (mPosition.getY() < 0)
			{
				mPosition.mY = 0;
			}
			else if (mPosition.getY() > (S_HEIGHT - PADDLE_HEIGHT))
			{
				mPosition.mY = S_HEIGHT - PADDLE_HEIGHT;
			}
		}

	public:
		Vector2D mPosition;
		Vector2D mVelocity;
		SDL_Rect rect{};
};

#endif