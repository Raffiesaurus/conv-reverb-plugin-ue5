#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundBase.h"
#include "ReverbPluginRoomEnums.h"
#include "IRsPathMapping.generated.h"

UCLASS()
class REVERBPLUGIN_API UIRsPathMapping : public UObject {
	GENERATED_BODY()

public:
	static FString GetIRPath(ERoomSelection RoomSelection);
};
