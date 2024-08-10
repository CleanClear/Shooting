// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CPP_MyPlayerPawn.generated.h"

UCLASS()
class SHOOTING_API ACPP_MyPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACPP_MyPlayerPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	/** Public 변수 */
public:
	/** Component 변수들 선언 */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Comp")
	UStaticMeshComponent* BodyMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Comp")
	class USphereComponent* SphereCollision;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Comp")
	UStaticMeshComponent* ShieldMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Comp")
	USceneComponent* TiltPoint;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Comp")
	USceneComponent* FirePosition;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Comp")
	class UCPP_BaseMovementComponent* MovementComponent;

	/** Public 함수 */
public:
	// 플레이어의 사망상태 반환
	UFUNCTION(BlueprintPure, Category = "Player|State")
	bool IsPlayerDead() const { return bIsDead; };

	UFUNCTION(BlueprintPure, Category = "Player|State")
	bool IsBoosting() const { return bIsBoost; };

	// 플레이어 숨김 설정
	UFUNCTION(BlueprintCallable, Category = "Player|State")
	void SetHiddenPlayer(bool hidden);

	// 플레이어 사망 설정
	UFUNCTION(BlueprintCallable, Category = "Player|State")
	void PlayerDead();

	UFUNCTION(BlueprintCallable, Category = "Player|Option")
	void UpdateScore(int32 score);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "UpdateScore"))
	void ReceiveUpdateScore(int32 score);

	// 플레이어가 데미지를 입고 나서 사망여부를 반환
	bool HitDamage(AActor* instigator);
	
	/** Protected 변수 */
protected:
	// 현재 움직이려는 방향
	UPROPERTY(BlueprintReadOnly, Category = "Player|Movement")
	FVector CurrentMoveDirection = FVector(0.0);

	// 좌우 이동 시 기체를 기울일 때 최대 각도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Movement")
	double HorizontalTiltRotDegreesMax = 25.0f;

	// 기체를 기울일 때 보간 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Movement")
	double TiltingInterpSpeed = 3.0f;

	// 플레이어의 탄창 용량 (메모리 풀링 최대값)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Bullet")
	int32 MaxMagazineBullets = 50;

	/* 탄창에 집어넣을 총알 액터
	 * TSubclassOf는 해당 타입의 클래스와 그 하위 상속된 클래스들을 의미
	 * 즉, 해당 UClass 타입 변수
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Bullet")
	TSubclassOf<class ACPP_BulletBase> BulletClass = NULL;

	// 총알 풀링된 탄창
	UPROPERTY(BlueprintReadOnly, Category = "Player|Bullet")
	TArray<class ACPP_BulletBase*> BulletMagazine;

	// 총알 발사 Index
	UPROPERTY(BlueprintReadOnly, Category = "Player|Bullet")
	int32 PoolBulletIndex = 0;
	
	// 총알을 막아주는 쉴드 에너지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Option")
	int32 ShieldBatteryMax = 3;

	// 연사 모드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Option")
	bool bAutoFireMode = false;

	// 연사 모드 시 딜레이
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Option")
	float AutoFireDelay = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Option")
	USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Option")
	USoundBase* ExplosionSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Option")
	UParticleSystem* ExplosionParticle;

	// 월드 EnemyManager 레퍼런스 변수
	UPROPERTY(BlueprintReadOnly, Category = "Player|Option")
	class ACPP_EnemyManager* EnemyManager = NULL;

	UPROPERTY(BlueprintReadOnly, Category = "Player|Option")
	int32 TotalScore = 0;

	// 현재 사격 중인 상태여부
	UPROPERTY(BlueprintReadOnly, Category = "Player|State")
	bool bIsFiring = false;

	// 기체 기울기 보간 시 타겟 각도
	double TargetTiltRotDegrees = 0.0f;
	bool bIsDead = false;
	bool bIsBoost = false;
	FTimerHandle AutoFireTimerHandle;
	int32 CurrentShieldBattery = 0;

	// Protected 함수
protected:
	// 총알 장전하기(총알 액터 풀링)
	void LockAndLoad();

	// 총알 발사
	void FireAction();

	/** 기체 좌우 Tilt 관련 함수들 */

	void SetTiltingHorizontal(double axis);
	void UpdateTiltingFlight(float deltaTime);

	// Private 함수
private:
	/** 입력에 따른 함수 호출 */ 

	void Horizontal(float value);
	void Vertical(float value);
	void Boost(bool isBoosting);
	void Fire(bool isFiring);

	// 게임 클리어
	UFUNCTION()
	void HandleFinishedGame();
};
