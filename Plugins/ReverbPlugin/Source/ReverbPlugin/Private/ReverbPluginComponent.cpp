// Copyright Epic Games, Inc. All Rights Reserved.

#include "ReverbPluginComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Sound/SoundAttenuation.h"

UReverbPluginComponent::UReverbPluginComponent() {
	PrimaryComponentTick.bCanEverTick = false;
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->bAutoActivate = false;
	ReverbSubmix = nullptr;
	ReverbEffectPreset = nullptr;
}

void UReverbPluginComponent::BeginPlay() {
	Super::BeginPlay();

	if (AudioComponent && GetOwner()) {
		AudioComponent->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	}
	InitializeReverbSubmix();
}

USoundBase* UReverbPluginComponent::GetSelectedRIR() const {
	if (UseCustomIR && CustomIR) {
		return CustomIR;
	}
	FString Path = UIRsPathMapping::GetIRPath(RoomSelection);
	return LoadSoundFromPath(Path);
}

USoundBase* UReverbPluginComponent::LoadSoundFromPath(const FString& Path) const {
	if (Path.IsEmpty()) return nullptr;

	//USoundBase* LoadedSound = Cast<USoundBase>(StaticLoadObject(USoundBase::StaticClass(), nullptr, *Path));
	UAudioImpulseResponse* IR = LoadObject<UAudioImpulseResponse>(NULL, *Path);

	UE_LOG(LogTemp, Warning, TEXT("Trying to load sound from path: %s"), *Path);
	if (!IR) {
		UE_LOG(LogTemp, Warning, TEXT("Failed to load sound from path: %s"), *Path);
	}

	USoundBase* LoadedSound = NULL;
	return LoadedSound;
}

void UReverbPluginComponent::InitializeReverbSubmix() {
	if (ReverbSubmix) return;

	ReverbSubmix = NewObject<USoundSubmix>(this, TEXT("ReverbSubmix"));
	ReverbEffectPreset = NewObject<USubmixEffectReverbPreset>(ReverbSubmix);

	// Configure default reverb settings here
	FSubmixEffectReverbSettings ReverbSettings;
	ReverbSettings.DecayTime = 0.5f;
	ReverbSettings.Density = 0.85f;
	ReverbSettings.Diffusion = 0.9f;
	ReverbEffectPreset->SetSettings(ReverbSettings);

	ReverbSubmix->SubmixEffectChain.Add(ReverbEffectPreset);

	UE_LOG(LogTemp, Warning, TEXT("Reverb submix and effect preset initialized."));
}

void UReverbPluginComponent::ApplyReverbEffect(USoundBase* IR) {
	if (!ReverbEffectPreset || !IR) {
		UE_LOG(LogTemp, Warning, TEXT("Invalid reverb effect preset or IR."));
		return;
	}

	// Use the provided IR in reverb settings
	FSubmixEffectReverbSettings ReverbSettings;
	// Assuming you want to apply the IR in some custom way. Adjust this as needed.
	ReverbSettings.DecayTime = 0.5f;
	ReverbSettings.Density = 0.85f;
	ReverbSettings.Diffusion = 0.9f;
	ReverbEffectPreset->SetSettings(ReverbSettings);
	//UAudioImpulseResponse
	//ConvReverbEffectPreset->SetImpulseResponse();

	UE_LOG(LogTemp, Warning, TEXT("Reverb effect applied with IR."));
}


void UReverbPluginComponent::PlayAudio() {
	// Load the selected IR from file
	USoundBase* SelectedRIR = GetSelectedRIR();
	if (!SelectedRIR)
	{
		UE_LOG(LogTemp, Warning, TEXT("Error: Invalid IR."));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("PlayAudio called. Sound file exists."));

	// Get actor location to set audio component's location
	FVector ActorLocation = GetOwner()->GetActorLocation();
	UE_LOG(LogTemp, Warning, TEXT("Actor Location: %s"), *ActorLocation.ToString());

	// Set the main sound to play on the audio component
	AudioComponent->SetSound(SoundToPlay);

	// Set the world location of the audio component
	AudioComponent->SetWorldLocation(ActorLocation);

	// If no attenuation settings provided, create default settings
	if (!AttenuationSettings)
	{
		UE_LOG(LogTemp, Warning, TEXT("Creating default attenuation settings."));

		// Default attenuation settings if none are provided
		USoundAttenuation* DefaultAttenuation = NewObject<USoundAttenuation>(this, USoundAttenuation::StaticClass());
		FSoundAttenuationSettings& AttenuationSettingsRef = DefaultAttenuation->Attenuation;

		// Set default attenuation settings
		AttenuationSettingsRef.bAttenuate = true;
		AttenuationSettingsRef.AttenuationShape = EAttenuationShape::Sphere;
		AttenuationSettingsRef.AttenuationShapeExtents = FVector(400.0f);
		AttenuationSettingsRef.FalloffDistance = 2000.0f;

		// Spatialization Settings
		AttenuationSettingsRef.bSpatialize = true;
		AttenuationSettingsRef.SpatializationAlgorithm = ESoundSpatializationAlgorithm::SPATIALIZATION_HRTF;

		// Focus Settings
		AttenuationSettingsRef.bEnableListenerFocus = true;

		// Occlusion Settings
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

	// Apply the reverb effect with the loaded IR
	ApplyReverbEffect(SelectedRIR);

	// Play the audio through the audio component
	AudioComponent->Play();
}

void UReverbPluginComponent::SetNewRoomSelection(ERoomSelection NewRoom) {
	RoomSelection = NewRoom;
	PlayAudio();
}
