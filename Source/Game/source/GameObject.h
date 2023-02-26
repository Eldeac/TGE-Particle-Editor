#pragma once
#include <tge/math/vector2.h>
#include <string>
#include <memory>

namespace Tga
{
	struct UnityData;
	struct RenderObjectSharedData;
}

namespace Collision
{
	class AABB;
	struct CollisionHitData;
}

enum class CollisionDirection;

enum class ModelType
{
	eUnknown,
	eStatic,
	eAnimated
};

class GameObject
{
	friend class ObjectManager;
	friend class CollisionManager;

public:
	GameObject();
	virtual ~GameObject();

	virtual void Update(const float &aDeltaTime) = 0;
	virtual void OnCollide(const GameObject &aOther, const Collision::CollisionHitData &aCollisionHitData, const CollisionDirection &aDirection) = 0;

	// Getters
	Collision::AABB *GetCollider() const;
	const std::string &GetName() const;
	const ModelType &GetModelType() const;
	const Tga::Vector2f GetPosition() const;
	
	virtual std::shared_ptr<Tga::RenderObjectSharedData> GetModel() const = 0;

	// Setters
	void SetName(const std::string &aName);
protected:
	std::string myName = "";
	ModelType myModelType = ModelType::eUnknown;

	Collision::AABB *myCollider = nullptr;
	
	Tga::Vector2f myPosition;
};

