#ifndef _PLAYERSCORE_H
#define _PLAYERSCORE_H

#include "Vector2D.h"
#include <SDL.h>
#include <string>
#include <SDL_ttf.h>

using std::string;

class PlayerScore {
	public:
		PlayerScore(Vector2D Position, SDL_Renderer* rend, TTF_Font* font) : mRenderer(rend), mFont(font)
		{
			mSurface = TTF_RenderText_Solid(font, "0", { 0xFF, 0xFF, 0xFF, 0xFF });
			mTexture = SDL_CreateTextureFromSurface(rend, mSurface);

			int width, height;
			SDL_QueryTexture(mTexture, nullptr, nullptr, &width, &height);

			mRect.x = static_cast<int>(Position.getX());
			mRect.y = static_cast<int>(Position.getY());
			mRect.w = width;
			mRect.h = height;
		}

		~PlayerScore()
		{
			SDL_FreeSurface(mSurface);
			SDL_DestroyTexture(mTexture);
		}

		void Draw()
		{
			SDL_RenderCopy(mRenderer, mTexture, nullptr, &mRect);
		}

		void setScore(int score)
		{
			SDL_FreeSurface(mSurface);
			SDL_DestroyTexture(mTexture);

			mSurface = TTF_RenderText_Solid(mFont, std::to_string(score).c_str(), { 0xFF, 0xFF, 0xFF, 0xFF });
			mTexture = SDL_CreateTextureFromSurface(mRenderer, mSurface);

			int width, height;
			SDL_QueryTexture(mTexture, nullptr, nullptr, &width, &height);
			mRect.w = width;
			mRect.h = height;
		}
	private:
		SDL_Renderer* mRenderer;
		TTF_Font* mFont;
		SDL_Surface* mSurface;
		SDL_Texture* mTexture;
		SDL_Rect mRect{};
};

#endif