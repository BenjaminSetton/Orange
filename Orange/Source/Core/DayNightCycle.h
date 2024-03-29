#ifndef _DAY_NIGHT_CYCLE_H
#define _DAY_NIGHT_CYCLE_H

#include <DirectXMath.h>

#include "DirectionalLight.h"

class DayNightCycle
{
public:

	enum class CelestialBody
	{
		SUN = 0,
		MOON
	};

	enum class Cycle
	{
		DAY,
		NIGHT
	};

	enum class Time
	{
		SUNRISE = 0,
		DAYTIME,
		MIDDAY,
		SUNSET,
		NIGHTTIME,
		MIDNIGHT
	};

	static void Update(const float& dt);

	static const Cycle GetCycle();
	static const Time GetTime();
	static const DirectX::XMFLOAT3 GetLightPosition(const CelestialBody body);
	static const DirectX::XMFLOAT3 GetLightDirection(const CelestialBody body);
	static const DirectX::XMFLOAT4 GetLightColor(const CelestialBody body);
	static const float GetLightAmbient(const CelestialBody body);

	static const DirectX::XMFLOAT4 GetSkyColor();

private:

	static DirectX::XMFLOAT3 m_sunPos;
	static DirectX::XMFLOAT3 m_moonPos;

	static DirectionalLight m_sun;
	static DirectionalLight m_moon;

	static DirectX::XMFLOAT4 m_skyColor;

	static Cycle m_cycle;
	static Time m_time;

	// For internal use only
	static float m_elapsedTime;
};

#endif

