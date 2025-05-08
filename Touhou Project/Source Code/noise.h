#pragma once

#include "Singleton.h"

class NoiseGenerator : public Singleton <NoiseGenerator>
{

public:
	float GenerateNoise(int x, int y, int random);	// put in a value for random using rand with your own parameters
	
};

extern NoiseGenerator * g_NGenerator;