#ifndef _BALL_H
#define _BALL_H

#include <SDL.h>
#include "Vector2D.h"
#include "Paddle.h"

#define BALL_WIDTH 15
#define BALL_HEIGHT 15
const float ballSpeed = .5f;

enum class CollisionType {
	None, Top, Middle, Bottom, Left, Right
};

struct Contact {
	CollisionType type;
	float penetration;
};

class Ball {
	public:
		
		Ball(Vector2D Position, Vector2D Velocity) : Position(Position), Velocity(Velocity)
		{
			rect.x = static_cast<int>(Position.mX);
			rect.y = static_cast<int>(Position.mY);
			rect.w = BALL_WIDTH;
			rect.h = BALL_HEIGHT;
		}

		void Draw(SDL_Renderer* rend)
		{
			rect.x = static_cast<int>(Position.mX);
			rect.y = static_cast<int>(Position.mY);

			SDL_RenderFillRect(rend, &rect);
		}

		void Update(float dt)
		{
			Position = (Position + Velocity * 1.25 * dt);
		}

		void CollideWithPaddle(Contact const& contact)
		{
			Position.mX += contact.penetration;
			Velocity.mX = -Velocity.mX;

			if (contact.type == CollisionType::Top)
			{
				Velocity.mY = -.75f * ballSpeed;
			}
			else if (contact.type == CollisionType::Bottom)
			{
				Velocity.mY = 0.75f * ballSpeed;
			}
		}

		void CollideWithWall(Contact const& contact)
		{
			if ((contact.type == CollisionType::Top)
				|| (contact.type == CollisionType::Bottom))
			{
				Position.mY += contact.penetration;
				Velocity.mY = -Velocity.mY;
			}
			else if (contact.type == CollisionType::Left)
			{
				Position.mX = S_WIDTH / 2.0f;
				Position.mY = S_HEIGHT / 2.0f;
				Velocity.mX = ballSpeed;
				Velocity.mY = 0.75f * ballSpeed;
			}
			else if (contact.type == CollisionType::Right)
			{
				Position.mX = S_WIDTH / 2.0f;
				Position.mY = S_HEIGHT / 2.0f;
				Velocity.mX = -ballSpeed;
				Velocity.mY = 0.75f * ballSpeed;
			}
		}

		Vector2D Position;
		Vector2D Velocity;
		SDL_Rect rect{};

};

#endif