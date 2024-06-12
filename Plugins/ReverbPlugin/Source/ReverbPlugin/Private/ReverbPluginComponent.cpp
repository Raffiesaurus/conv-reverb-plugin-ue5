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
}

void UReverbPluginComponent::BeginPlay() {
	Super::BeginPlay();

	if(AudioComponent && GetOwner()) {
		AudioComponent->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	}
}

USoundBase* UReverbPluginComponent::GetSelectedRIR() const {
	FString Path = UIRsPathMapping::GetIRPath(RoomSelection);
	return LoadSoundFromPath(Path);
}

USoundBase* UReverbPluginComponent::LoadSoundFromPath(const FString& Path) const {
	if(Path.IsEmpty()) return nullptr;

	return Cast<USoundBase>(StaticLoadObject(USoundBase::StaticClass(), nullptr, *Path));
}


void UReverbPluginComponent::PlayAudio() {
	USoundBase* SelectedRIR = GetSelectedRIR();
	if(SelectedRIR) {
		UE_LOG(LogTemp, Warning, TEXT("PlayAudio called. Sound file exists."));

		FVector ActorLocation = GetOwner()->GetActorLocation();
		UE_LOG(LogTemp, Warning, TEXT("Actor Location: %s"), *ActorLocation.ToString());

		AudioComponent->SetSound(SelectedRIR);
		AudioComponent->SetWorldLocation(ActorLocation);

		if(!AttenuationSettings) {
			UE_LOG(LogTemp, Warning, TEXT("Creating default attenuation settings."));

			// Create default attenuation settings if none are provided
			USoundAttenuation* DefaultAttenuation = NewObject<USoundAttenuation>(this, USoundAttenuation::StaticClass());

			// Configure the default attenuation settings
			FSoundAttenuationSettings& AttenuationSettingsRef = DefaultAttenuation->Attenuation;

			// Attenuation Settings
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

			UE_LOG(LogTemp, Warning, TEXT("Default Attenuation Settings Created. Attenuate: %d, Shape: %d, Extents: %s, FalloffDistance: %f"),
				AttenuationSettingsRef.bAttenuate, (int)AttenuationSettingsRef.AttenuationShape, *AttenuationSettingsRef.AttenuationShapeExtents.ToString(), AttenuationSettingsRef.FalloffDistance);

			AudioComponent->AttenuationSettings = DefaultAttenuation;
		} else {
			UE_LOG(LogTemp, Warning, TEXT("Using provided attenuation settings."));
			AudioComponent->AttenuationSettings = AttenuationSettings;
		}
		AudioComponent->Play();
	} else {
		UE_LOG(LogTemp, Warning, TEXT("No sound file provided."));
	}
}
