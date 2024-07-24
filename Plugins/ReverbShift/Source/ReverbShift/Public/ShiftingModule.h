#pragma once

#include "CoreMinimal.h"
#include "Enums.h"
#include "IRLoader.h"
#include "EffectConvolutionReverb.h"
#include "ShiftingSpecifierModule.h"
#include "Components/AudioComponent.h"
#include "SourceEffects/SourceEffectConvolutionReverb.h"
#include <Kismet/KismetSystemLibrary.h>
#include "ShiftingModule.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REVERBSHIFT_API UReverbShiftComponent : public USceneComponent {
	GENERATED_BODY()

public:
	UReverbShiftComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReverbShift: Searching")
	float CheckInterval = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReverbShift: Searching")
	float SearchRadius = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReverbShift: Audio")
	USoundBase* SoundToPlay = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReverbShift: Audio")
	float VolumeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReverbShift: Audio")
	float PitchMultiplier = 1.0f;

	UFUNCTION(BlueprintCallable, Category = "ReverbShift")
	void PlayAudio();

	UFUNCTION(BlueprintCallable, Category = "ReverbShift")
	void StopAudio();

	UFUNCTION(BlueprintCallable, Category = "ReverbShift")
	void SetNewAudio(USoundBase* NewSoundToPlay);

	UFUNCTION(BlueprintCallable, Category = "ReverbShift")
	void SetNewRoomSelection(ERoomSelection NewRoom);

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void FindComponentsInRadius();
	void SetAudioProperties();
	void SetAttenuationValues();
	void ResetImpulse();
	void SetImpulse();
	void ResetAudioProperties();

private:
	float TimeSinceLastCheck = 0.0f;

	ERoomSelection RoomSelection = ERoomSelection::RSE_None;
	bool UseCustomAttenuationSettings = false;
	bool UseCustomAttenuationAsset = false;
	USoundAttenuation* AttenuationAsset = nullptr;
	TEnumAsByte<EAttenuationShape::Type> AttenuationShape = EAttenuationShape::Sphere;
	EAttenuationDistanceModel DistanceAlgorithm = EAttenuationDistanceModel::Linear;
	float FallOffDistance = 3500.0f;
	bool UseCustomIR = false;
	UAudioImpulseResponse* CustomIR = nullptr;
	float WetVolumeDb = 0.0f;
	float DryVolumeDb = -96.0f;
	bool EnableHardwareAcceleration = true;

	UIRLoader* IRLoader = nullptr;

	UAudioComponent* AudioComponent = nullptr;

	USourceEffectConvolutionReverbPreset* IrToApply = nullptr;

	USoundAttenuation* DefaultAttenuation = nullptr;

	USoundEffectSourcePresetChain* DefaultSoundEffectToApply = nullptr;

	FSourceEffectChainEntry* DefaultEffectChain = nullptr;

	UAudioImpulseResponse* SelectedRIR = nullptr;

	FSoundAttenuationSettings DefaultAttenuationSettingsRef;

	FSourceEffectConvolutionReverb* test = nullptr;

};