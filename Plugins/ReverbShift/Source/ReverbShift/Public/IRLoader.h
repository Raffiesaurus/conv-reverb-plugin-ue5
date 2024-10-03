#pragma once

#include "CoreMinimal.h"
#include "Enums.h"
#include "IRPathMapping.h"
#include "EffectConvolutionReverb.h"
#include "IRLoader.generated.h"

UCLASS()
class REVERBSHIFT_API UIRLoader : public UObject {
	GENERATED_BODY()

public:
	UAudioImpulseResponse* GetIR(EReverbSelection ReverbSelection, FString test);
};