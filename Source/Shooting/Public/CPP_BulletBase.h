// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_BulletBase.generated.h"

UCLASS()
class SHOOTING_API ACPP_BulletBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACPP_BulletBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Public 변수 */
public:
	/** Component 변수들 선언 */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bullet|Comp")
	UStaticMeshComponent* BulletMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bullet|Comp")
	class UCPP_BaseMovementComponent* MovementComp;

	/** Public 함수 */
public:
	// 총알 발사
	UFUNCTION(BlueprintCallable, Category = "Bullet|State")
	virtual void FireBullet(FVector firePos, FVector direction);

	// 총알의 Visibility와 Collision 활성화 상태 설정
	UFUNCTION(BlueprintCallable, Category = "Bullet|State")
	virtual void SetActivationBullet(bool isActive);

	/** Protected 변수 */
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Bullet|State")
	bool bIsFire = false;

	FTimerHandle HiddenBulletTimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bullet|Option")
	bool bIsAutoHidden = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bullet|Option")
	float BulletHiddenTime = 5.0f;

	/** Protected 함수 */
protected:
	// 충돌했을 때 처리
	UFUNCTION()
	virtual void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Bullet|State")
	void AutoHiddenBullet();
};
