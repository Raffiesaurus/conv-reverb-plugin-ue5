#include "StaticReverbShift.h"

UStaticReverbShiftComponent::UStaticReverbShiftComponent() {
	ReverbSelection = EReverbSelection::RS_None;
	IRLoader = NewObject<UIRLoader>();
	PrimaryComponentTick.bCanEverTick = true;
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->bAutoActivate = true;
	AudioComponent->bDisableParameterUpdatesWhilePlaying = false;
	AudioComponent->SetupAttachment(this);
}

void UStaticReverbShiftComponent::BeginPlay() {
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

void UStaticReverbShiftComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (SoundToPlay == NULL) {
		UE_LOG(LogTemp, Error, TEXT("Invalid sound file."));
		return;
	}
	AudioComponent->SetWorldLocation(this->GetComponentLocation());
}

void UStaticReverbShiftComponent::SetNewAudio(UMetaSoundSource* NewSoundToPlay) {
	if (SoundToPlay == NULL) {
		UE_LOG(LogTemp, Error, TEXT("Invalid sound file."));
		return;
	}
	SoundToPlay = NewSoundToPlay;
}

void UStaticReverbShiftComponent::SetNewRoomSelection(EReverbSelection NewReverb) {
	ReverbSelection = NewReverb;
}

void UStaticReverbShiftComponent::SetAudioProperties() {
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
	if (SoundToPlay != NULL) {

		if (SoundToPlay->AttenuationSettings != NULL) {
			SoundToPlay->AttenuationSettings = DefaultAttenuation;
		}
	} else {
		UE_LOG(LogTemp, Warning, TEXT("MetaSound does not exist."));
	}
}

void UStaticReverbShiftComponent::SetImpulse() {
	IrToApply = NewObject<USourceEffectConvolutionReverbPreset>(this, TEXT("IrToApply"));
	SelectedRIR = IRLoader->GetIR(ReverbSelection, GetOwner()->GetName());
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

void UStaticReverbShiftComponent::ResetImpulse() {
	IrToApply = NewObject<USourceEffectConvolutionReverbPreset>(this, TEXT("IrToApply"));
	IrToApply->SetImpulseResponse(IRLoader->GetIR(EReverbSelection::RS_None, GetOwner()->GetName()));
	IrToApply->Settings.WetVolumeDb = -96.0f;
	IrToApply->Settings.DryVolumeDb = 0.0f;
	IrToApply->bEnableHardwareAcceleration = false;
	IrToApply->BlockSize = ESubmixEffectConvolutionReverbBlockSize::BlockSize1024;

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

void UStaticReverbShiftComponent::PlayAudio() {
	if (SoundToPlay == NULL) {
		UE_LOG(LogTemp, Error, TEXT("Invalid sound file."));
		return;
	}

	AudioComponent->SetVolumeMultiplier(VolumeMultiplier);
	AudioComponent->SetPitchMultiplier(PitchMultiplier);
	AudioComponent->SetSound(SoundToPlay);

	AudioComponent->Play();
}

void UStaticReverbShiftComponent::StopAudio() {
	if (SoundToPlay == NULL) {
		UE_LOG(LogTemp, Error, TEXT("Invalid sound file."));
		return;
	}
	AudioComponent->Stop();
}