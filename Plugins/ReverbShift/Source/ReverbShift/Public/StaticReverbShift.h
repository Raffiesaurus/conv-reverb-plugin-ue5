// Copyright Raahul Sankaranarayanan, 2024. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Enums.h"
#include "IRLoader.h"
#include "EffectConvolutionReverb.h"
#include "ReverbShifter.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "SourceEffects/SourceEffectConvolutionReverb.h"
#include "Engine/World.h"
#include "MetasoundSource.h"
#include "MetasoundAssetSubsystem.h"
#include <Kismet/KismetSystemLibrary.h>
#include "StaticReverbShift.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REVERBSHIFT_API UStaticReverbShiftComponent : public USceneComponent {
	GENERATED_BODY()

public:
	UStaticReverbShiftComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReverbShift: Audio")
	UMetaSoundSource* SoundToPlay = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReverbShift: Audio")
	float VolumeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReverbShift: Audio")
	float PitchMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReverbShift: Attenuation")
	bool UseCustomAttenuationSettings = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReverbShift: Attenuation", meta = (EditCondition = "UseCustomAttenuationSettings", EditConditionHides))
	bool UseCustomAttenuationAsset = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReverbShift: Attenuation", meta = (EditCondition = "UseCustomAttenuationAsset", EditConditionHides))
	USoundAttenuation* AttenuationAsset = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReverbShift: Attenuation", meta = (EditCondition = "!UseCustomAttenuationAsset && UseCustomAttenuationSettings", EditConditionHides))
	TEnumAsByte<EAttenuationShape::Type> AttenuationShape = EAttenuationShape::Sphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReverbShift: Attenuation", meta = (EditCondition = "!UseCustomAttenuationAsset && UseCustomAttenuationSettings", EditConditionHides))
	EAttenuationDistanceModel DistanceAlgorithm = EAttenuationDistanceModel::Linear;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReverbShift: Attenuation", meta = (EditCondition = "!UseCustomAttenuationAsset && UseCustomAttenuationSettings", EditConditionHides))
	float FallOffDistance = 3500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReverbShift: Reverb")
	bool UseCustomIR = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReverbShift: Reverb", meta = (EditCondition = "UseCustomIR", EditConditionHides))
	UAudioImpulseResponse* CustomIR = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReverbShift: Reverb", meta = (EditCondition = "UseCustomIR", EditConditionHides))
	float WetVolumeDb = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReverbShift: Reverb", meta = (EditCondition = "UseCustomIR", EditConditionHides))
	float DryVolumeDb = -96.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReverbShift: Reverb", meta = (EditCondition = "UseCustomIR", EditConditionHides))
	bool EnableHardwareAcceleration = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReverbShift: Reverb", meta = (EditCondition = "!UseCustomIR", EditConditionHides))
	EReverbSelection ReverbSelection = EReverbSelection::RS_None;

	UFUNCTION(BlueprintCallable, Category = "ReverbShift")
	void PlayAudio();

	UFUNCTION(BlueprintCallable, Category = "ReverbShift")
	void StopAudio();

	UFUNCTION(BlueprintCallable, Category = "ReverbShift")
	void SetNewAudio(UMetaSoundSource* NewSoundToPlay);

	UFUNCTION(BlueprintCallable, Category = "ReverbShift")
	void SetNewRoomSelection(EReverbSelection NewReverb);

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetAudioProperties();
	void SetAttenuationValues();
	void ResetImpulse();
	void SetImpulse();

private:

	UIRLoader* IRLoader = nullptr;

	UAudioComponent* AudioComponent = nullptr;

	USourceEffectConvolutionReverbPreset* IrToApply = nullptr;

	USoundAttenuation* DefaultAttenuation = nullptr;

	USoundEffectSourcePresetChain* DefaultSoundEffectToApply = nullptr;

	FSourceEffectChainEntry* DefaultEffectChain = nullptr;

	UAudioImpulseResponse* SelectedRIR = nullptr;

	FSoundAttenuationSettings DefaultAttenuationSettingsRef;

};