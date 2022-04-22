#ifndef _FOOD_H
#define _FOOD_H

#include <SDL.h>
#include "Vector2D.h"
#include <random>

#define FOOD_WIDTH 15
#define FOOD_HEIGHT 15

class Food {
	public:
		Food(Vector2D Position) : mPosition(Position)
		{
			mRect.x = static_cast<int>(Position.mX);
			mRect.y = static_cast<int>(Position.mY);
			mRect.w = FOOD_WIDTH;
			mRect.h = FOOD_HEIGHT;
		}

		void Draw(SDL_Renderer* rend, SDL_Texture* text)
		{
			mRect.x = static_cast<int>(mPosition.mX);
			mRect.y = static_cast<int>(mPosition.mY);

			SDL_RenderFillRect(rend, &mRect);
			SDL_RenderCopy(rend, text, NULL, &mRect);
		}
		
		void Update()
		{
			//std::srand((unsigned int)time(nullptr));

			mPosition.mX = rand() % S_WIDTH + 20;
			mPosition.mY = rand() % S_HEIGHT + 20;
		}

	public:
		Vector2D mPosition;
		SDL_Rect mRect{};
};

#endif