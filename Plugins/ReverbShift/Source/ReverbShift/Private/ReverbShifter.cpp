// Copyright Raahul Sankaranarayanan, 2024. All Rights Reserved.
#include "ReverbShifter.h"

UReverbShifterComponent::UReverbShifterComponent() {}

void UReverbShifterComponent::SetNewReverbSelection(EReverbSelection NewReverb) {
	SpecificReverb = NewReverb;
}

void UReverbShifterComponent::BeginPlay() {
	Super::BeginPlay();
}

void UReverbShifterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
