// Copyright Epic Games, Inc. All Rights Reserved.

#include "ReverbPluginComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Sound/SoundAttenuation.h"
#include "EffectConvolutionReverb.h"

UReverbPluginComponent::UReverbPluginComponent() {
	PrimaryComponentTick.bCanEverTick = false;
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->bAutoActivate = false;
	ConvReverbEffectPreset = nullptr;
}

void UReverbPluginComponent::BeginPlay() {
	Super::BeginPlay();

	if (AudioComponent && GetOwner()) {
		AudioComponent->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	}
}

UAudioImpulseResponse* UReverbPluginComponent::GetSelectedRIR() const {
	if (UseCustomIR && CustomIR) {
		return CustomIR;
	}
	FString Path = UIRsPathMapping::GetIRPath(RoomSelection);
	return LoadIRFromPath(Path);
}

UAudioImpulseResponse* UReverbPluginComponent::LoadIRFromPath(const FString& Path) const {
	if (Path.IsEmpty()) return nullptr;

	UAudioImpulseResponse* IR = Cast<UAudioImpulseResponse>(StaticLoadObject(UAudioImpulseResponse::StaticClass(), nullptr, *Path));

	UE_LOG(LogTemp, Warning, TEXT("Trying to load sound from path: %s"), *Path);
	if (!IR) {
		UE_LOG(LogTemp, Warning, TEXT("Failed to load sound from path: %s"), *Path);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Successfully loaded IR from path: %s"), *Path);
	}

	return IR;
}

void UReverbPluginComponent::PlayAudio() {

	UAudioImpulseResponse* SelectedRIR = GetSelectedRIR();

	UE_LOG(LogTemp, Warning, TEXT("PlayAudio called. Sound file exists."));

	FVector ActorLocation = GetOwner()->GetActorLocation();
	UE_LOG(LogTemp, Warning, TEXT("Actor Location: %s"), *ActorLocation.ToString());

	AudioComponent->SetSound(SoundToPlay);

	AudioComponent->SetWorldLocation(ActorLocation);

	if (!AttenuationSettings)
	{
		UE_LOG(LogTemp, Warning, TEXT("Creating default attenuation settings."));

		USoundAttenuation* DefaultAttenuation = NewObject<USoundAttenuation>(this, USoundAttenuation::StaticClass());
		FSoundAttenuationSettings& AttenuationSettingsRef = DefaultAttenuation->Attenuation;

		AttenuationSettingsRef.bAttenuate = true;
		AttenuationSettingsRef.AttenuationShape = EAttenuationShape::Sphere;
		AttenuationSettingsRef.AttenuationShapeExtents = FVector(400.0f);
		AttenuationSettingsRef.FalloffDistance = 2000.0f;

		AttenuationSettingsRef.bSpatialize = true;
		AttenuationSettingsRef.SpatializationAlgorithm = ESoundSpatializationAlgorithm::SPATIALIZATION_HRTF;

		AttenuationSettingsRef.bEnableListenerFocus = true;

		AttenuationSettingsRef.bEnableOcclusion = true;
		AttenuationSettingsRef.bUseComplexCollisionForOcclusion = true;

		// Assign default attenuation settings to audio component
		AudioComponent->AttenuationSettings = DefaultAttenuation;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Using provided attenuation settings."));
		AudioComponent->AttenuationSettings = AttenuationSettings;
	}

	USourceEffectConvolutionReverbPreset* IrToApply = NewObject<USourceEffectConvolutionReverbPreset>();
	if (UseCustomIR) {
		IrToApply->SetImpulseResponse(CustomIR);
	}
	else {
		IrToApply->SetImpulseResponse(SelectedRIR);
	}

	IrToApply->Settings.WetVolumeDb = 0.0f;
	IrToApply->Settings.DryVolumeDb = -96.0f;
	IrToApply->Settings.bBypass = false;
	IrToApply->BlockSize = ESubmixEffectConvolutionReverbBlockSize::BlockSize1024;
	IrToApply->bEnableHardwareAcceleration = true;

	UE_LOG(LogTemp, Warning, TEXT("Created USourceEffectConvolutionReverbPreset."));

	USoundEffectSourcePresetChain* SoundEffectToApply = NewObject<USoundEffectSourcePresetChain>();
	FSourceEffectChainEntry* ApplyChain = new FSourceEffectChainEntry();
	ApplyChain->Preset = IrToApply;
	UE_LOG(LogTemp, Warning, TEXT("Created FSourceEffectChainEntry."));
	SoundEffectToApply->Chain.Add(*ApplyChain);
	UE_LOG(LogTemp, Warning, TEXT("Created USoundEffectSourcePresetChain."));

	AudioComponent->SetSourceEffectChain(SoundEffectToApply);
	UE_LOG(LogTemp, Warning, TEXT("Set SourceEffectChain."));

	AudioComponent->Play();
	UE_LOG(LogTemp, Warning, TEXT("Played."));
}

void UReverbPluginComponent::SetNewRoomSelection(ERoomSelection NewRoom) {
	RoomSelection = NewRoom;
	PlayAudio();
}
