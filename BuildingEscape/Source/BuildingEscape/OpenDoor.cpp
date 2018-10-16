// Copyright Matthew Pye 2018.

#include "OpenDoor.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Components/PrimitiveComponent.h"

#define OUT

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}

// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	Owner = GetOwner();	
	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("Pressure plate in %s is missing"), *GetOwner()->GetName())
	}
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Poll the trigger volume

	//If the ActorThatOpens is in the volume 
	if (GetTotalMassOfActorsOnPlate()> TriggerMass) //TODO make into param
	{
		OnOpen.Broadcast();
	}
	//Check to see if it is time to close the door
	else
	{
		OnClose.Broadcast();
	}	
}

float UOpenDoor::GetTotalMassOfActorsOnPlate()
{
	
	float TotalMass = 0.f;

	//Find Overlapping Actors
	TArray<AActor*> OverlappingActors;
	if (!PressurePlate) { return TotalMass; }
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);

	//Iterate 
	for (const auto* Actor : OverlappingActors)
	{
		TotalMass+= Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();

		UE_LOG(LogTemp, Warning, TEXT("%s is on pressure plate"), *Actor->GetName())

	}
	return TotalMass;

}
