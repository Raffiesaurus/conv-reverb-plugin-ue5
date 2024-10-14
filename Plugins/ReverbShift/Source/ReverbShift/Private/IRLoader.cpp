// Copyright Raahul Sankaranarayanan, 2024. All Rights Reserved.
#include "IRLoader.h"

UAudioImpulseResponse* UIRLoader::GetIR(EReverbSelection ReverbSelection, FString owner) {

	FString Path = UIRsPathMapping::GetIRPath(ReverbSelection);
	UAudioImpulseResponse* IR = Cast<UAudioImpulseResponse>(StaticLoadObject(UAudioImpulseResponse::StaticClass(), nullptr, *Path));

	if (!IR) {
		UE_LOG(LogTemp, Error, TEXT("Failed to load sound from path: %s"), *Path);
	} else {
		UE_LOG(LogTemp, Warning, TEXT("Loaded sound from path: %s"), *Path);
	}

	return IR;
}