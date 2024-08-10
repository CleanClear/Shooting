// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_Enemy.generated.h"

UENUM(BlueprintType)
enum class ECPP_EnemyType : uint8
{
	ET_Base UMETA(DisplayName = "Base"),
	ET_Finder UMETA(DisplayName = "Tracker")
};

UCLASS()
class SHOOTING_API ACPP_Enemy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACPP_Enemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Public 변수
public:
	/** Component 변수들 선언 */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Comp")
	class USphereComponent* SphereCollision;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Comp")
	class UStaticMeshComponent* BodyMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Comp")
	class USceneComponent* FirePosition;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Comp")
	class UCPP_BaseMovementComponent* MovementComp;

	// Public 함수
public:
	UFUNCTION(BlueprintPure, Category = "Enemy|Action")
	bool IsEnemyDead() { return IsHidden(); };

	// 적 사망
	UFUNCTION(BlueprintCallable, Category = "Enemy|Action")
	void DeadEnemy();

	// 적과 플레이어가 부딪혀서 플레이어 사망
	UFUNCTION(BlueprintCallable, Category = "Enemy|Action")
	void PlayerKill();
	
	// 적 숨김 설정
	UFUNCTION(BlueprintCallable, Category = "Enemy|Action")
	void SetHiddenEnemy(bool hidden);

	// 적 처치했을 때 획득 점수 반환
	UFUNCTION(BlueprintPure, Category = "Enemy|Option")
	int32 GetEnemyScore() const { return EnemyScore; };

	// 적 등장하기
	void SpawnEnemy(FVector location, FRotator rotation);

	// 적 생성 후 초기값 설정
	void InitializeEnemy(float speed, ECPP_EnemyType enemyType);

	// Protected 변수
protected:
	// 추적할 때 회전 보간 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Option")
	float RotateInterpSpeed = 3.0f;

	// 추적 갱신 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Option")
	float RefreshSearchingTime = 1.0f;

	// 적 타입
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Option")
	ECPP_EnemyType ThisEnemyType = ECPP_EnemyType::ET_Base;

	UPROPERTY(BlueprintReadOnly, BlueprintReadOnly, Category = "Enemy|Option")
	FRotator TargetRotation = FRotator(0.0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Option")
	USoundBase* ExplosionSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Option")
	UParticleSystem* ExplosionParticle;

	// 적의 탄창 용량 (메모리 풀링 최대값)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Bullet")
	int32 MaxMagazineBullets = 5;

	// 적이 사용할 총알 액터 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Bullet")
	TSubclassOf<class ACPP_BulletBase> BulletClass = NULL;

	// 총알 풀링된 탄창
	UPROPERTY(BlueprintReadOnly, Category = "Enemy|Bullet")
	TArray<class ACPP_BulletBase*> BulletMagazine;

	// 총알 발사 Index
	UPROPERTY(BlueprintReadOnly, Category = "Enemy|Bullet")
	int32 PoolBulletIndex = 0;

	// 총을 발사할 때 기다리는 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Option")
	FVector2D RandomFireDelay = FVector2D(2.0, 4.0);

	UPROPERTY()
	AActor* TargetPlayer = NULL;

	UPROPERTY()
	FVector PlayerTargetPosition = FVector(0.0);

	int32 EnemyScore = 0;
	float FireTimer = 0.0f;
	float NextFireDelayTime = 0.0f;

	// Protected 함수
protected:
	// 충돌 판정
	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// 추적 타겟에 따른 해당 방향 이동
	void TrackingTargetPoint(float deltaTime);

	// 플레이어 탐색 및 회전 설정
	void SearchingPlayer();

	void AutoFiring(float deltaTime);

	// 총알 장전하기(총알 액터 풀링)
	void LockAndLoad();

	// 총알 발사
	void FireAction();
};
