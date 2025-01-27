// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	UPhysicsHandleComponent *PhysicsHandle = GetPhysicsHandle();

	if(PhysicsHandle && PhysicsHandle-> GetGrabbedComponent() != nullptr)
	{
		FVector TargetLocation = GetComponentLocation() + GetForwardVector() * HoldDistance;
		PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, GetComponentRotation());
	}
	
}

void UGrabber::Release()
{
	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();
	
	if(PhysicsHandle && PhysicsHandle->GetGrabbedComponent() != nullptr)
	{
		PhysicsHandle->GetGrabbedComponent()->GetOwner()->Tags.Remove("Grabbed");
		PhysicsHandle->ReleaseComponent();
	}
}

void UGrabber::Grab()
{
	UPhysicsHandleComponent *PhysicsHandle = GetPhysicsHandle();
	FHitResult HitResult;
	// sweep hits something grabbable
	if (GetGrabbableInReach(HitResult))
	{
		UPrimitiveComponent* HitComponent = HitResult.GetComponent();
		//wake the component
		HitComponent-> WakeAllRigidBodies();
		HitComponent->SetSimulatePhysics(true);
		AActor* HitActor = HitResult.GetActor();
		HitActor->Tags.Add("Grabbed");
		HitActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		//pick up: attach to the Physics Handle
		PhysicsHandle -> GrabComponentAtLocationWithRotation(
			HitComponent,
			NAME_None,
			HitResult.ImpactPoint,
			GetComponentRotation()
			);
	}
}

UPhysicsHandleComponent* UGrabber::GetPhysicsHandle() const
{
	UPhysicsHandleComponent *PhysicsHandle = GetOwner()-> FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle == nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("No Physics Handle exists"));
		return nullptr;
	}

	return PhysicsHandle;
}

bool UGrabber::GetGrabbableInReach(FHitResult& outHit) const
{
	FVector Start = GetComponentLocation();
	FVector End = Start + GetForwardVector() * MaxGrabDistance;
	DrawDebugLine(GetWorld(), Start, End, FColor::Red);

	FCollisionShape Sphere = FCollisionShape::MakeSphere(GrabRadius);
	//save hit component

	// check whether a component was hit by our trace
	return GetWorld()->SweepSingleByChannel(outHit,
		Start, End,
		FQuat::Identity,
		ECC_GameTraceChannel2,
		Sphere
		);
}


