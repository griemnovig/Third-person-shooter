// Shoot Them Up Game, All Rights Reserved.

#include "Sound/STUSoundFuncLib.h"
#include "Sound/SoundClass.h"

float USTUSoundFuncLib::GetSoundClassVolume(USoundClass* SoundClass)
{
	if(!SoundClass) return 0.0f;
	
	return SoundClass->Properties.Volume;
}

void USTUSoundFuncLib::SetSoundClassVolume(USoundClass* SoundClass, const float Volume)
{
	if(!SoundClass) return;

	SoundClass->Properties.Volume = FMath::Clamp(Volume, 0.0f, 1.0f);
}

void USTUSoundFuncLib::ToggleSoundClassVolume(USoundClass* SoundClass)
{
	if(!SoundClass) return;

	const float NextVolume = SoundClass->Properties.Volume > 0.0f ? 0.0f : 1.0f;
	SetSoundClassVolume(SoundClass, NextVolume);
}
