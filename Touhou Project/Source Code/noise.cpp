#include "stdafx.h"
#include "noise.h"

NoiseGenerator * g_NGenerator;

float NoiseGenerator :: GenerateNoise(int x, int y, int random)
{
	int number = x + y * 57 + random * 131;
	number ^= (number << 13);
	return (((number * (number * number * 15731 + 78922) + 1376312589) & 0x7fffffff) * 0.000000000931322574615478515625f);
}