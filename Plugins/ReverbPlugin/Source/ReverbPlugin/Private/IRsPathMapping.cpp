#include "IRsPathMapping.h"

FString UIRsPathMapping::GetIRPath(ERoomSelection RoomSelection) {
	switch(RoomSelection) {
	case ERoomSelection::RSE_Arena:
		return TEXT("/ReverbPlugin/RIRs/Arena_IR.Arena_IR");
	case ERoomSelection::RSE_CarPark:
		return TEXT("/ReverbPlugin/RIRs/Car_Park_IR.Car_Park_IR");
	case ERoomSelection::RSE_Cavern:
		return TEXT("/ReverbPlugin/RIRs/Cavern_IR.Cavern_IR");
	case ERoomSelection::RSE_Cinema:
		return TEXT("/ReverbPlugin/RIRs/Cinema_IR.Cinema_IR");
	case ERoomSelection::RSE_Hillside:
		return TEXT("/ReverbPlugin/RIRs/Hillside_IR.Hillside_IR");
	default:
		return TEXT("woops");
	}
}