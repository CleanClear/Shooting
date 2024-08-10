// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_BulletBase.h"
#include "CPP_BaseMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"	
#include "CPP_Enemy.h"
#include "CPP_MyPlayerPawn.h"

// Sets default values
ACPP_BulletBase::ACPP_BulletBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// CreateDefaultSubobject() �Լ��� ���� ������Ʈ ����
	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	MovementComp = CreateDefaultSubobject<UCPP_BaseMovementComponent>(TEXT("MovementComp"));

	// ������Ʈ ����
	RootComponent = BulletMesh;

	BulletMesh->SetRelativeScale3D(FVector(0.1, 0.1, 0.4));
	BulletMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	BulletMesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"), true);
	MovementComp->MoveSpeed = 2000.0f;

	// ����� ���� ����
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	ConstructorHelpers::FObjectFinder<UMaterial> tempMat(TEXT("Material'/Game/Mat/M_Bullet.M_Bullet'"));

	if (tempMesh.Succeeded())
		BulletMesh->SetStaticMesh(tempMesh.Object);

	if (tempMat.Succeeded())
		BulletMesh->SetMaterial(0, tempMat.Object);
}

// Called when the game starts or when spawned
void ACPP_BulletBase::BeginPlay()
{
	Super::BeginPlay();
	// AddDynamic�� �ǵ��� BeginPlay���� �����ϴ°� ����. �����ڿ��� �ϸ� ���ε��� �Ǵٰ� �� �Ǵ� ���ڴ�δ�
	BulletMesh->OnComponentBeginOverlap.AddDynamic(this, &ACPP_BulletBase::OnComponentBeginOverlap);
	SetActivationBullet(false);
}

// Called every frame
void ACPP_BulletBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACPP_BulletBase::FireBullet(FVector firePos, FVector direction)
{
	if (bIsFire)
		GetWorldTimerManager().ClearTimer(HiddenBulletTimerHandle);
	else
		bIsFire = true;

	SetActorLocation(firePos);
	MovementComp->SetDirectionToMove(direction);
	SetActorRotation(UKismetMathLibrary::MakeRotFromXZ(GetActorForwardVector(), direction));
	SetActivationBullet(true);

	if (bIsAutoHidden)
	{
		GetWorldTimerManager().SetTimer(HiddenBulletTimerHandle, this, &ACPP_BulletBase::AutoHiddenBullet, BulletHiddenTime, false);
	}
}

void ACPP_BulletBase::SetActivationBullet(bool isActive)
{
	SetActorHiddenInGame(!isActive);
	BulletMesh->SetCollisionEnabled((isActive) ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void ACPP_BulletBase::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor))
	{
		bIsFire = false;
		SetActivationBullet(false);
		GetWorldTimerManager().ClearTimer(HiddenBulletTimerHandle);
	}
}

void ACPP_BulletBase::AutoHiddenBullet()
{
	SetActivationBullet(false);
	bIsFire = false;
}

