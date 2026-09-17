// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Test/Test02/Test02_2.h"
#include "GameplayTagContainer.h"
#include "Test02_3.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL10THGAS_API ATest02_3 : public ATest02_2
{
	GENERATED_BODY()
	
public:
	ATest02_3();

protected:
	virtual void ApplyGameplayEffect() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	float SetByCallerMagnitude = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS", meta = (Categories = "GAS"))
	FGameplayTag SetByCallerTag;
};
