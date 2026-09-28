// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Test/Test03/TestPlayerCharacter03.h"
#include "NetTestCharacter01_Connection.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL10THGAS_API ANetTestCharacter01_Connection : public ATestPlayerCharacter03
{
	GENERATED_BODY()

protected:
	virtual void Tick(float DeltaTime) override;
	
};
