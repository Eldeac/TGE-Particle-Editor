#pragma once
//#include <tge/math/vector2.h>
#include <tge/math/Vector.h>


class GameObject;



namespace Tga
{
	struct LinePrimitive;
}

namespace Collision
{

	struct CollisionData
	{
		friend class AABB;

		float currentMinX = {};
		float currentMaxX = {};
		float currentMinY = {};
		float currentMaxY = {};
	
		float minX = {};
		float maxX = {};
		float minY = {};
		float maxY = {};
	private:
		Tga::Vector2f origin = {};
	};

	struct CollisionHitData
	{
		Tga::Vector2f hitPoint;
		Tga::Vector2f normal;
		CollisionData collisionData;
	};

	class AABB
	{
	public:
		AABB();
		AABB(const Tga::Vector2f &aMin, const Tga::Vector2f &aMax, const Tga::Vector2f &aPosition);
		~AABB();

		void SetCollisionData(const Tga::Vector2f &aMin, const Tga::Vector2f &aMax, const Tga::Vector2f &aPosition);
		const CollisionData GetCollisionData() const;
		void SetOrigin(Tga::Vector2f aPosition);

		//Add Offset for Actors
		void DebugRender(GameObject& aGameObject, Tga::Vector3f aOffset = {});
		void SetLineColor(Tga::Vector4f aColor);

		//returns true if the sum of all points are > 0
		const bool HasCollider() const;

	private:
		CollisionData* myCollisionData;
		Tga::LinePrimitive* line;
		
	};

	class Circle
	{
	public:
		Circle();
		Circle(const Tga::Vector2f &aPosition, const float &aRadius);
		const Tga::Vector2f &GetPosition() const;
		const float &GetRadius() const;
		void SetPosition(Tga::Vector2f aPosition);
		void SetRadius(float aRadius);
	private:
		Tga::Vector2f myPosition;
		float myRadius;
	};

	const bool AABBIntersect(const AABB &aSource, const AABB &aCollidesWith);
	const bool AABBIntersect(const AABB &aSource, const AABB &aCollidesWith, CollisionHitData &outHitData);
	const bool AABBPointIntersect(const AABB &aAABB, const Tga::Vector2f &aPoint);
	const bool AABBCircleIntersect(const AABB &aAABB, const Circle &aCircle);
	const bool CircleIntersect(const Circle &aCircle1, const Circle &aCircle2);
	const bool CirclePointIntersect(const Circle &aCircle, const Tga::Vector2f &aPoint);
}