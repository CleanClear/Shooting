// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CPP_BaseMovementComponent.generated.h"


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTING_API UCPP_BaseMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCPP_BaseMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Public �Լ� */
public:
	// ���� �ӵ���
	UFUNCTION(BlueprintPure, Category="Movement")
	FVector GetVelocity() { return Velocity; }

	// �̵� ���� ���� �� Speed�� ���Ͽ� Velocity ����. ������ ������ �̵����� ����.
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetDirectionToMove(FVector direction);

	/** Public ���� */
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Option")
	float MoveSpeed = 100.0f;

	// �̵� �� �浹 ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Option")
	bool UseBlockingCollision = false;

protected:
	// �ǽð� �̵�
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void UpdateMovement(float DeltaTime);

	UPROPERTY(BlueprintReadOnly, Category = "Movement|Option")
	FVector Velocity;

};
