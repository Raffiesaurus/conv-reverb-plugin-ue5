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

	FString RoomToLoad;
	switch (RoomSelection) {
	case ERoomSelection::RSE_Arena:
	case ERoomSelection::RSE_CarPark:
		RoomToLoad = "Car Park";
	case ERoomSelection::RSE_Cavern:
		RoomToLoad = "Cavern";
	case ERoomSelection::RSE_Cinema:
		RoomToLoad = "Cinema";
	case ERoomSelection::RSE_Hillside:
		RoomToLoad = "Hillside";
	default:
		RoomToLoad = "Uhoh";
	}
	UE_LOG(LogTemp, Warning, TEXT("Trying to load sound from path: %s"), *RoomToLoad);

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

	/*USoundWave* SoundWave = Cast<USoundWave>(SoundToPlay);

	if (SoundWave->GetResourceSizeBytes(EResourceSizeMode::EstimatedTotal) > 0) {
		int32 SoundFileSize = SoundWave->GetResourceSizeBytes(EResourceSizeMode::EstimatedTotal);
		UE_LOG(LogTemp, Log, TEXT("Sound file size: %d bytes"), SoundFileSize);
	}*/

	// 

	//// Accessing the raw audio data
	//if (SoundWave->GetResourceSizeBytes(EResourceSizeMode::EstimatedTotal) > 0) {
	//	FByteBulkData& BulkData = SoundWave->GetResourceData();
	//	uint8* RawWaveData = (uint8*)BulkData->Lock(LOCK_READ_ONLY);

	//	int32 DataSize = BulkData->GetBulkDataSize();
	//	TArray<uint8> RawDataArray;
	//	RawDataArray.Append(RawWaveData, DataSize);

	//	BulkData->Unlock();

	//	// You now have the raw audio data in RawDataArray
	//	UE_LOG(LogTemp, Log, TEXT("Retrieved raw audio data of size: %d"), DataSize);

	//	// Optionally, decode the raw audio data to PCM format
	//	ICompressedAudioInfo* Worker = nullptr;
	//	Worker->ReadCompressedData(RawDataArray.GetData(), DataSize, SoundWave->GetCompressionQuality());

	//	int16* PCMData = Worker->GetStreamingSoundWave()->GetResourceSize();
	//	int32 PCMDataSize = Worker->GetSourceBufferSize();

	//	// You now have the PCM data in PCMData
	//	UE_LOG(LogTemp, Log, TEXT("PCM data size: %d"), PCMDataSize);

	//	// Process PCMData as needed (e.g., to generate a waveform)
	//}

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
		DefaultAttenuationSettingsRef.SpatializationAlgorithm = ESoundSpatializationAlgorithm::SPATIALIZATION_HRTF;

		DefaultAttenuationSettingsRef.bEnableListenerFocus = true;

		DefaultAttenuationSettingsRef.bEnableOcclusion = true;
		DefaultAttenuationSettingsRef.bUseComplexCollisionForOcclusion = true;

		// Assign default attenuation settings to audio component
		AudioComponent->AttenuationSettings = DefaultAttenuation;
	}

	USourceEffectConvolutionReverbPreset* IrToApply = NewObject<USourceEffectConvolutionReverbPreset>();
	if (UseCustomIR) {
		UE_LOG(LogTemp, Warning, TEXT("Using custom."));
		IrToApply->SetImpulseResponse(CustomIR);
	} else {
		UE_LOG(LogTemp, Warning, TEXT("Using selected."));
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
