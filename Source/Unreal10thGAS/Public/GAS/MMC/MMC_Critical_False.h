// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_Critical_False.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL10THGAS_API UMMC_Critical_False : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
public:
	UMMC_Critical_False();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CriticalMultiflier = 5.0f;
};
