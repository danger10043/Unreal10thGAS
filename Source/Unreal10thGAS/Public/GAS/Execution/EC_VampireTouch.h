// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "EC_VampireTouch.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL10THGAS_API UEC_VampireTouch : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UEC_VampireTouch();
	virtual void Execute_Implementation(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "VampireTouch")
	float BaseDamage = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "VampireTouch", meta = (Categories = "GAS"))
	FGameplayTag TargetWeakTag;

	UPROPERTY(EditDefaultsOnly, Category = "VampireTouch", meta = (Categories = "GAS"))
	FGameplayTag SourceAdventageTag;

	UPROPERTY(EditDefaultsOnly, Category = "VampireTouch")
	float HealRate = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "VampireTouch")
	float RecoilRate = 0.1f;

};
