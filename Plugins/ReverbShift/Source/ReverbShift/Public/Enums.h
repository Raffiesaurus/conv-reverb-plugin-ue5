#pragma once

#include "CoreMinimal.h"
#include "Enums.generated.h"

UENUM(BlueprintType)
enum class ERoomSelection : uint8 {
	RSE_None				UMETA(DisplayName = "None"),
	RSE_Cinema				UMETA(DisplayName = "Cinema"),
	RSE_CarPark				UMETA(DisplayName = "Car Park"),
	RSE_Arena				UMETA(DisplayName = "Arena"),
	RSE_Hillside			UMETA(DisplayName = "Hillside"),
	RSE_Cavern				UMETA(DisplayName = "Cavern"),
	RSE_Stone_Quarry		UMETA(DisplayName = "Stone Quarry"),
};