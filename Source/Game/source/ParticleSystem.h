#pragma once
#include "GameObject.h"
#include "Particle.h"
#include <tge/graphics/GraphicsEngine.h>
#include <vector>
#include <cstdint>
#include <random>

static Tga::Color operator+(const Tga::Color& aLhs, const Tga::Color& aRhs)
{
	return
	{
		aLhs.myR + aRhs.myR,
		aLhs.myG + aRhs.myG,
		aLhs.myB + aRhs.myB,
		aLhs.myA + aRhs.myA
	};
}

static Tga::Color operator-(const Tga::Color& aLhs, const Tga::Color& aRhs)
{
	return
	{
		aLhs.myR - aRhs.myR,
		aLhs.myG - aRhs.myG,
		aLhs.myB - aRhs.myB,
		aLhs.myA - aRhs.myA
	};
}

static Tga::Color operator*(const Tga::Color& aLhs, const Tga::Color& aRhs)
{
	return
	{
		aLhs.myR * aRhs.myR,
		aLhs.myG * aRhs.myG,
		aLhs.myB * aRhs.myB,
		aLhs.myA * aRhs.myA
	};
}

class ParticleSystem : GameObject
{
	friend class GameWorld;

private:
	static constexpr float ourDataScaleFactor = 10.f;

	std::vector<Particle> myParticlePool;
	Tga::SpriteSharedData mySprite;
	Tga::Vector2f mySpriteSize;
	int64_t myPoolIndex;
	int64_t myActiveParticles;
	int64_t myPoolSize;

	// TODO: Create a container for the random number generator
	static std::uniform_int_distribution<std::mt19937::result_type> ourDistr;
	static std::mt19937 ourRandom;

private:
	Tga::Color ColorLerp(Tga::Color aColorA, Tga::Color aColorB, float aTime)
	{
		return aColorA + Tga::Color(aTime, aTime, aTime, aTime) * (aColorB - aColorA);
	}

	Tga::Vector2f VectorLerp(Tga::Vector2f aVectorA, Tga::Vector2f aVectorB, float aTime)
	{
		return aVectorA + Tga::Vector2f(aTime, aTime) * (aVectorB - aVectorA);
	}

public:
	ParticleSystem() = default;
	ParticleSystem(const Tga::SpriteSharedData aSprite, const uint32_t aInitialPoolSize = 500);

	Tga::Vector2f& GetPosition();

	void Update(const float& aDeltaTime) override;
	void OnCollide(const GameObject& aOther, const Collision::CollisionHitData& aCollisionHitData, const CollisionDirection& aDirection) override 
	{ 
		UNREFERENCED_PARAMETER(aOther); 
		UNREFERENCED_PARAMETER(aCollisionHitData);
		UNREFERENCED_PARAMETER(aDirection);
	};
	std::shared_ptr<Tga::RenderObjectSharedData> GetModel() const { return nullptr; };
	void Render(Tga::SpriteDrawer& aDrawer) const;
	void Emit(const ParticleInstance& aParticle);
};

