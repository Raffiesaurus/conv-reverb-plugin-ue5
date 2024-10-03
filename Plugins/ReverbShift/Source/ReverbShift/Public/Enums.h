#pragma once

#include "CoreMinimal.h"
#include "Enums.generated.h"

UENUM(BlueprintType)
enum class EReverbSelection : uint8 {
	RS_None				UMETA(DisplayName = "None"),
	RS_Cinema			UMETA(DisplayName = "Cinema"),
	RS_CarPark			UMETA(DisplayName = "Car Park"),
	RS_Arena			UMETA(DisplayName = "Arena"),
	RS_Hillside			UMETA(DisplayName = "Hillside"),
	RS_Cavern			UMETA(DisplayName = "Cavern"),
	RS_Stone_Quarry		UMETA(DisplayName = "Stone Quarry"),
};