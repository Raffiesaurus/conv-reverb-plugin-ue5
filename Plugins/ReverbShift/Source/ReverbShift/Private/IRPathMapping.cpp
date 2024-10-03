#include "IRPathMapping.h"

FString UIRsPathMapping::GetIRPath(EReverbSelection ReverbSelection) {
	switch (ReverbSelection) {
	case EReverbSelection::RS_None:
		return TEXT("/ReverbShift/IRs/Clear_IR.Clear_IR");
		break;
	case EReverbSelection::RS_Arena:
		return TEXT("/ReverbShift/IRs/Arena_IR.Arena_IR");
		break;
	case EReverbSelection::RS_CarPark:
		return TEXT("/ReverbShift/IRs/Car_Park_IR.Car_Park_IR");
		break;
	case EReverbSelection::RS_Cavern:
		return TEXT("/ReverbShift/IRs/Cavern_IR.Cavern_IR");
		break;
	case EReverbSelection::RS_Cinema:
		return TEXT("/ReverbShift/IRs/Cinema_Room_IR.Cinema_Room_IR");
		break;
	case EReverbSelection::RS_Hillside:
		return TEXT("/ReverbShift/IRs/Hillside_IR.Hillside_IR");
		break;
	case EReverbSelection::RS_Stone_Quarry:
		return TEXT("/ReverbShift/IRs/Stone_Quarry_IR.Stone_Quarry_IR");
		break;
	default:
		return TEXT("Invalid Reverb Selected.");
		break;
	}
}