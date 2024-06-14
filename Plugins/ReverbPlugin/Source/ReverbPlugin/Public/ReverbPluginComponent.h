// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Sound/SoundBase.h"
#include "IRsPathMapping.h"
#include "SubmixEffects/AudioMixerSubmixEffectReverb.h"
#include "SubmixEffects/SubmixEffectConvolutionReverb.h"
#include "ReverbPluginRoomEnums.h"
#include "Sound/SoundSubmix.h"
#include "ConvolutionReverb.h"
#include "ReverbPluginComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REVERBPLUGIN_API UReverbPluginComponent : public UActorComponent {
	GENERATED_BODY()

public:
	UReverbPluginComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* SoundToPlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	bool UseCustomAttenuationSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (EditCondition = "UseCustomAttenuationSettings", EditConditionHides))
	USoundAttenuation* AttenuationSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	bool UseCustomIR;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (EditCondition = "UseCustomIR", EditConditionHides))
	USoundBase* CustomIR;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (EditCondition = "!UseCustomIR", EditConditionHides))
	ERoomSelection RoomSelection;

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayAudio();

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetNewRoomSelection(ERoomSelection NewRoom);

protected:
	virtual void BeginPlay() override;

private:
	UAudioComponent* AudioComponent;

	USoundSubmix* ReverbSubmix;

	USubmixEffectReverbPreset* ReverbEffectPreset;

	USubmixEffectConvolutionReverbPreset* ConvReverbEffectPreset;

	USoundBase* GetSelectedRIR() const;
	USoundBase* LoadSoundFromPath(const FString& Path) const;

	void InitializeReverbSubmix();
	void ApplyReverbEffect(USoundBase* IR);
};
