#include "StaticReverbModule.h"

UStaticReverbShiftComponent::UStaticReverbShiftComponent() {
	RoomSelection = ERoomSelection::RSE_None;
	IRLoader = NewObject<UIRLoader>();
	PrimaryComponentTick.bCanEverTick = true;
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->bAutoActivate = true;
	AudioComponent->bDisableParameterUpdatesWhilePlaying = false;
	AudioComponent->SetupAttachment(this);
}

void UStaticReverbShiftComponent::BeginPlay() {
	Super::BeginPlay();
	IrToApply = NewObject<USourceEffectConvolutionReverbPreset>(this, TEXT("IrToApply"));

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

void UStaticReverbShiftComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AudioComponent->SetWorldLocation(this->GetComponentLocation());
}

void UStaticReverbShiftComponent::SetAudioProperties() {

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

}

void UStaticReverbShiftComponent::SetAttenuationValues() {
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
	DefaultAttenuationSettingsRef = DefaultAttenuation->Attenuation;
}

void UStaticReverbShiftComponent::ResetImpulse() {
	UE_LOG(LogTemp, Warning, TEXT("Using clear reverb."));
	IrToApply->SetImpulseResponse(IRLoader->GetIR(ERoomSelection::RSE_None, GetOwner()->GetName()));
	IrToApply->Settings.WetVolumeDb = 0.0f;
	IrToApply->Settings.DryVolumeDb = 0.0f;
	IrToApply->bEnableHardwareAcceleration = false;
	IrToApply->BlockSize = ESubmixEffectConvolutionReverbBlockSize::BlockSize1024;

	DefaultEffectChain->Preset = IrToApply;
	DefaultSoundEffectToApply->Chain.Empty();
	DefaultSoundEffectToApply->Chain.Add(*DefaultEffectChain);
	AudioComponent->SetSourceEffectChain(DefaultSoundEffectToApply);
}

void UStaticReverbShiftComponent::SetImpulse() {
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

	DefaultEffectChain->Preset = IrToApply;
	DefaultSoundEffectToApply->Chain.Empty();
	DefaultSoundEffectToApply->Chain.Add(*DefaultEffectChain);
	AudioComponent->SetSourceEffectChain(DefaultSoundEffectToApply);
}


void UStaticReverbShiftComponent::SetNewRoomSelection(ERoomSelection NewRoom) {
	RoomSelection = NewRoom;
	if (RoomSelection == ERoomSelection::RSE_None) {
		ResetImpulse();
	} else {
		SetImpulse();
	}
}

void UStaticReverbShiftComponent::PlayAudio() {
	if (SoundToPlay == NULL) {
		UE_LOG(LogTemp, Warning, TEXT("Uhoh."));
		return;
	}

	AudioComponent->SetVolumeMultiplier(VolumeMultiplier);
	AudioComponent->SetPitchMultiplier(PitchMultiplier);
	AudioComponent->SetSound(SoundToPlay);

	AudioComponent->Play();
}

void UStaticReverbShiftComponent::StopAudio() {
	AudioComponent->Stop();
}

void UStaticReverbShiftComponent::SetNewAudio(USoundBase* NewSoundToPlay) {
	SoundToPlay = NewSoundToPlay;
}
