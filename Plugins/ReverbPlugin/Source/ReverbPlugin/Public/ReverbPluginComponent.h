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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundBase* SoundToPlay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundAttenuation* AttenuationSettings;

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlayAudio();

protected:
	virtual void BeginPlay() override;

private:
	UAudioComponent* AudioComponent;

};
