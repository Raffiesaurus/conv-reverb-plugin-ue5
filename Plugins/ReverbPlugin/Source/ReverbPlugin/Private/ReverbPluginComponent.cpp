// Copyright Epic Games, Inc. All Rights Reserved.

#include "ReverbPluginComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Sound/SoundAttenuation.h"
#include "EffectConvolutionReverb.h"

UReverbPluginComponent::UReverbPluginComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->bAutoActivate = true;
	AudioComponent->SetupAttachment(this);

	// Create and attach a ProceduralMeshComponent for detection
	DetectionMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("DetectionMesh"));
	DetectionMesh->SetupAttachment(this);
}

void UReverbPluginComponent::BeginPlay() {
	Super::BeginPlay();

	if (AudioComponent && GetOwner()) {
		AudioComponent->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	}

	TakeMeshShape();
}

void UReverbPluginComponent::TakeMeshShape() {
	UpdateVolumeBounds();
	PlayAudio();
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

void UReverbPluginComponent::UpdateVolumeBounds() {
	GenerateMeshFromEnvironment();
}

void UReverbPluginComponent::GenerateMeshFromEnvironment() {
	FVector ActorLocation = GetOwner()->GetActorLocation();
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	TArray<FOverlapResult> OverlapResults;
	GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		ActorLocation,
		FQuat::Identity,
		ECC_WorldStatic,
		FCollisionShape::MakeSphere(1000.0f),  // Adjust the sphere radius as needed
		QueryParams
	);

	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UV0;
	TArray<FProcMeshTangent> Tangents;

	for (const FOverlapResult& Result : OverlapResults) {
		if (Result.GetActor() && Result.GetActor() != GetOwner()) {
			FBox ActorBox = Result.GetActor()->GetComponentsBoundingBox();
			FVector ActorMin = ActorBox.Min;
			FVector ActorMax = ActorBox.Max;

			// Generate vertices and triangles for a simple box shape as an example
			// This can be adapted to create more complex shapes
			Vertices.Add(ActorMin);
			Vertices.Add(FVector(ActorMax.X, ActorMin.Y, ActorMin.Z));
			Vertices.Add(FVector(ActorMax.X, ActorMax.Y, ActorMin.Z));
			Vertices.Add(FVector(ActorMin.X, ActorMax.Y, ActorMin.Z));

			Vertices.Add(FVector(ActorMin.X, ActorMin.Y, ActorMax.Z));
			Vertices.Add(FVector(ActorMax.X, ActorMin.Y, ActorMax.Z));
			Vertices.Add(ActorMax);
			Vertices.Add(FVector(ActorMin.X, ActorMax.Y, ActorMax.Z));

			Triangles.Append({ 0, 1, 2, 0, 2, 3 });
			Triangles.Append({ 4, 6, 5, 4, 7, 6 });
			Triangles.Append({ 0, 4, 5, 0, 5, 1 });
			Triangles.Append({ 1, 5, 6, 1, 6, 2 });
			Triangles.Append({ 2, 6, 7, 2, 7, 3 });
			Triangles.Append({ 3, 7, 4, 3, 4, 0 });

			// Normals, UVs, and Tangents can be calculated based on the vertices and triangles
		}
	}

	DetectionMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, TArray<FLinearColor>(), Tangents, true);

	// Optionally, draw the mesh for debugging
	DrawDebugBox(GetWorld(), DetectionMesh->GetComponentLocation(), DetectionMesh->Bounds.BoxExtent, FColor::Red, false, 10.0f);
}

void UReverbPluginComponent::PlayAudio() {
	if (SoundToPlay == NULL) {
		return;
	}
	UAudioImpulseResponse* SelectedRIR = GetSelectedRIR();

	UE_LOG(LogTemp, Warning, TEXT("PlayAudio called. Sound file exists."));

	AudioComponent->SetVolumeMultiplier(VolumeMultiplier);
	AudioComponent->SetPitchMultiplier(PitchMultiplier);

	FVector ActorLocation = GetOwner()->GetActorLocation();
	AudioComponent->SetWorldLocation(ActorLocation);
	AudioComponent->SetSound(SoundToPlay);

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
}

void UReverbPluginComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateVolumeBounds();
}