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
	DefaultConvReverbEffectPreset = nullptr;
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
	} else {
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

	if (UseCustomAttenuationSettings) {
		UE_LOG(LogTemp, Warning, TEXT("Using provided attenuation settings."));
		AudioComponent->AttenuationSettings = AttenuationSettings;
	} else {
		UE_LOG(LogTemp, Warning, TEXT("Creating default attenuation settings."));

		DefaultAttenuation = NewObject<USoundAttenuation>();
		FSoundAttenuationSettings& DefaultAttenuationSettingsRef = DefaultAttenuation->Attenuation;

		DefaultAttenuationSettingsRef.bAttenuate = true;
		DefaultAttenuationSettingsRef.AttenuationShape = EAttenuationShape::Sphere;
		DefaultAttenuationSettingsRef.AttenuationShapeExtents = FVector(400.0f);
		DefaultAttenuationSettingsRef.FalloffDistance = 2000.0f;

		DefaultAttenuationSettingsRef.bSpatialize = true;
		DefaultAttenuationSettingsRef.SpatializationAlgorithm = ESoundSpatializationAlgorithm::SPATIALIZATION_Default;

		DefaultAttenuationSettingsRef.bEnableListenerFocus = true;

		DefaultAttenuationSettingsRef.bEnableOcclusion = true;
		DefaultAttenuationSettingsRef.bUseComplexCollisionForOcclusion = true;

		// Assign default attenuation settings to audio component
		AudioComponent->AttenuationSettings = DefaultAttenuation;
	}

	USourceEffectConvolutionReverbPreset* IrToApply = NewObject<USourceEffectConvolutionReverbPreset>();
	if (UseCustomIR) {
		IrToApply->SetImpulseResponse(CustomIR);
	} else {
		IrToApply->SetImpulseResponse(SelectedRIR);
	}

	IrToApply->Settings.WetVolumeDb = 0.0f;
	IrToApply->Settings.DryVolumeDb = -96.0f;
	IrToApply->Settings.bBypass = false;
	IrToApply->BlockSize = ESubmixEffectConvolutionReverbBlockSize::BlockSize1024;
	IrToApply->bEnableHardwareAcceleration = true;

	UE_LOG(LogTemp, Warning, TEXT("Created USourceEffectConvolutionReverbPreset."));

	DefaultSoundEffectToApply = NewObject<USoundEffectSourcePresetChain>();
	FSourceEffectChainEntry* DefaultEffectChain = new FSourceEffectChainEntry();
	DefaultEffectChain->Preset = IrToApply;
	UE_LOG(LogTemp, Warning, TEXT("Created FSourceEffectChainEntry."));
	DefaultSoundEffectToApply->Chain.Add(*DefaultEffectChain);
	UE_LOG(LogTemp, Warning, TEXT("Created USoundEffectSourcePresetChain."));

	AudioComponent->SetSourceEffectChain(DefaultSoundEffectToApply);
	UE_LOG(LogTemp, Warning, TEXT("Set SourceEffectChain."));

	AudioComponent->Play();
	UE_LOG(LogTemp, Warning, TEXT("Played."));
}

void UReverbPluginComponent::SetNewRoomSelection(ERoomSelection NewRoom) {
	RoomSelection = NewRoom;
	PlayAudio();
}
