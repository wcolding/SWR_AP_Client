#include "AIScaling.h"

namespace SWRGame
{
	//////////////////////
	// AI Scaling Methods
	//////////////////////
	// Method 1: Scale according to the quality of your pod's parts
	//
	// Pieces will be weighted differently
	PodParts partWeights = { 2, 3, 3, 3, 1, 2, 1 };

	float GetAIScaleFromParts()
	{
		float playerScore = 0;
		float maxScore = 0;
		float low = 0.846f; // lowest scale in vanilla
		float high = 1.135f; // highest scale in vanilla

		for (int i = 0; i < 7; i++)
		{
			playerScore += (float)swrSaveData->parts[i] * (float)swrSaveData->partsHealth[i] * (float)partWeights[i];
			maxScore += (float)6 * (float)255 * (float)partWeights[i];
		}

		Log("Player parts score: %f / %f", playerScore, maxScore);
		float scaled = (high - low) * (playerScore - 0) / (maxScore - 0) + low; // may tweak since base parts don't degrade
		scaled *= aiModifier;
		Log("Scale: %f", scaled);
		return scaled;
	}

	// Method 2: Scale the vanilla track values based on their circuit placement
	// Amateur will be easier than Semi-Pro, Semi-Pro easier than Galactic, etc
	//
	// These modifiers are based on the average vanilla values per circuit
	const std::vector<float> circuitAIConversions = {
		1.121f, // Amateur -> Semi-Pro
		1.064f, // Semi-Pro -> Galactic
		1.018f  // Galactic -> Invitational
	};

	float GetAIScaleByCircuit(float val, int baseCircuit, int targetCircuit)
	{
		if (baseCircuit < targetCircuit)
		{
			// Upscale
			while (baseCircuit < targetCircuit)
			{
				val *= circuitAIConversions[baseCircuit];
				baseCircuit++;
			}
		}
		else
		{
			// Downscale
			while (baseCircuit > targetCircuit)
			{
				val /= circuitAIConversions[baseCircuit - 1];
				baseCircuit--;
			}
		}

		val *= aiModifier;
		return val;
	}

	float GetAIScaleVanilla(float val)
	{
		return val * aiModifier;
	}

	void ChangeAIModifier(float amount)
	{
		if (!modifierControl)
			return;

		// Only change this on the Main Menu (pre race screen)
		if ((*menuVal == 3) && (*menuValB == 3))
		{
			aiModifier += amount;
		}
	}
}