#include "ShiftingSpecifierModule.h"

UReverbShiftSpecifierComponent::UReverbShiftSpecifierComponent() {

}

void UReverbShiftSpecifierComponent::BeginPlay() {
	Super::BeginPlay();
}

void UReverbShiftSpecifierComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UReverbShiftSpecifierComponent::SetNewRoomSelection(ERoomSelection NewRoom) {
	SpecificReverb = NewRoom;
}