#pragma once

#include "CoreMinimal.h"
#include "Enums.h"
#include "IRPathMapping.generated.h"

UCLASS()
class REVERBSHIFT_API UIRsPathMapping : public UObject {
	GENERATED_BODY()

public:
	static FString GetIRPath(EReverbSelection ReverbSelection);
};