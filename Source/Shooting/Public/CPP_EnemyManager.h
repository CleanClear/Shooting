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

	/** Public 변수*/
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Manager|Comp")
	class USceneComponent* RootTransform;
	
	// 적이 모두 죽어서 게임이 끝났다면
	UPROPERTY(BlueprintAssignable)
	FOnFinishedGameDelegate OnFinishedGame;

	UPROPERTY(BlueprintAssignable)
	FOnFinishedGameDelegate OnGameOver;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Manager|Option")
	bool bIsAutoGameStart = false;

	// 적 등장 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Manager|Option")
	float SpawnTimer = 1.0f;

	// 적 풀링 숫자
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Manager|Option")
	int32 EnemyPoolingCount = 30;

	// 등장할 적의 최대 숫자
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Manager|Option")
	int32 EnemySpawnMaxCount = 60;

	// 사라진 적(플레이어에 의해 죽거나 밖으로 나간 적 숫자)
	UPROPERTY(BlueprintReadOnly, Category = "Manager|Option")
	int32 EnemyHideCount = 0;

	// 현재까지 적 생성 숫자
	UPROPERTY(BlueprintReadOnly, Category = "Manager|Option")
	int32 CurrentSpawnCount = 0;

	// 추적하는 적 숫자 비율 (0.0 ~ 1.0)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Manager|Option")
	float TrackerRatio = 0.2f;

	// 풀링한 적 액터를 보관할 배열
	UPROPERTY(BlueprintReadOnly, Category = "Manager|Option")
	TArray<class ACPP_Enemy*> EnemyPoolStack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Manager|Option")
	TSubclassOf<class ACPP_Enemy> EnemyClass;

	// 적이 랜덤 등장하는 좌우 위치 범위값. 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Manager|Option")
	FVector2D RandomPositionYRange = FVector2D(-500.0, 500.0);

	// 적 등장 시 설정할 랜덤 속도값 범위
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Manager|Option")
	FVector2D RandomSpeedRange = FVector2D(150.0, 300.0);

	int32 EnemyPoolIndex = 0;

	/** Public 함수 */
public:
	UFUNCTION(BlueprintCallable, Category = "Manager|Action")
	void GameStart();

	// 적이 죽으면 매니저에게 보고함
	UFUNCTION(BlueprintCallable, Category = "Manager|Option")
	void ReportByEnemyToHide();

	// 플레이어가 사망하면 게임 종료
	void EndGame();

	/** Protected 함수 */
protected:
	// 적들 메모리 풀링
	void EnemyPooling();

	// 적 등장시키기
	UFUNCTION()
	void ShowEnemy();

	/** Protected 변수 */
protected:
	FTimerHandle SpawnTimerHandle;
	bool bWasStartedGame = false;
};
