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

	// Public ����
public:
	/** Component ������ ���� */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Comp")
	class USphereComponent* SphereCollision;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Comp")
	class UStaticMeshComponent* BodyMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Comp")
	class USceneComponent* FirePosition;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Comp")
	class UCPP_BaseMovementComponent* MovementComp;

	// Public �Լ�
public:
	UFUNCTION(BlueprintPure, Category = "Enemy|Action")
	bool IsEnemyDead() { return IsHidden(); };

	// �� ���
	UFUNCTION(BlueprintCallable, Category = "Enemy|Action")
	void DeadEnemy();

	// ���� �÷��̾ �ε����� �÷��̾� ���
	UFUNCTION(BlueprintCallable, Category = "Enemy|Action")
	void PlayerKill();
	
	// �� ���� ����
	UFUNCTION(BlueprintCallable, Category = "Enemy|Action")
	void SetHiddenEnemy(bool hidden);

	// �� óġ���� �� ȹ�� ���� ��ȯ
	UFUNCTION(BlueprintPure, Category = "Enemy|Option")
	int32 GetEnemyScore() const { return EnemyScore; };

	// �� �����ϱ�
	void SpawnEnemy(FVector location, FRotator rotation);

	// �� ���� �� �ʱⰪ ����
	void InitializeEnemy(float speed, ECPP_EnemyType enemyType);

	// Protected ����
protected:
	// ������ �� ȸ�� ���� �ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Option")
	float RotateInterpSpeed = 3.0f;

	// ���� ���� �ð�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Option")
	float RefreshSearchingTime = 1.0f;

	// �� Ÿ��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Option")
	ECPP_EnemyType ThisEnemyType = ECPP_EnemyType::ET_Base;

	UPROPERTY(BlueprintReadOnly, BlueprintReadOnly, Category = "Enemy|Option")
	FRotator TargetRotation = FRotator(0.0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Option")
	USoundBase* ExplosionSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Option")
	UParticleSystem* ExplosionParticle;

	// ���� źâ �뷮 (�޸� Ǯ�� �ִ밪)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Bullet")
	int32 MaxMagazineBullets = 5;

	// ���� ����� �Ѿ� ���� Ŭ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Bullet")
	TSubclassOf<class ACPP_BulletBase> BulletClass = NULL;

	// �Ѿ� Ǯ���� źâ
	UPROPERTY(BlueprintReadOnly, Category = "Enemy|Bullet")
	TArray<class ACPP_BulletBase*> BulletMagazine;

	// �Ѿ� �߻� Index
	UPROPERTY(BlueprintReadOnly, Category = "Enemy|Bullet")
	int32 PoolBulletIndex = 0;

	// ���� �߻��� �� ��ٸ��� �ð�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Option")
	FVector2D RandomFireDelay = FVector2D(2.0, 4.0);

	UPROPERTY()
	AActor* TargetPlayer = NULL;

	UPROPERTY()
	FVector PlayerTargetPosition = FVector(0.0);

	int32 EnemyScore = 0;
	float FireTimer = 0.0f;
	float NextFireDelayTime = 0.0f;

	// Protected �Լ�
protected:
	// �浹 ����
	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// ���� Ÿ�ٿ� ���� �ش� ���� �̵�
	void TrackingTargetPoint(float deltaTime);

	// �÷��̾� Ž�� �� ȸ�� ����
	void SearchingPlayer();

	void AutoFiring(float deltaTime);

	// �Ѿ� �����ϱ�(�Ѿ� ���� Ǯ��)
	void LockAndLoad();

	// �Ѿ� �߻�
	void FireAction();
};
