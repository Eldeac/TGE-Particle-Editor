#include "ParticleSystem.h"
#include <tge/drawers/SpriteDrawer.h>
#include <tge/error/ErrorManager.h>
#include <limits>
#include <algorithm>

std::uniform_int_distribution<std::mt19937::result_type> ParticleSystem::ourDistr;
std::mt19937 ParticleSystem::ourRandom;

ParticleSystem::ParticleSystem(const Tga::SpriteSharedData aSprite, const uint32_t aInitialPoolSize)
{
	mySprite = aSprite;
	// Use resize instead of reserve because it instantiates all particles instead of only reserving memory
	myParticlePool.resize(aInitialPoolSize);
	mySpriteSize = aSprite.myTexture->CalculateTextureSize();
	myPoolIndex = 0;
	myActiveParticles = 0;
	myPoolSize = aInitialPoolSize;
}

Tga::Vector2f& ParticleSystem::GetPosition()
{
	return myPosition;
}

void ParticleSystem::Update(const float& aDeltaTime)
{
	for (Particle& p : myParticlePool)
	{
		if (!p.active)
			continue;

		// Deactivate if dead
		p.currentLifetime += aDeltaTime;
		if (p.currentLifetime >= p.lifetime)
		{
			p.active = false;
			myActiveParticles--;
			continue;
		}

		// Simulate particle, scale values to enable nicer looking ParticleInstance values
		float particleTime = p.currentLifetime / p.lifetime;

		p.velocity += p.deltaVelocity * aDeltaTime;
		p.velocity += p.randomVelocity * aDeltaTime;
		p.instance.myColor = ColorLerp(p.startColor, p.endColor, particleTime);
		p.instance.mySize = mySpriteSize * VectorLerp(p.startSize, p.endSize, particleTime);
		p.instance.myPosition += p.velocity * aDeltaTime;
	}
}

void ParticleSystem::Render(Tga::SpriteDrawer& aDrawer) const
{
	// Use a sprite batch for performance
	Tga::SpriteBatchScope batch = aDrawer.BeginBatch(mySprite);

	int64_t renderedCount = 0;
	int64_t initialIndex = myPoolIndex - myActiveParticles;

	if (initialIndex < 0)
		initialIndex += myPoolSize;

	// Offset the start index to the newest activated particle
	for (int64_t i = initialIndex; renderedCount < myActiveParticles; i++)
	{
		renderedCount++;

		// Wrap around to the beginning
		if (i == myPoolSize)
			i -= myPoolSize;

		batch.Draw(myParticlePool[i].instance);
	}
}

void ParticleSystem::Emit(const ParticleInstance& aData)
{
	if (myActiveParticles == myPoolSize)
		return;

	// Grab next particle from the pool
	Particle& next = myParticlePool[myPoolIndex++];
	myPoolIndex %= myPoolSize;
	myActiveParticles++;

	// Set particle data
	next.active = true;

	next.instance.myPosition = myPosition;
	next.instance.myColor = aData.startColor;
	next.instance.mySize = mySpriteSize * aData.startSize;
	
	next.lifetime = aData.lifetime;
	next.currentLifetime = 0;

	next.velocity = aData.initialVelocity * ourDataScaleFactor;
	next.deltaVelocity = aData.deltaVelocity * ourDataScaleFactor;

	next.startSize = aData.startSize;
	next.endSize = aData.endSize;

	//																												Numeric limit max call needs to have parentheses
	//																												because the compiler confuses it with the max macro.
	//																												Very fun!
	next.randomVelocity.x = (aData.randomVelocity.x * ourDataScaleFactor) * (static_cast<float>(ourDistr(ourRandom)) / static_cast<float>((std::numeric_limits<uint32_t>::max)()) * 2.f - 1.f);
	next.randomVelocity.y = (aData.randomVelocity.y * ourDataScaleFactor) * (static_cast<float>(ourDistr(ourRandom)) / static_cast<float>((std::numeric_limits<uint32_t>::max)()) * 2.f - 1.f);

	next.startColor = aData.startColor;
	next.endColor = aData.endColor;
}
