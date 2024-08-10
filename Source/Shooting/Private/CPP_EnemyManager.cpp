// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_EnemyManager.h"
#include "CPP_Enemy.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

// TArray를 셔플하는 함수
template<typename T>
void ShuffleArray(TArray<T>& Array)
{
	const int32 LastIndex = Array.Num() - 1;
	for (int32 i = 0; i <= LastIndex; ++i)
	{
		int32 Index = FMath::RandRange(i, LastIndex);
		if (i != Index)
		{
			Array.Swap(i, Index);
		}
	}
}

// Sets default values
ACPP_EnemyManager::ACPP_EnemyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootTransform = CreateDefaultSubobject<USceneComponent>(TEXT("RootTransform"));
	RootComponent = RootTransform;
}

// Called when the game starts or when spawned
void ACPP_EnemyManager::BeginPlay()
{
	Super::BeginPlay();
	EnemyPooling();
	if (bIsAutoGameStart)
		GameStart();
}

// Called every frame
void ACPP_EnemyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACPP_EnemyManager::GameStart()
{
	if (bWasStartedGame)
	{
		UE_LOG(LogTemp, Warning, TEXT("Already started the game"));
	}
	else
	{
		bWasStartedGame = true;
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ACPP_EnemyManager::ShowEnemy, SpawnTimer, true);
	}
}

void ACPP_EnemyManager::EnemyPooling()
{
	EnemyPoolStack.Reserve(EnemyPoolingCount);
	ACPP_Enemy* tempEnemy = NULL;
	TrackerRatio = FMath::Clamp(TrackerRatio, 0.0f, 1.0f);
	int32 trackerCount = EnemyPoolingCount * TrackerRatio;

	for (int i = 0; i < EnemyPoolingCount; ++i)
	{
		tempEnemy = GetWorld()->SpawnActor<ACPP_Enemy>(EnemyClass);
		tempEnemy->SetOwner(this);
		tempEnemy->InitializeEnemy(FMath::FRandRange(RandomSpeedRange.X, RandomSpeedRange.Y), (i < trackerCount) ? ECPP_EnemyType::ET_Finder : ECPP_EnemyType::ET_Base);
		EnemyPoolStack.Add(tempEnemy);
	}

	ShuffleArray(EnemyPoolStack);
}

void ACPP_EnemyManager::ShowEnemy()
{
	FVector Position = GetActorLocation();
	FRotator defaultRot(0.0, 0.0, 180.0);
	Position.Y = FMath::FRandRange(RandomPositionYRange.X, RandomPositionYRange.Y);
	
	while (true)
	{
		if (EnemyPoolStack[EnemyPoolIndex]->IsEnemyDead())
		{
			EnemyPoolStack[EnemyPoolIndex]->SpawnEnemy(Position, defaultRot);
			EnemyPoolIndex = (EnemyPoolIndex + 1) % EnemyPoolingCount;
			break;
		}
		EnemyPoolIndex = (EnemyPoolIndex + 1) % EnemyPoolingCount;
	}

	++CurrentSpawnCount;
	if (CurrentSpawnCount >= EnemySpawnMaxCount)
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
}

void ACPP_EnemyManager::ReportByEnemyToHide()
{
	++EnemyHideCount;
	if(EnemyHideCount >= EnemySpawnMaxCount)
		OnFinishedGame.Broadcast();
}

void ACPP_EnemyManager::EndGame()
{
	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
	OnGameOver.Broadcast();
}
