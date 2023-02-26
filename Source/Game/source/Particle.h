#pragma once
#include <tge/sprite/sprite.h>
#include <tge/math/Vector.h>

struct ParticleInstance
{
	float lifetime;
	Tga::Vector2f initialVelocity, deltaVelocity, randomVelocity;
	Tga::Vector2f startSize, endSize;
	Tga::Color startColor, endColor;
};

struct Particle
{
	bool active;
	float lifetime, currentLifetime;
	Tga::Sprite2DInstanceData instance;
	Tga::Vector2f velocity, deltaVelocity, randomVelocity;
	Tga::Vector2f startSize, endSize;
	Tga::Color startColor, endColor;
};