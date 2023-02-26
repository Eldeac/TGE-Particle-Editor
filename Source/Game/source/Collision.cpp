#include "Collision.h"

#include "GameObject.h"
#include <tge/engine.h>
#include <tge/graphics/GraphicsEngine.h>

//For Debug
#include <tge/primitives/LinePrimitive.h>
#include <tge/drawers/LineDrawer.h>

Collision::AABB::AABB()
{
	line = new Tga::LinePrimitive;
	line->color = { 1.0f, 1.0f, 1.0f, 1.f };

	myCollisionData = new CollisionData();
	myCollisionData->minX = {};
	myCollisionData->maxX = {};
	myCollisionData->minY = {};
	myCollisionData->maxY = {};
}

Collision::AABB::AABB(const Tga::Vector2f &aMin, const Tga::Vector2f &aMax, const Tga::Vector2f &aPosition)
{
	line = new Tga::LinePrimitive;
	line->color = { 1.0f, 1.0f, 1.0f, 1.f };

	myCollisionData = new CollisionData();
	SetCollisionData(aMin, aMax, aPosition);
}

Collision::AABB::~AABB()
{
	delete myCollisionData;
	delete line;
}

void Collision::AABB::SetCollisionData(const Tga::Vector2f &aMin, const Tga::Vector2f &aMax, const Tga::Vector2f &aPosition)
{
	myCollisionData->maxY = aMax.y;
	myCollisionData->minY = aMin.y;
	myCollisionData->maxX = aMax.x;
	myCollisionData->minX = aMin.x;
	myCollisionData->currentMinX = aPosition.x + aMin.x;
	myCollisionData->currentMaxX = aPosition.x + aMax.x;
	myCollisionData->currentMinY = aPosition.y + aMin.y;
	myCollisionData->currentMaxY = aPosition.y + aMax.y;

	line->color = { 1.0f, 1.0f, 1.0f, 1.f };
}

const Collision::CollisionData Collision::AABB::GetCollisionData() const
{
	return *myCollisionData;
}

void Collision::AABB::DebugRender(GameObject &aGameObject, Tga::Vector3f aOffset)
{
	auto col = aGameObject.GetCollider()->GetCollisionData();
	auto &engine = *Tga::Engine::GetInstance();

	Tga::Vector3f botLeft{ col.currentMinX, col.currentMinY, 0.f };
	Tga::Vector3f botRight{ col.currentMaxX, col.currentMinY, 0.f };
	Tga::Vector3f topLeft{ col.currentMinX, col.currentMaxY, 0.f };
	Tga::Vector3f topRight{ col.currentMaxX, col.currentMaxY, 0.f };

	std::array<Tga::Vector3f, 4> myPoints = { botLeft, botRight, topRight, topLeft };

	for (auto i = 0; i < 4; i++)
	{
		line->fromPosition = myPoints[0 + i] + aOffset;

		if (i > 2)
			line->toPosition = myPoints[0] + aOffset;

		else
			line->toPosition = myPoints[1 + i] + aOffset;

		engine.GetGraphicsEngine().GetLineDrawer().Draw(*line);
	}
}

void Collision::AABB::SetLineColor(Tga::Vector4f aColor)
{
	line->color = aColor;
}

const bool Collision::AABB::HasCollider() const
{
	float sum = myCollisionData->maxX + myCollisionData->maxX + myCollisionData->maxX + myCollisionData->maxX;

	return sum > 0;
}

Collision::Circle::Circle()
{
	myPosition = Tga::Vector2f();
	myRadius = 0;
}

Collision::Circle::Circle(const Tga::Vector2f &aPosition, const float &aRadius)
{
	myPosition = aPosition;
	myRadius = aRadius;
}

const Tga::Vector2f &Collision::Circle::GetPosition() const
{
	return myPosition;
}

void Collision::Circle::SetPosition(Tga::Vector2f aPosition)
{
	myPosition = aPosition;
}

const float &Collision::Circle::GetRadius() const
{
	return myRadius;
}

void Collision::Circle::SetRadius(float aRadius)
{
	myRadius = aRadius;
}

const bool Collision::AABBIntersect(const AABB &aSource, const AABB &aCollidesWith)
{
	if (aSource.GetCollisionData().currentMaxX <= aCollidesWith.GetCollisionData().currentMinX) return false; // a är till vänster om b
	if (aSource.GetCollisionData().currentMinX >= aCollidesWith.GetCollisionData().currentMaxX) return false; // a är till höger om b
	if (aSource.GetCollisionData().currentMaxY <= aCollidesWith.GetCollisionData().currentMinY) return false; // a är under b
	if (aSource.GetCollisionData().currentMinY >= aCollidesWith.GetCollisionData().currentMaxY) return false; // a är ovanför b
	return true; // AABB överlappar
}

const bool Collision::AABBIntersect(const AABB &aSource, const AABB &aCollidesWith, CollisionHitData &outHitData)
{
	outHitData.normal = { 0.f, 0.f };
	outHitData.hitPoint = { 0.f, 0.f };
	outHitData.collisionData = aCollidesWith.GetCollisionData();

	bool result = AABBIntersect(aSource, aCollidesWith);

	if (!result) return false;

	outHitData.normal.x += (aSource.GetCollisionData().currentMaxX <= aCollidesWith.GetCollisionData().currentMaxX) ? -1.f : 0.f;
	outHitData.normal.x += (aSource.GetCollisionData().currentMinX >= aCollidesWith.GetCollisionData().currentMinX) ? 1.f : 0.f;

	outHitData.normal.y += (aSource.GetCollisionData().currentMaxY <= aCollidesWith.GetCollisionData().currentMaxY) ? -1.f : 0.f;
	outHitData.normal.y += (aSource.GetCollisionData().currentMinY >= aCollidesWith.GetCollisionData().currentMinY) ? 1.f : 0.f;

	if (outHitData.normal.LengthSqr() != 0)
	{
		Tga::Vector2f min;
		min.x = aCollidesWith.GetCollisionData().currentMaxX - aSource.GetCollisionData().currentMinX;
		min.x *= outHitData.normal.x * outHitData.normal.x;
		min.y = aCollidesWith.GetCollisionData().currentMaxY - aSource.GetCollisionData().currentMinY;
		min.y *= outHitData.normal.y * outHitData.normal.y;


		Tga::Vector2f max;
		max.x = aSource.GetCollisionData().currentMaxX - aCollidesWith.GetCollisionData().currentMinX;
		max.x *= outHitData.normal.x * outHitData.normal.x;
		max.y = aSource.GetCollisionData().currentMaxY - aCollidesWith.GetCollisionData().currentMinY;
		max.y *= outHitData.normal.y * outHitData.normal.y;

		//Tga::Vector2f nearest;

		//if (outHitData.normal.x != 0)
		//{
		//	nearest.x = aCollidesWith.GetCollisionData().currentMinX;
		//	if (max.x > min.x) nearest.x = aCollidesWith.GetCollisionData().currentMaxX;

		//	//if (min.x < max.x)
		//	//	outHitData.normal.x = -1;
		//	//else {
		//	//	outHitData.normal.x = 1;
		//	//	nearest.x = max.x;
		//	//}
		//}

		//if (outHitData.normal.y != 0)
		//{
		//	nearest.y = aCollidesWith.GetCollisionData().currentMinY;
		//	if (max.y > min.y) nearest.y = aCollidesWith.GetCollisionData().currentMaxY;

		//	//if (min.y < max.y) outHitData.normal.y = 1;
		//	//else {
		//	//	outHitData.normal.y = -1;
		//	//	nearest.y = max.y;
		//	//}
		//}

		//if (outHitData.normal.x != 0 && outHitData.normal.y != 0)
		//{
		//	//float distanceX = Tga::Vector2f::Distance(
		//	//	aSource.GetCollisionData().origin +
		//	//	Tga::Vector2f{ aSource.GetCollisionData().maxX / 2.f, aSource.GetCollisionData().maxY / 2.f },
		//	//	Tga::Vector2f{ nearest.x, aCollidesWith.GetCollisionData().origin.y + aSource.GetCollisionData().maxY / 2.f }
		//	//);
		//	float distanceX = aSource.GetCollisionData().origin.x + aSource.GetCollisionData().maxX / 2.f - nearest.x;
		//	float distanceY = aSource.GetCollisionData().origin.y + aSource.GetCollisionData().maxY / 2.f - nearest.y;

		//	if (distanceX > distanceY) outHitData.normal.x = 0;
		//	else if (distanceX <= distanceY) outHitData.normal.y = 0;
		//}
		//if (outHitData.normal.x != 0 && outHitData.normal.y != 0) {
		//	if (nearest.x <= nearest.y)
		//	{
		//		outHitData.normal.x = 0;  // prioritizes x-axis
		//		outHitData.hitPoint.x = nearest.x;
		//	}
		//	else {
		//		outHitData.hitPoint.y = nearest.y;
		//		outHitData.normal.y = 0;
		//	}
		//}
		//std::cout << static_cast<int>(min.x) << "," << static_cast<int>(min.y) << " | " << static_cast<int>(max.x) << ", " << static_cast<int>(max.y) << std::endl;
	}
	return result;
}

const bool Collision::AABBPointIntersect(const AABB &aAABB, const Tga::Vector2f &aPoint)
{
	auto a = aAABB.GetCollisionData();

	return aPoint.x > a.minX && aPoint.x < a.maxX &&aPoint.y > a.minY && aPoint.y < a.maxY;

}

const bool Collision::AABBCircleIntersect(const AABB &aAABB, const Circle &aCircle)
{
	Tga::Vector2f a;

	if (aCircle.GetPosition().x >= aAABB.GetCollisionData().currentMaxX)
	{
		a.x = aAABB.GetCollisionData().currentMaxX;
	}
	else if (aCircle.GetPosition().x <= aAABB.GetCollisionData().currentMinX)
	{
		a.x = aAABB.GetCollisionData().currentMinX;
	}
	else
	{
		a.x = aCircle.GetPosition().x;
	}
	if (aCircle.GetPosition().y >= aAABB.GetCollisionData().currentMaxY)
	{
		a.y = aAABB.GetCollisionData().currentMaxY;
	}
	else if (aCircle.GetPosition().y <= aAABB.GetCollisionData().currentMinY)
	{
		a.y = aAABB.GetCollisionData().currentMinY;
	}
	else
	{
		a.y = aCircle.GetPosition().y;
	}

	return CirclePointIntersect(aCircle, a);
}

const bool Collision::CircleIntersect(const Circle &aCircle1, const Circle &aCircle2)
{
	Tga::Vector2f a = aCircle1.GetPosition() - aCircle2.GetPosition();
	return a.Length() <= aCircle1.GetRadius() + aCircle2.GetRadius();
}

const bool Collision::CirclePointIntersect(const Circle &aCircle, const Tga::Vector2f &aPoint)
{
	Tga::Vector2f a = aCircle.GetPosition() - aPoint;
	return a.Length() <= aCircle.GetRadius();
}

void Collision::AABB::SetOrigin(Tga::Vector2f aPosition)
{
	myCollisionData->currentMinX = aPosition.x + myCollisionData->minX;
	myCollisionData->currentMaxX = aPosition.x + myCollisionData->maxX;
	myCollisionData->currentMinY = aPosition.y + myCollisionData->minY;
	myCollisionData->currentMaxY = aPosition.y + myCollisionData->maxY;
}
