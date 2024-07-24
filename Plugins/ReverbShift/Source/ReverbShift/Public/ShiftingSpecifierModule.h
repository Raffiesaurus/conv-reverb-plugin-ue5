#pragma once

#include "CoreMinimal.h"
#include "Enums.h"
#include "IRLoader.h"
#include "Components/AudioComponent.h"
#include "EffectConvolutionReverb.h"
#include "SubmixEffects/SubmixEffectConvolutionReverb.h"
#include "SourceEffects/SourceEffectConvolutionReverb.h"
#include <Kismet/KismetSystemLibrary.h>
#include "ShiftingSpecifierModule.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REVERBSHIFT_API UReverbShiftSpecifierComponent : public USceneComponent {
	GENERATED_BODY()

public:
	UReverbShiftSpecifierComponent();

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReverbShift: Reverb", meta = (EditCondition = "!UseCustomIR", EditConditionHides))
	ERoomSelection SpecificReverb = ERoomSelection::RSE_Cinema;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReverbShift: Reverb", meta = (EditCondition = "UseCustomIR", EditConditionHides))
	UAudioImpulseResponse* CustomIR = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReverbShift: Reverb")
	float WetVolumeDb = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReverbShift: Reverb")
	float DryVolumeDb = -96.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReverbShift: Reverb")
	bool EnableHardwareAcceleration = true;

	UFUNCTION(BlueprintCallable, Category = "ReverbShift")
	void SetNewRoomSelection(ERoomSelection NewRoom);

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};