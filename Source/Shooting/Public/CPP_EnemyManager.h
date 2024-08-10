// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_EnemyManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinishedGameDelegate);

UCLASS()
class SHOOTING_API ACPP_EnemyManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACPP_EnemyManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Public ����*/
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Manager|Comp")
	class USceneComponent* RootTransform;
	
	// ���� ��� �׾ ������ �����ٸ�
	UPROPERTY(BlueprintAssignable)
	FOnFinishedGameDelegate OnFinishedGame;

	UPROPERTY(BlueprintAssignable)
	FOnFinishedGameDelegate OnGameOver;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Manager|Option")
	bool bIsAutoGameStart = false;

	// �� ���� �ð�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Manager|Option")
	float SpawnTimer = 1.0f;

	// �� Ǯ�� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Manager|Option")
	int32 EnemyPoolingCount = 30;

	// ������ ���� �ִ� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Manager|Option")
	int32 EnemySpawnMaxCount = 60;

	// ����� ��(�÷��̾ ���� �װų� ������ ���� �� ����)
	UPROPERTY(BlueprintReadOnly, Category = "Manager|Option")
	int32 EnemyHideCount = 0;

	// ������� �� ���� ����
	UPROPERTY(BlueprintReadOnly, Category = "Manager|Option")
	int32 CurrentSpawnCount = 0;

	// �����ϴ� �� ���� ���� (0.0 ~ 1.0)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Manager|Option")
	float TrackerRatio = 0.2f;

	// Ǯ���� �� ���͸� ������ �迭
	UPROPERTY(BlueprintReadOnly, Category = "Manager|Option")
	TArray<class ACPP_Enemy*> EnemyPoolStack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Manager|Option")
	TSubclassOf<class ACPP_Enemy> EnemyClass;

	// ���� ���� �����ϴ� �¿� ��ġ ������. 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Manager|Option")
	FVector2D RandomPositionYRange = FVector2D(-500.0, 500.0);

	// �� ���� �� ������ ���� �ӵ��� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Manager|Option")
	FVector2D RandomSpeedRange = FVector2D(150.0, 300.0);

	int32 EnemyPoolIndex = 0;

	/** Public �Լ� */
public:
	UFUNCTION(BlueprintCallable, Category = "Manager|Action")
	void GameStart();

	// ���� ������ �Ŵ������� ������
	UFUNCTION(BlueprintCallable, Category = "Manager|Option")
	void ReportByEnemyToHide();

	// �÷��̾ ����ϸ� ���� ����
	void EndGame();

	/** Protected �Լ� */
protected:
	// ���� �޸� Ǯ��
	void EnemyPooling();

	// �� �����Ű��
	UFUNCTION()
	void ShowEnemy();

	/** Protected ���� */
protected:
	FTimerHandle SpawnTimerHandle;
	bool bWasStartedGame = false;
};
