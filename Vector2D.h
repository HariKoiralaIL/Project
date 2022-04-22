#ifndef _VECTOR2D_H
#define _VECTOR2D_H

#define S_HEIGHT 840
#define S_WIDTH 840

class Vector2D {
	public:

		Vector2D() : mX(0.0f), mY(0.0f) {}
		Vector2D(float x, float y)
		{
			mX = x;
			mY = y;
		}

		Vector2D operator+(Vector2D const& rhs)
		{
			return Vector2D(mX + rhs.mX, mY + rhs.mY);
		}

		Vector2D operator+=(Vector2D const& rhs)
		{
			mX += rhs.mX;
			mY += rhs.mY;

			return *this;
		}

		Vector2D operator*(float rhs)
		{
			return Vector2D(mX * rhs, mY * rhs);
		}

		float getX()
		{
			return mX;
		}

		float getY()
		{
			return mY;
		}

	public:
		float mX;
		float mY;
};

#endif