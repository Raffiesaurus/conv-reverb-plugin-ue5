#include "DynamicReverbShift.h"

UDynamicReverbShiftComponent::UDynamicReverbShiftComponent() {
	ReverbSelection = EReverbSelection::RS_None;
	IRLoader = NewObject<UIRLoader>();
	PrimaryComponentTick.bCanEverTick = true;
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->bAutoActivate = true;
	AudioComponent->SetupAttachment(this);
	AudioComponent->bDisableParameterUpdatesWhilePlaying = false;

	if (AudioComponent && GetOwner()) {
		AudioComponent->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	}
}

void UDynamicReverbShiftComponent::BeginPlay() {
	Super::BeginPlay();
	if (SoundToPlay == NULL) {
		UE_LOG(LogTemp, Error, TEXT("Invalid sound file."));
		return;
	}
	IrToApply = NewObject<USourceEffectConvolutionReverbPreset>(this, TEXT("IrToApply"));
	DefaultEffectChain = new FSourceEffectChainEntry();
	DefaultEffectChain->Preset = IrToApply;

	DefaultSoundEffectToApply = NewObject<USoundEffectSourcePresetChain>(this, TEXT("SoundEffect"));
	DefaultSoundEffectToApply->Chain.Empty();
	DefaultSoundEffectToApply->Chain.Add(*DefaultEffectChain);

	DefaultAttenuation = NewObject<USoundAttenuation>(this, TEXT("SoundAttenuation"));
	DefaultAttenuation->Attenuation = DefaultAttenuationSettingsRef;

	SoundToPlay->AttenuationSettings = NewObject<USoundAttenuation>(this, TEXT("MetaSoundAttenuation"));
	SoundToPlay->SourceEffectChain = NewObject<USoundEffectSourcePresetChain>(this, TEXT("MetaSoundEffect"));

	SetAudioProperties();
}

void UDynamicReverbShiftComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (SoundToPlay == NULL) {
		UE_LOG(LogTemp, Error, TEXT("Invalid sound file."));
		return;
	}
	TimeSinceLastCheck += DeltaTime;
	if (TimeSinceLastCheck >= CheckInterval) {
		FindComponentsInRadius();
		TimeSinceLastCheck = 0.0f;
	}
	AudioComponent->SetWorldLocation(GetOwner()->GetActorLocation());
}

void UDynamicReverbShiftComponent::SetNewReverbSelection(EReverbSelection NewReverb) {
	if (SoundToPlay == NULL) {
		UE_LOG(LogTemp, Error, TEXT("Invalid sound file."));
		return;
	}
	ReverbSelection = NewReverb;
}

void UDynamicReverbShiftComponent::FindComponentsInRadius() {
	FVector SphereCenter = GetOwner()->GetActorLocation();

	TArray<UPrimitiveComponent*> OverlappedComponents;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner()); // Ignore self
	UWorld* World = GetWorld();
	const UObject* Object = Cast<UObject>(World);

	bool bResult = UKismetSystemLibrary::SphereOverlapComponents(
		Object,
		SphereCenter,
		SearchRadius,
		ObjectTypes,
		UStaticMeshComponent::StaticClass(), // filter by specific component class
		ActorsToIgnore,
		OverlappedComponents
	);
	bool NearbyComponent = false;
	if (bResult) {
		NearbyComponent = false;
		for (UPrimitiveComponent* Component : OverlappedComponents) {
			AActor* ActorToCheck = Component->GetOwner();
			UReverbShifterComponent* ReverbShifterComponent = ActorToCheck->FindComponentByClass<UReverbShifterComponent>();
			if (ReverbShifterComponent != NULL) {
				NearbyComponent = true;
				if (ReverbSelection != ReverbShifterComponent->SpecificReverb) {
					ReverbSelection = ReverbShifterComponent->SpecificReverb;
					UseCustomAttenuationSettings = ReverbShifterComponent->UseCustomAttenuationSettings;
					UseCustomAttenuationAsset = ReverbShifterComponent->UseCustomAttenuationAsset;
					AttenuationAsset = ReverbShifterComponent->AttenuationAsset;
					AttenuationShape = ReverbShifterComponent->AttenuationShape;
					DistanceAlgorithm = ReverbShifterComponent->DistanceAlgorithm;
					FallOffDistance = ReverbShifterComponent->FallOffDistance;
					UseCustomIR = ReverbShifterComponent->UseCustomIR;
					CustomIR = ReverbShifterComponent->CustomIR;
					WetVolumeDb = ReverbShifterComponent->WetVolumeDb;
					DryVolumeDb = ReverbShifterComponent->DryVolumeDb;
					EnableHardwareAcceleration = ReverbShifterComponent->EnableHardwareAcceleration;
					SetAudioProperties();
				}
			}
		}
		if (!NearbyComponent && ReverbSelection != EReverbSelection::RS_None) {
			ReverbSelection = EReverbSelection::RS_None; // None of the actors closeby have the component
			SetAudioProperties();
		}
	} else {
		if (ReverbSelection != EReverbSelection::RS_None) {
			ReverbSelection = EReverbSelection::RS_None;	// No actors closeby
			SetAudioProperties();
		}
	}
}

void UDynamicReverbShiftComponent::SetAudioProperties() {
	if (UseCustomAttenuationAsset) {
		DefaultAttenuation->Attenuation = AttenuationAsset->Attenuation;
		SoundToPlay->AttenuationSettings = DefaultAttenuation;
	} else {
		SetAttenuationValues();
	}

	if (ReverbSelection == EReverbSelection::RS_None) {
		ResetImpulse();
	} else {
		SetImpulse();
	}
}

void UDynamicReverbShiftComponent::SetAttenuationValues() {
	DefaultAttenuationSettingsRef.bAttenuate = true;
	DefaultAttenuationSettingsRef.bSpatialize = true;
	DefaultAttenuationSettingsRef.bEnableOcclusion = true;
	DefaultAttenuationSettingsRef.bEnableListenerFocus = true;
	DefaultAttenuationSettingsRef.bUseComplexCollisionForOcclusion = true;
	DefaultAttenuationSettingsRef.SpatializationAlgorithm = ESoundSpatializationAlgorithm::SPATIALIZATION_HRTF;

	if (UseCustomAttenuationSettings) {
		DefaultAttenuationSettingsRef.AttenuationShape = AttenuationShape;
		DefaultAttenuationSettingsRef.DistanceAlgorithm = DistanceAlgorithm;
		DefaultAttenuationSettingsRef.FalloffDistance = FallOffDistance;
	} else {
		DefaultAttenuationSettingsRef.AttenuationShape = EAttenuationShape::Sphere;
		DefaultAttenuationSettingsRef.DistanceAlgorithm = EAttenuationDistanceModel::Linear;
		DefaultAttenuationSettingsRef.FalloffDistance = 3500.0f;
	}

	if (SoundToPlay != NULL) {

		if (SoundToPlay->AttenuationSettings != NULL) {
			SoundToPlay->AttenuationSettings = DefaultAttenuation;
		}
	} else {
		UE_LOG(LogTemp, Warning, TEXT("MetaSound does not exist."));
	}
}

void UDynamicReverbShiftComponent::ResetImpulse() {
	IrToApply = NewObject<USourceEffectConvolutionReverbPreset>(this, TEXT("IrToApply"));
	IrToApply->SetImpulseResponse(IRLoader->GetIR(EReverbSelection::RS_None, GetOwner()->GetName()));
	IrToApply->Settings.WetVolumeDb = -96.0f;
	IrToApply->Settings.DryVolumeDb = 0.0f;

	DefaultEffectChain->Preset = IrToApply;
	DefaultSoundEffectToApply->Chain.Empty();
	DefaultSoundEffectToApply->Chain.Add(*DefaultEffectChain);
	if (SoundToPlay != NULL) {

		if (SoundToPlay->SourceEffectChain != NULL) {
			SoundToPlay->SourceEffectChain->Chain.Empty();
			SoundToPlay->SourceEffectChain->Chain.Add(*DefaultEffectChain);
		} else {
			UE_LOG(LogTemp, Warning, TEXT("MetaSound SourceEffectChain does not exist."));
		}
	} else {
		UE_LOG(LogTemp, Warning, TEXT("MetaSound Source does not exist."));
	}
	if (AudioComponent->IsPlaying()) {
		StopAudio();
		PlayAudio();
	} else {
		StopAudio();
	}
}

void UDynamicReverbShiftComponent::PlayAudio() {
	if (SoundToPlay == NULL) {
		UE_LOG(LogTemp, Error, TEXT("Invalid sound file."));
		return;
	}

	AudioComponent->SetVolumeMultiplier(VolumeMultiplier);
	AudioComponent->SetPitchMultiplier(PitchMultiplier);
	AudioComponent->SetSound(SoundToPlay);

	AudioComponent->Play();

}

void UDynamicReverbShiftComponent::StopAudio() {
	if (SoundToPlay == NULL) {
		UE_LOG(LogTemp, Error, TEXT("Invalid sound file."));
		return;
	}
	AudioComponent->Stop();
}

void UDynamicReverbShiftComponent::SetNewAudio(UMetaSoundSource* NewSoundToPlay) {
	if (SoundToPlay == NULL) {
		UE_LOG(LogTemp, Error, TEXT("Invalid sound file."));
		return;
	}
	SoundToPlay = NewSoundToPlay;
}

void UDynamicReverbShiftComponent::SetImpulse() {
	IrToApply = NewObject<USourceEffectConvolutionReverbPreset>(this, TEXT("IrToApply"));
	SelectedRIR = IRLoader->GetIR(ReverbSelection, GetOwner()->GetName());
	if (UseCustomIR) {
		IrToApply->SetImpulseResponse(CustomIR);
	} else {
		IrToApply->SetImpulseResponse(SelectedRIR);
	}
	IrToApply->Settings.WetVolumeDb = WetVolumeDb;
	IrToApply->Settings.DryVolumeDb = DryVolumeDb;

	DefaultEffectChain->Preset = IrToApply;
	DefaultSoundEffectToApply->Chain.Empty();
	DefaultSoundEffectToApply->Chain.Add(*DefaultEffectChain);
	if (SoundToPlay != NULL) {
		if (SoundToPlay->SourceEffectChain != NULL) {
			SoundToPlay->SourceEffectChain->Chain.Empty();
			SoundToPlay->SourceEffectChain->Chain.Add(*DefaultEffectChain);
		} else {
			UE_LOG(LogTemp, Warning, TEXT("MetaSound SourceEffectChain does not exist."));
		}
	} else {
		UE_LOG(LogTemp, Warning, TEXT("MetaSound Source does not exist."));
	}
	if (AudioComponent->IsPlaying()) {
		StopAudio();
		PlayAudio();
	} else {
		StopAudio();
	}
}
