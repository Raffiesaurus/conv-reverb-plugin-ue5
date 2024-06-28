// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "IRsPathMapping.h"
#include "SubmixEffects/AudioMixerSubmixEffectReverb.h"
#include "SubmixEffects/SubmixEffectConvolutionReverb.h"
#include "ReverbPluginRoomEnums.h"
#include "Sound/SoundSubmix.h"
#include "Sound/SoundWave.h"
#include "AudioDecompress.h"
#include "Logging/LogMacros.h"
#include "ConvolutionReverb.h"
#include "Engine/Attenuation.h"
#include "Sound/SoundEffectSource.h"
#include "EffectConvolutionReverb.h"
#include "ProceduralMeshComponent.h"
#include "SourceEffects/SourceEffectConvolutionReverb.h"
#include "ReverbPluginComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REVERBPLUGIN_API UReverbPluginComponent : public USceneComponent {
	GENERATED_BODY()

public:
	UReverbPluginComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	float VolumeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	float PitchMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* SoundToPlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	bool UseCustomAttenuationSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (EditCondition = "UseCustomAttenuationSettings", EditConditionHides))
	USoundAttenuation* AttenuationSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (EditCondition = "!UseCustomAttenuationSettings", EditConditionHides))
	TEnumAsByte<EAttenuationShape::Type> AttenuationShape;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (EditCondition = "!UseCustomAttenuationSettings", EditConditionHides))
	TEnumAsByte<EAttenuationDistanceModel> AttenuationFunction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (EditCondition = "!UseCustomAttenuationSettings", EditConditionHides))
	FVector InnerRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (EditCondition = "!UseCustomAttenuationSettings", EditConditionHides))
	float FallOffDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	bool UseCustomIR;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (EditCondition = "UseCustomIR", EditConditionHides))
	UAudioImpulseResponse* CustomIR;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (EditCondition = "!UseCustomIR", EditConditionHides))
	ERoomSelection RoomSelection;

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayAudio();

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetNewRoomSelection(ERoomSelection NewRoom);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void TakeMeshShape();

protected:
	virtual void BeginPlay() override;

private:
	UAudioComponent* AudioComponent;

	USoundSubmix* DefaultReverbSubmix;

	USubmixEffectConvolutionReverbPreset* DefaultConvReverbEffectPreset;

	USoundAttenuation* DefaultAttenuation;

	USoundEffectSourcePresetChain* DefaultSoundEffectToApply;

	UAudioImpulseResponse* GetSelectedRIR() const;
	UAudioImpulseResponse* LoadIRFromPath(const FString& Path) const;

	UPROPERTY()
	UProceduralMeshComponent* DetectionMesh;

	void UpdateVolumeBounds();
	void GenerateMeshFromEnvironment();
};
