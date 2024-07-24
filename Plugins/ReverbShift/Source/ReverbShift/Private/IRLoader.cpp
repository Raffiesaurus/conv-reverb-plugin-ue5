#include "IRLoader.h"

UAudioImpulseResponse* UIRLoader::GetIR(ERoomSelection RoomSelection, FString test) {

	FString Path = UIRsPathMapping::GetIRPath(RoomSelection);
	UAudioImpulseResponse* IR = Cast<UAudioImpulseResponse>(StaticLoadObject(UAudioImpulseResponse::StaticClass(), nullptr, *Path));

	UE_LOG(LogTemp, Warning, TEXT("From: %s"), *test);

	if (!IR) {
		UE_LOG(LogTemp, Warning, TEXT("Failed to load sound from path: %s"), *Path);
	} else {
		UE_LOG(LogTemp, Warning, TEXT("Successfully loaded IR from path: %s"), *Path);
	}

	return IR;
}