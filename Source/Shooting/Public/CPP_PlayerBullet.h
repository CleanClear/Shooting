// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPP_BulletBase.h"
#include "CPP_PlayerBullet.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTING_API ACPP_PlayerBullet : public ACPP_BulletBase
{
	GENERATED_BODY()
	
public:
	ACPP_PlayerBullet();

protected:
	virtual void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
