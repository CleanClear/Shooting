// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Enemy.h"
#include "Components/SphereComponent.h"
#include "CPP_BaseMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "CPP_MyPlayerPawn.h"
#include "CPP_EnemyManager.h"
#include "CPP_BulletBase.h"
#include "Particles/ParticleSystem.h"

// Sets default values
ACPP_Enemy::ACPP_Enemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// CreateDefaultSubobject() �Լ��� ���� ������Ʈ ����
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	FirePosition = CreateDefaultSubobject<USceneComponent>(TEXT("FirePosition"));
	MovementComp = CreateDefaultSubobject<UCPP_BaseMovementComponent>(TEXT("MovementComp"));

	// ������Ʈ ����
	RootComponent = SphereCollision;
	BodyMesh->SetupAttachment(RootComponent);
	FirePosition->SetupAttachment(RootComponent);

	BodyMesh->SetRelativeLocationAndRotation(
		FVector(-75.0, 0.0, 0.0),
		FRotator(0.0, 90.0, -90.0)
	);
	BodyMesh->SetRelativeScale3D(FVector(1.5));
	BodyMesh->SetCollisionProfileName(TEXT("NoCollision"), false);
	BodyMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	FirePosition->SetRelativeLocation(FVector(0.0, 0.0, 10.0));

	SphereCollision->SetCollisionProfileName(TEXT("EnemyBlock"));
	SphereCollision->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	SphereCollision->OnComponentHit.AddDynamic(this, &ACPP_Enemy::OnComponentHit);
	SphereCollision->SetSphereRadius(80.0f);

	// ����� ����ƽ �޽� ������ ���� �Ҵ�
	// ������Ʈ ���� ���� ���� ���۷��� ����(�׳� ��Ʈ��+C) �ؼ� �Ʒ�ó�� ��� �ֱ�
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("StaticMesh'/Game/Drone/Drone_low.Drone_low'"));
	ConstructorHelpers::FObjectFinder<USoundBase> explosionSoundObj(TEXT("SoundCue'/Game/Audio/Cue_BoomEnemy.Cue_BoomEnemy'"));
	ConstructorHelpers::FObjectFinder<UParticleSystem> explosionParticleObj(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Explosion.P_Explosion'"));

	// �ε� ���� �� �⺻�� ����
	if (tempMesh.Succeeded())
		BodyMesh->SetStaticMesh(tempMesh.Object);

	if (explosionSoundObj.Succeeded())
		ExplosionSound = explosionSoundObj.Object;

	if (explosionParticleObj.Succeeded())
		ExplosionParticle = explosionParticleObj.Object;
}

// Called when the game starts or when spawned
void ACPP_Enemy::BeginPlay()
{
	Super::BeginPlay();
	SetHiddenEnemy(true);
	LockAndLoad();
}

// Called every frame
void ACPP_Enemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsHidden() && IsValid(TargetPlayer) && !TargetPlayer->IsHidden())
	{
		// �̵� �� ȸ��
		if(ThisEnemyType == ECPP_EnemyType::ET_Finder)
			SearchingPlayer();
		TrackingTargetPoint(DeltaTime);

		// �ڵ� ���
		AutoFiring(DeltaTime);
	}
}

void ACPP_Enemy::DeadEnemy()
{
	SetHiddenEnemy(true);
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticle, GetActorLocation());

	Cast<ACPP_EnemyManager>(GetOwner())->ReportByEnemyToHide();
}

void ACPP_Enemy::PlayerKill()
{
	Cast<ACPP_EnemyManager>(GetOwner())->EndGame();
}

void ACPP_Enemy::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ACPP_MyPlayerPawn* player = Cast<ACPP_MyPlayerPawn>(OtherActor);
	if (IsValid(player) && player->HitDamage(this))
		PlayerKill();
}

void ACPP_Enemy::TrackingTargetPoint(float deltaTime)
{
	if (Cast<ACPP_MyPlayerPawn>(TargetPlayer)->IsPlayerDead())
	{
		// �÷��̾� ��� �� ����
		MovementComp->SetDirectionToMove(FVector(0.0));
	}
	else
	{
		if (ThisEnemyType == ECPP_EnemyType::ET_Finder)
		{
			// ���� ���Ͱ� ������ ��
			if (GetActorLocation().Z > -250.0)
			{
				FVector direction = PlayerTargetPosition - GetActorLocation();
				direction.Normalize();
				MovementComp->SetDirectionToMove(direction);
			}
			else
			{
				// �Ʒ��� �������ٰ� ���� �Ѿ�� �Ϲ� ����ó�� �Ʒ��θ� �������� ����
				ThisEnemyType = ECPP_EnemyType::ET_Base;
				TargetRotation = FRotator(0.0, 0.0, 180.0);
			}
		}
		else
		{
			// �⺻ ���ʹ� �׳� �Ʒ��� ������
			MovementComp->SetDirectionToMove(FVector(0.0, 0.0, -1.0));
		}
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, deltaTime, RotateInterpSpeed));
	}
}

void ACPP_Enemy::SpawnEnemy(FVector location, FRotator rotation)
{
	TargetPlayer = UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_MyPlayerPawn::StaticClass());

	if (IsValid(TargetPlayer))
	{
		SetActorLocationAndRotation(location, rotation);
		TargetRotation = GetActorRotation();
		SetHiddenEnemy(false);
	}
}

void ACPP_Enemy::SearchingPlayer()
{
	PlayerTargetPosition = TargetPlayer->GetActorLocation();
		
	// �ٶ� ���� ���ϱ�
	FVector tempVt = PlayerTargetPosition - GetActorLocation();
	FVector targetDirection = FVector(
		0.0,
		tempVt.Y,
		tempVt.Z
	);
	tempVt.Normalize();

	// Rotation �� ���ϱ�. X�� ���� Z���� �ٶ� �������� ����
	TargetRotation = UKismetMathLibrary::MakeRotFromXZ(GetActorForwardVector(), targetDirection);
}

void ACPP_Enemy::AutoFiring(float deltaTime)
{
	FireTimer += deltaTime;
	if (FireTimer >= NextFireDelayTime)
	{
		FireTimer = 0.0f;
		FireAction();
		NextFireDelayTime = FMath::FRandRange(RandomFireDelay.X, RandomFireDelay.Y);
	}
}

void ACPP_Enemy::LockAndLoad()
{
	if (!IsValid(BulletClass))
	{
		UE_LOG(LogTemp, Error, TEXT("Not Set BulletClass"));
		return;
	}

	BulletMagazine.Reserve(MaxMagazineBullets);
	for (int i = 0; i < MaxMagazineBullets; ++i)
	{
		BulletMagazine.Add(Cast<ACPP_BulletBase>(GetWorld()->SpawnActor(BulletClass)));
		BulletMagazine[i]->Owner = this;
	}
	NextFireDelayTime = FMath::FRandRange(RandomFireDelay.X, RandomFireDelay.Y);
}

void ACPP_Enemy::FireAction()
{
	if (!BulletMagazine.IsValidIndex(0))
	{
		UE_LOG(LogTemp, Error, TEXT("Empty Magazine"));
		return;
	}

	BulletMagazine[PoolBulletIndex]->FireBullet(FirePosition->GetComponentLocation(), GetActorUpVector());
	PoolBulletIndex = (PoolBulletIndex + 1) % MaxMagazineBullets;
}

void ACPP_Enemy::SetHiddenEnemy(bool hidden)
{
	SetActorHiddenInGame(hidden);
	SphereCollision->SetCollisionEnabled(
		(hidden) ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics
	);
}

void ACPP_Enemy::InitializeEnemy(float speed, ECPP_EnemyType enemyType)
{
	MovementComp->MoveSpeed = speed;
	ThisEnemyType = enemyType;
	EnemyScore = (enemyType == ECPP_EnemyType::ET_Base) ? 10 : 30;
}

