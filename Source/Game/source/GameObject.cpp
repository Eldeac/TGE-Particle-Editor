#include "GameObject.h"
#include "Collision.h"

GameObject::GameObject() {}

GameObject::~GameObject()
{
	delete myCollider;
}

void GameObject::SetName(const std::string &aName)
{
	myName = aName;
}

const std::string & GameObject::GetName() const
{
	return myName;
}

Collision::AABB* GameObject::GetCollider() const
{
	return myCollider;
}

const Tga::Vector2f GameObject::GetPosition() const
{
	return myPosition;
}

const ModelType &GameObject::GetModelType() const
{
	return myModelType;
}
