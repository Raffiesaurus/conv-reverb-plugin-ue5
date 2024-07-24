#include "ShiftingModule.h"

UReverbShiftComponent::UReverbShiftComponent() {
	RoomSelection = ERoomSelection::RSE_None;
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

void UReverbShiftComponent::BeginPlay() {
	Super::BeginPlay();

	IrToApply = NewObject<USourceEffectConvolutionReverbPreset>(this, TEXT("IrToApply"));
	IrToApply->SetImpulseResponse(IRLoader->GetIR(ERoomSelection::RSE_None, GetOwner()->GetName()));
	IrToApply->Settings.WetVolumeDb = 0.0f;
	IrToApply->Settings.DryVolumeDb = 0.0f;
	IrToApply->bEnableHardwareAcceleration = false;
	IrToApply->BlockSize = ESubmixEffectConvolutionReverbBlockSize::BlockSize1024;

	DefaultEffectChain = new FSourceEffectChainEntry();
	DefaultEffectChain->Preset = IrToApply;

	DefaultSoundEffectToApply = NewObject<USoundEffectSourcePresetChain>(this, TEXT("SoundEffect"));
	DefaultSoundEffectToApply->Chain.Empty();
	DefaultSoundEffectToApply->Chain.Add(*DefaultEffectChain);
	AudioComponent->SetSourceEffectChain(DefaultSoundEffectToApply);

	DefaultAttenuation = NewObject<USoundAttenuation>(this, TEXT("SoundAttenuation"));
	DefaultAttenuation->Attenuation = DefaultAttenuationSettingsRef;


	SetAudioProperties();
}

void UReverbShiftComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	TimeSinceLastCheck += DeltaTime;
	if (TimeSinceLastCheck >= CheckInterval) {
		FindComponentsInRadius();
		TimeSinceLastCheck = 0.0f;
	}
	AudioComponent->SetWorldLocation(GetOwner()->GetActorLocation());//SetAudioProperties();
}

void UReverbShiftComponent::FindComponentsInRadius() {
	FVector SphereCenter = GetOwner()->GetActorLocation();

	TArray<UPrimitiveComponent*> OverlappedComponents;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner()); // Ignore self

	bool bResult = UKismetSystemLibrary::SphereOverlapComponents(
		GetWorld(),
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
			UReverbShiftSpecifierComponent* SpecifierComponent = ActorToCheck->FindComponentByClass<UReverbShiftSpecifierComponent>();
			if (SpecifierComponent != NULL) {
				NearbyComponent = true;
				//UE_LOG(LogTemp, Warning, TEXT("The good object: %s"), *ActorToCheck->GetActorLabel());
				if (RoomSelection != SpecifierComponent->SpecificReverb) {
					UE_LOG(LogTemp, Warning, TEXT("Set new reverb. %s"), NearbyComponent ? TEXT("true") : TEXT("false"));
					RoomSelection = SpecifierComponent->SpecificReverb;
					UseCustomAttenuationSettings = SpecifierComponent->UseCustomAttenuationSettings;
					UseCustomAttenuationAsset = SpecifierComponent->UseCustomAttenuationAsset;
					AttenuationAsset = SpecifierComponent->AttenuationAsset;
					AttenuationShape = SpecifierComponent->AttenuationShape;
					DistanceAlgorithm = SpecifierComponent->DistanceAlgorithm;
					FallOffDistance = SpecifierComponent->FallOffDistance;
					UseCustomIR = SpecifierComponent->UseCustomIR;
					CustomIR = SpecifierComponent->CustomIR;
					WetVolumeDb = SpecifierComponent->WetVolumeDb;
					DryVolumeDb = SpecifierComponent->DryVolumeDb;
					EnableHardwareAcceleration = SpecifierComponent->EnableHardwareAcceleration;
					SetAudioProperties();
				}
			} else {
				//UE_LOG(LogTemp, Warning, TEXT("The bad object: %s"), *ActorToCheck->GetActorLabel())
			}
		}
		if (!NearbyComponent && RoomSelection != ERoomSelection::RSE_None) {
			UE_LOG(LogTemp, Warning, TEXT("Reset reverb none as no actors closeby have the component. %s"), NearbyComponent ? TEXT("true") : TEXT("false"));
			RoomSelection = ERoomSelection::RSE_None; // None of the actors closeby have the component
			SetAudioProperties();
		}
	} else {
		if (RoomSelection != ERoomSelection::RSE_None) {
			UE_LOG(LogTemp, Warning, TEXT("Reset reverb none as there are no actors. %s"), NearbyComponent ? TEXT("true") : TEXT("false"));
			RoomSelection = ERoomSelection::RSE_None;	// No actors closeby
			SetAudioProperties();
		}
	}
}

void UReverbShiftComponent::SetAudioProperties() {

	AudioComponent->AttenuationSettings = DefaultAttenuation;
	if (UseCustomAttenuationAsset) {
		DefaultAttenuation->Attenuation = AttenuationAsset->Attenuation;
	} else {
		SetAttenuationValues();
	}

	if (RoomSelection == ERoomSelection::RSE_None) {
		ResetImpulse();
	} else {
		SetImpulse();
	}

	/*bool bWasPlaying = AudioComponent->IsPlaying();
	if (bWasPlaying) {
		AudioComponent->Stop();
		AudioComponent->Play();
	}*/
}

void UReverbShiftComponent::SetAttenuationValues() {
	DefaultAttenuationSettingsRef.bAttenuate = true;
	DefaultAttenuationSettingsRef.bSpatialize = true;
	DefaultAttenuationSettingsRef.bEnableOcclusion = true;
	DefaultAttenuationSettingsRef.bEnableListenerFocus = true;
	DefaultAttenuationSettingsRef.bUseComplexCollisionForOcclusion = true;
	DefaultAttenuationSettingsRef.SpatializationAlgorithm = ESoundSpatializationAlgorithm::SPATIALIZATION_HRTF;

	if (UseCustomAttenuationSettings) {
		//UE_LOG(LogTemp, Warning, TEXT("Using provided attenuation settings."));
		DefaultAttenuationSettingsRef.AttenuationShape = AttenuationShape;
		DefaultAttenuationSettingsRef.DistanceAlgorithm = DistanceAlgorithm;
		DefaultAttenuationSettingsRef.FalloffDistance = FallOffDistance;
	} else {
		//UE_LOG(LogTemp, Warning, TEXT("Creating default attenuation settings."));
		DefaultAttenuationSettingsRef.AttenuationShape = EAttenuationShape::Sphere;
		DefaultAttenuationSettingsRef.DistanceAlgorithm = EAttenuationDistanceModel::Linear;
		DefaultAttenuationSettingsRef.FalloffDistance = 3500.0f;
	}
	DefaultAttenuationSettingsRef = DefaultAttenuation->Attenuation;
}

void UReverbShiftComponent::ResetImpulse() {
	UE_LOG(LogTemp, Warning, TEXT("Using clear reverb."));
	IrToApply->SetImpulseResponse(IRLoader->GetIR(ERoomSelection::RSE_None, GetOwner()->GetName()));
	IrToApply->Settings.WetVolumeDb = -96.0f;
	IrToApply->Settings.DryVolumeDb = 0.0f;
	IrToApply->bEnableHardwareAcceleration = false;
	IrToApply->BlockSize = ESubmixEffectConvolutionReverbBlockSize::BlockSize1024;
	//IrToApply->Settings.bBypass = true;

	DefaultEffectChain->Preset = IrToApply;
	DefaultEffectChain->bBypass = true;
	DefaultSoundEffectToApply->Chain.Empty();
	DefaultSoundEffectToApply->Chain.Add(*DefaultEffectChain);
	AudioComponent->SetSourceEffectChain(DefaultSoundEffectToApply);
}

void UReverbShiftComponent::SetImpulse() {
	SelectedRIR = IRLoader->GetIR(RoomSelection, GetOwner()->GetName());
	UE_LOG(LogTemp, Warning, TEXT("Using the actual changed reverb."));
	if (UseCustomIR) {
		IrToApply->SetImpulseResponse(CustomIR);
		IrToApply->Settings.WetVolumeDb = WetVolumeDb;
		IrToApply->Settings.DryVolumeDb = DryVolumeDb;
		IrToApply->bEnableHardwareAcceleration = EnableHardwareAcceleration;
	} else {
		IrToApply->SetImpulseResponse(SelectedRIR);
		IrToApply->Settings.WetVolumeDb = 0.0f;
		IrToApply->Settings.DryVolumeDb = -96.0f;
		IrToApply->bEnableHardwareAcceleration = true;
	}
	//IrToApply->Settings.bBypass = false;

	DefaultEffectChain->Preset = IrToApply;
	DefaultEffectChain->bBypass = false;
	DefaultSoundEffectToApply->Chain.Empty();
	DefaultSoundEffectToApply->Chain.Add(*DefaultEffectChain);
	AudioComponent->SetSourceEffectChain(DefaultSoundEffectToApply);
}

void UReverbShiftComponent::ResetAudioProperties() {
	//UE_LOG(LogTemp, Warning, TEXT("Audio reset."));
	//DefaultSoundEffectToApply = NewObject<USoundEffectSourcePresetChain>();
	//IrToApply = NewObject<USourceEffectConvolutionReverbPreset>();
	//DefaultAttenuation = NewObject<USoundAttenuation>();
	//AudioComponent->SetSourceEffectChain(DefaultSoundEffectToApply);
	//SetAudioProperties();
}

void UReverbShiftComponent::SetNewRoomSelection(ERoomSelection NewRoom) {
	RoomSelection = NewRoom;
}

void UReverbShiftComponent::PlayAudio() {
	if (SoundToPlay == NULL) {
		UE_LOG(LogTemp, Warning, TEXT("Uhoh."));
		return;
	}

	AudioComponent->SetVolumeMultiplier(VolumeMultiplier);
	AudioComponent->SetPitchMultiplier(PitchMultiplier);
	AudioComponent->SetSound(SoundToPlay);

	//SetAudioProperties();
	AudioComponent->Play();
}

void UReverbShiftComponent::StopAudio() {
	AudioComponent->Stop();
}

void UReverbShiftComponent::SetNewAudio(USoundBase* NewSoundToPlay) {
	SoundToPlay = NewSoundToPlay;
}
