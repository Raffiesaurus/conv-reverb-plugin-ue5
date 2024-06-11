// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Sound/SoundBase.h"
#include "ReverbPluginComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REVERBPLUGIN_API UReverbPluginComponent : public UActorComponent {
    GENERATED_BODY()

public:
    UReverbPluginComponent();

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundBase* SoundToPlay;

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlayAudio();
};
