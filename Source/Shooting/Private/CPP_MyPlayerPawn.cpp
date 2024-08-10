// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_MyPlayerPawn.h"
#include "Components/SphereComponent.h"
#include "CPP_BaseMovementComponent.h"
#include "Kismet/GameplayStatics.h"		// UGamePlayerStatics : 언리얼 C++ 또는 블루프린트에서 유용하게 사용하는 전역 함수들을 모아놓은 클래스
#include "Kismet/KismetMathLibrary.h"	// UKismetMathLibrary : 수학적인 계산과 관련된 유용한 전역 함수들을 모아놓은 클래스. 
#include "CPP_BulletBase.h"
#include "CPP_EnemyManager.h"
#include "CPP_Enemy.h"
#include "Particles/ParticleSystem.h"

// Sets default values
ACPP_MyPlayerPawn::ACPP_MyPlayerPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// CreateDefaultSubobject() 함수를 통해 컴포넌트 생성
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMesh"));
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	FirePosition = CreateDefaultSubobject<USceneComponent>(TEXT("FirePosition"));
	TiltPoint = CreateDefaultSubobject<USceneComponent>(TEXT("TiltPoint"));
	MovementComponent = CreateDefaultSubobject<UCPP_BaseMovementComponent>(TEXT("MovementComponent"));

	// 컴포넌트 설정
	RootComponent = SphereCollision;
	TiltPoint->SetupAttachment(RootComponent);
	ShieldMesh->SetupAttachment(RootComponent);
	BodyMesh->SetupAttachment(TiltPoint);
	FirePosition->SetupAttachment(BodyMesh);

	BodyMesh->SetRelativeRotation(FRotator(0.0, 90.0, 90.0));
	BodyMesh->SetRelativeScale3D(FVector(8.0));
	BodyMesh->SetCollisionProfileName(TEXT("NoCollision"), false);
	BodyMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;

	ShieldMesh->SetRelativeScale3D(FVector(3.0));
	ShieldMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	ShieldMesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	FirePosition->SetRelativeLocation(FVector(0.0, -10.0, 0.0));

	SphereCollision->SetSphereRadius(80.0f);
	SphereCollision->SetGenerateOverlapEvents(true);
	SphereCollision->SetCollisionProfileName(TEXT("Pawn"));
	SphereCollision->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;

	MovementComponent->UseBlockingCollision = true;
	MovementComponent->MoveSpeed = 300.0f;
	CurrentShieldBattery = ShieldBatteryMax;

	// 사용할 스태틱 메쉬 데이터 동적 할당
	// 프로젝트 폴더 내의 파일 레퍼런스 복사(그냥 컨트롤+C) 해서 아래처럼 경로 넣기
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("StaticMesh'/Game/SpaceShip/Spaceship_ARA.Spaceship_ARA'"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> shieldSphere(TEXT("StaticMesh'/Game/StarterContent/Shapes/Sphere.Sphere'"));
	ConstructorHelpers::FObjectFinder<UMaterial> shieldMat(TEXT("Material'/Game/M_Shield.M_Shield'"));
	ConstructorHelpers::FObjectFinder<USoundBase> fireSoundObj(TEXT("SoundCue'/Game/Audio/Cue_FireSound.Cue_FireSound'"));
	ConstructorHelpers::FObjectFinder<USoundBase> boomSoundObj(TEXT("SoundCue'/Game/StarterContent/Audio/Cue_PlayerBoom.Cue_PlayerBoom'"));
	ConstructorHelpers::FObjectFinder<UParticleSystem> explosionParticleObj(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Explosion.P_Explosion'"));

	// 로드 성공 시 기본값 세팅
	if (tempMesh.Succeeded())
		BodyMesh->SetStaticMesh(tempMesh.Object);

	if (shieldSphere.Succeeded())
		ShieldMesh->SetStaticMesh(shieldSphere.Object);

	if (shieldMat.Succeeded())
		ShieldMesh->SetMaterial(0, shieldMat.Object);

	if (fireSoundObj.Succeeded())
		FireSound = fireSoundObj.Object;

	if (boomSoundObj.Succeeded())
		ExplosionSound = boomSoundObj.Object;

	if (explosionParticleObj.Succeeded())
		ExplosionParticle = explosionParticleObj.Object;
}

// Called when the game starts or when spawned
void ACPP_MyPlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	// 게임 시작 초기화
	LockAndLoad();
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EnemyManager = Cast<ACPP_EnemyManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_EnemyManager::StaticClass()));
	if (EnemyManager)
		EnemyManager->OnFinishedGame.AddDynamic(this, &ACPP_MyPlayerPawn::HandleFinishedGame);
	else
		UE_LOG(LogTemp, Error, TEXT("Enemy Manager not Found!"));
}

// Called every frame
void ACPP_MyPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsDead)
	{
		UpdateTiltingFlight(DeltaTime);
	}
}

// Called to bind functionality to input
void ACPP_MyPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 키 바인딩하기
	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this,
		&ACPP_MyPlayerPawn::Horizontal);
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this,
		&ACPP_MyPlayerPawn::Vertical);
	PlayerInputComponent->BindAction<TDelegate<void(bool)>>(TEXT("Fire"), IE_Pressed,
		this, &ACPP_MyPlayerPawn::Fire, true);
	PlayerInputComponent->BindAction<TDelegate<void(bool)>>(TEXT("Fire"), IE_Released,
		this, &ACPP_MyPlayerPawn::Fire, false);
	PlayerInputComponent->BindAction<TDelegate<void(bool)>>(TEXT("Boost"), IE_Pressed,
		this, &ACPP_MyPlayerPawn::Boost, true);
	PlayerInputComponent->BindAction<TDelegate<void(bool)>>(TEXT("Boost"), IE_Released,
		this, &ACPP_MyPlayerPawn::Boost, false);
}

void ACPP_MyPlayerPawn::LockAndLoad()
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
}

void ACPP_MyPlayerPawn::Horizontal(float value)
{
	if (!bIsDead)
	{
		CurrentMoveDirection = FVector(
			CurrentMoveDirection.X,
			value,
			CurrentMoveDirection.Z);

		MovementComponent->SetDirectionToMove(CurrentMoveDirection);
		SetTiltingHorizontal(value);
	}
}

void ACPP_MyPlayerPawn::Vertical(float value)
{
	if (!bIsDead)
	{
		CurrentMoveDirection = FVector(
			CurrentMoveDirection.X,
			CurrentMoveDirection.Y,
			value);

		MovementComponent->SetDirectionToMove(CurrentMoveDirection);
	}
}

void ACPP_MyPlayerPawn::Boost(bool isBoosting)
{
	if (!bIsDead)
	{
		bIsBoost = isBoosting;
		MovementComponent->MoveSpeed *= (bIsBoost) ? 2.0f : 0.5f;
	}
}

void ACPP_MyPlayerPawn::Fire(bool isFiring)
{
	// 플레이어가 생존 시에만 사격 가능
	if (!bIsDead)
	{
		bIsFiring = isFiring;
		if (bIsFiring)
		{
			FireAction();

			// 연사
			if (bAutoFireMode)
				GetWorldTimerManager().SetTimer(AutoFireTimerHandle, this, &ACPP_MyPlayerPawn::FireAction, AutoFireDelay, true);
		}
		else
		{
			// 연사 취소
			if (bAutoFireMode)
				GetWorldTimerManager().ClearTimer(AutoFireTimerHandle);
		}
	}
}

void ACPP_MyPlayerPawn::FireAction()
{
	if (!BulletMagazine.IsValidIndex(0))
	{
		UE_LOG(LogTemp, Error, TEXT("Empty Magazine"));
		return;
	}

	BulletMagazine[PoolBulletIndex]->FireBullet(FirePosition->GetComponentLocation(), GetActorUpVector());
	PoolBulletIndex = (PoolBulletIndex + 1) % MaxMagazineBullets;

	// @TODO : 발사 사운드 배치
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), FireSound, GetActorLocation());
}

void ACPP_MyPlayerPawn::SetTiltingHorizontal(double axis)
{
	TargetTiltRotDegrees = UKismetMathLibrary::MapRangeClamped(
		axis, -1.0, 1.0, HorizontalTiltRotDegreesMax, -HorizontalTiltRotDegreesMax
	);
}

void ACPP_MyPlayerPawn::UpdateTiltingFlight(float deltaTime)
{
	FRotator targetRot(0.0, TargetTiltRotDegrees, 0.0);
	FRotator tiltRot = UKismetMathLibrary::RInterpTo(TiltPoint->GetRelativeRotation(), targetRot, deltaTime, TiltingInterpSpeed);
	TiltPoint->SetRelativeRotation(tiltRot);
}

void ACPP_MyPlayerPawn::UpdateScore(int32 score)
{
	TotalScore += score;
	ReceiveUpdateScore(TotalScore);
}

bool ACPP_MyPlayerPawn::HitDamage(AActor* instigator)
{
	// 데미지와 상관없이 이미 사망한 상태면 False 반환
	if (bIsDead)
		return false;

	ACPP_Enemy* hitEnemy = Cast<ACPP_Enemy>(instigator);
	ACPP_BulletBase* hitBullet = Cast<ACPP_BulletBase>(instigator);

	if (IsValid(hitEnemy) || IsValid(hitBullet))
	{
		// 쉴드!
		if (CurrentShieldBattery > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Shield Damage"));
			--CurrentShieldBattery;
			if (CurrentShieldBattery == 0)
			{
				ShieldMesh->SetVisibility(false);
				ShieldMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
			SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

			if (IsValid(hitEnemy))
				hitEnemy->DeadEnemy();
		}
		// 사망
		else
		{
			if (IsValid(hitEnemy))
			{
				hitEnemy->SetHiddenEnemy(true);
				hitEnemy->PlayerKill();
			}

			PlayerDead();
			UE_LOG(LogTemp, Warning, TEXT("Game Over! [ Socre : %d]"), TotalScore);
		}
	}

	return bIsDead;
}

void ACPP_MyPlayerPawn::SetHiddenPlayer(bool hidden)
{
	SetActorHiddenInGame(hidden);
	SphereCollision->SetCollisionEnabled((hidden) ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);
}

void ACPP_MyPlayerPawn::PlayerDead()
{
	bIsDead = true;
	bIsFiring = false;

	// 연사 취소
	if (bAutoFireMode)
		GetWorldTimerManager().ClearTimer(AutoFireTimerHandle);

	CurrentMoveDirection = FVector(0.0);
	SetHiddenPlayer(true);

	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticle, GetActorLocation());
}

void ACPP_MyPlayerPawn::HandleFinishedGame()
{
	UE_LOG(LogTemp, Warning, TEXT("Game Finished! Total Score : %d"), TotalScore);
	bIsFiring = false;

	// 연사 취소
	if (bAutoFireMode)
		GetWorldTimerManager().ClearTimer(AutoFireTimerHandle);

	SetTiltingHorizontal(0.0);
	MovementComponent->SetDirectionToMove(FVector(0.0));
}