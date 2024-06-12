// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ReverbPluginRoomEnums.generated.h"

UENUM(BlueprintType)
enum class ERoomSelection : uint8 {
	RSE_Cinema		UMETA(DisplayName = "Cinema"),
	RSE_CarPark		UMETA(DisplayName = "Car Park"),
	RSE_Arena		UMETA(DisplayName = "Arena"),
	RSE_Hillside    UMETA(DisplayName = "Hillside"),
	RSE_Cavern		UMETA(DisplayName = "Cavern"),
};
