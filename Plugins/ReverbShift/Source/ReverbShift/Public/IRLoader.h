#pragma once

#include "CoreMinimal.h"
#include "Enums.h"
#include "IRsPathMapping.h"
#include "EffectConvolutionReverb.h"
#include "IRLoader.generated.h"

UCLASS()
class REVERBSHIFT_API UIRLoader : public UObject {
	GENERATED_BODY()

public:
	UAudioImpulseResponse* GetIR(ERoomSelection RoomSelection, FString test);
};