// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_BaseMovementComponent.h"

// Sets default values for this component's properties
UCPP_BaseMovementComponent::UCPP_BaseMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCPP_BaseMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCPP_BaseMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateMovement(DeltaTime);
	// ...
}

void UCPP_BaseMovementComponent::SetDirectionToMove(FVector direction)
{
	Velocity = direction * MoveSpeed;
}

void UCPP_BaseMovementComponent::UpdateMovement(float DeltaTime)
{
	GetOwner()->AddActorWorldOffset(DeltaTime * Velocity, UseBlockingCollision);
}

