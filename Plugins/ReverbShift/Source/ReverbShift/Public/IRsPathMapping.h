#pragma once

#include "CoreMinimal.h"
#include "Enums.h"
#include "IRsPathMapping.generated.h"

UCLASS()
class REVERBSHIFT_API UIRsPathMapping : public UObject {
	GENERATED_BODY()

public:
	static FString GetIRPath(ERoomSelection RoomSelection);
};