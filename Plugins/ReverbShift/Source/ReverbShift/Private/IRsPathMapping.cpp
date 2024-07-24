#include "IRsPathMapping.h"

FString UIRsPathMapping::GetIRPath(ERoomSelection RoomSelection) {
	switch (RoomSelection) {
	case ERoomSelection::RSE_None:
		return TEXT("/ReverbShift/RIRs/Clear_IR.Clear_IR");
		break;
	case ERoomSelection::RSE_Arena:
		return TEXT("/ReverbShift/RIRs/Arena_IR.Arena_IR");
		break;
	case ERoomSelection::RSE_CarPark:
		return TEXT("/ReverbShift/RIRs/Car_Park_IR.Car_Park_IR");
		break;
	case ERoomSelection::RSE_Cavern:
		return TEXT("/ReverbShift/RIRs/Cavern_IR.Cavern_IR");
		break;
	case ERoomSelection::RSE_Cinema:
		return TEXT("/ReverbShift/RIRs/Cinema_IR.Cinema_IR");
		break;
	case ERoomSelection::RSE_Hillside:
		return TEXT("/ReverbShift/RIRs/Hillside_IR.Hillside_IR");
		break;
	case ERoomSelection::RSE_Stone_Quarry:
		return TEXT("/ReverbShift/RIRs/Stone_Quarry_IR.Stone_Quarry_IR");
		break;
	default:
		return TEXT("woops");
		break;
	}
}