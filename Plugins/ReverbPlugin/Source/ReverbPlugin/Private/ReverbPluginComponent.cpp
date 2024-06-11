// Copyright Epic Games, Inc. All Rights Reserved.
#include "ReverbPluginComponent.h"
#include "Kismet/GameplayStatics.h"

UReverbPluginComponent::UReverbPluginComponent() {
    PrimaryComponentTick.bCanEverTick = false;
}

void UReverbPluginComponent::BeginPlay() {
    Super::BeginPlay();
}

void UReverbPluginComponent::PlayAudio() {
    if(SoundToPlay) {
        UGameplayStatics::PlaySoundAtLocation(this, SoundToPlay, GetOwner()->GetActorLocation());
    }
}
