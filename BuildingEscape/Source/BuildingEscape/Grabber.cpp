// Copyright Matthew Pye 2018.

#include "Grabber.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Components/PrimitiveComponent.h"

#define OUT


// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	FindPhysicsHandleComponent();
	SetupInputComponent();
}

/// Look for attached Physics Handle
void UGrabber::FindPhysicsHandleComponent()
{

	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle)
	{
		//Physics Handle is found
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Physics Handle in %s is missing"), *GetOwner()->GetName())
	}
}
/// Look for attached Input Component (Only at run-time)
void UGrabber::SetupInputComponent()
{

	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Input Component found"))
			///Bind the input action
			InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Input Component in %s is missing"), *GetOwner()->GetName())
	}
}

void UGrabber::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab Pressed"))

		//LINE TRACE and see if we reach any actors with physics body collision channel set
		auto HitResult = GetFirstPhysicsBodyInReach();
		auto ComponentToGrab = HitResult.GetComponent();
		auto ActorHit = HitResult.GetActor();

	//If we hit something then attach a physics handle
	if (ActorHit)
	{
		//Attach physics handle
		PhysicsHandle->GrabComponentAtLocationWithRotation(
			ComponentToGrab, //ComponentToGrab
			NAME_None, //grab what bone name, if any
			ComponentToGrab->GetOwner()->GetActorLocation(), //grab location
			ComponentToGrab->GetOwner()->GetActorRotation() //grab rotation	
		);
	}
	

}
void UGrabber::Release()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab Released"))

		//Release physics handle
		PhysicsHandle->ReleaseComponent();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Get the players view point per tick
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation);

	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector()*Reach;

	//if the physics handle is attached
	if(PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocation(LineTraceEnd);
	}
		//move the object everyframe

	
	 
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{

	//Get the players view point per tick
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation);

	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector()*Reach;


	///Setup query paramerters
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());

	///Ray-cast out to reach distance
	FHitResult LineTraceHit;
	GetWorld()->LineTraceSingleByObjectType
	(
		OUT LineTraceHit,
		PlayerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);

	///see what we hit
	AActor* ActorHit = LineTraceHit.GetActor();
	if (ActorHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Line Trace Hit: %s"), *(ActorHit->GetName()))
	}
	return LineTraceHit;
}