// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_PlayerBullet.h"
#include "CPP_Enemy.h"
#include "CPP_MyPlayerPawn.h"

ACPP_PlayerBullet::ACPP_PlayerBullet()
{
}

void ACPP_PlayerBullet::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnComponentBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (IsValid(OtherActor))
	{
		ACPP_Enemy* enemy = Cast<ACPP_Enemy>(OtherActor);
		if (enemy)
		{
			Cast<ACPP_MyPlayerPawn>(GetOwner())->UpdateScore(enemy->GetEnemyScore());
			enemy->DeadEnemy();
		}	
	}
}
