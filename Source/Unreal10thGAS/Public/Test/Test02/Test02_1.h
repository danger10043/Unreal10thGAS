// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Test/TestBaseActor.h"
#include "Test02_1.generated.h"

class ATestCharacter;
class UGameplayEffect;
/**
 * 
 */
UCLASS()
class UNREAL10THGAS_API ATest02_1 : public ATestBaseActor
{
	GENERATED_BODY()
public:

	UFUNCTION(CallInEditor, Category = "GAS")
	virtual void ApplyGameplayEffect();

	UFUNCTION(CallInEditor, Category = "GAS")
	void RemoveAllEffects();

	UFUNCTION(CallInEditor, Category = "GAS")
	void ResetTarget();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	TSubclassOf<UGameplayEffect> GameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	float EffectLevel = 1.0f;
	
};
