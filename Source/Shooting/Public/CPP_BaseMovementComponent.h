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

	/** Public 함수 */
public:
	// 현재 속도값
	UFUNCTION(BlueprintPure, Category="Movement")
	FVector GetVelocity() { return Velocity; }

	// 이동 방향 설정 및 Speed와 곱하여 Velocity 설정. 방향이 없으면 이동하지 않음.
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetDirectionToMove(FVector direction);

	/** Public 변수 */
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Option")
	float MoveSpeed = 100.0f;

	// 이동 시 충돌 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Option")
	bool UseBlockingCollision = false;

protected:
	// 실시간 이동
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void UpdateMovement(float DeltaTime);

	UPROPERTY(BlueprintReadOnly, Category = "Movement|Option")
	FVector Velocity;

};
