#include "IRsPathMapping.h"

FString UIRsPathMapping::GetIRPath(ERoomSelection RoomSelection) {
	switch(RoomSelection) {
	case ERoomSelection::RSE_Arena:
		return TEXT("/ReverbPlugin/RIRs/Arena.Arena");
	case ERoomSelection::RSE_CarPark:
		return TEXT("/ReverbPlugin/RIRs/Car_Park.Car_Park");
	case ERoomSelection::RSE_Cavern:
		return TEXT("/ReverbPlugin/RIRs/Cavern.Cavern");
	case ERoomSelection::RSE_Cinema:
		return TEXT("/ReverbPlugin/RIRs/Cinema.Cinema");
	case ERoomSelection::RSE_Hillside:
		return TEXT("/ReverbPlugin/RIRs/Hillside.Hillside");
	default:
		return TEXT("woops");
	}
}