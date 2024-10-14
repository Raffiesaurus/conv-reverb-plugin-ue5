// Copyright Raahul Sankaranarayanan, 2024. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Enums.h"
#include "IRLoader.h"
#include "EffectConvolutionReverb.h"
#include "ReverbShifter.h"
#include "GameFramework/Actor.h"
#include "SourceEffects/SourceEffectConvolutionReverb.h"
#include <Kismet/KismetSystemLibrary.h>
#include "Components/AudioComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "MetasoundSource.h"
#include "MetasoundAssetSubsystem.h"
#include "DynamicReverbShift.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REVERBSHIFT_API UDynamicReverbShiftComponent : public USceneComponent {
	GENERATED_BODY()

public:
	UDynamicReverbShiftComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReverbShift: Searching")
	float CheckInterval = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReverbShift: Searching")
	float SearchRadius = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReverbShift: Audio")
	UMetaSoundSource* SoundToPlay = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReverbShift: Audio")
	float VolumeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReverbShift: Audio")
	float PitchMultiplier = 1.0f;

	UFUNCTION(BlueprintCallable, Category = "ReverbShift")
	void PlayAudio();

	UFUNCTION(BlueprintCallable, Category = "ReverbShift")
	void StopAudio();

	UFUNCTION(BlueprintCallable, Category = "ReverbShift")
	void SetNewAudio(UMetaSoundSource* NewSoundToPlay);

	UFUNCTION(BlueprintCallable, Category = "ReverbShift")
	void SetNewReverbSelection(EReverbSelection NewReverb);

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void FindComponentsInRadius();
	void SetAudioProperties();
	void SetAttenuationValues();
	void ResetImpulse();
	void SetImpulse();

private:
	float TimeSinceLastCheck = 0.0f;
	float FallOffDistance = 3500.0f;
	float WetVolumeDb = 0.0f;
	float DryVolumeDb = -96.0f;

	EReverbSelection ReverbSelection = EReverbSelection::RS_None;

	bool UseCustomAttenuationSettings = false;
	bool UseCustomAttenuationAsset = false;
	bool UseCustomIR = false;
	bool EnableHardwareAcceleration = true;

	USoundAttenuation* AttenuationAsset = nullptr;

	TEnumAsByte<EAttenuationShape::Type> AttenuationShape = EAttenuationShape::Sphere;

	EAttenuationDistanceModel DistanceAlgorithm = EAttenuationDistanceModel::Linear;

	UAudioImpulseResponse* CustomIR = nullptr;

	UIRLoader* IRLoader = nullptr;

	UAudioComponent* AudioComponent = nullptr;

	USourceEffectConvolutionReverbPreset* IrToApply = nullptr;

	USoundAttenuation* DefaultAttenuation = nullptr;

	USoundEffectSourcePresetChain* DefaultSoundEffectToApply = nullptr;

	FSourceEffectChainEntry* DefaultEffectChain = nullptr;

	UAudioImpulseResponse* SelectedRIR = nullptr;

	FSoundAttenuationSettings DefaultAttenuationSettingsRef;
};