// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbilityTypes.h"
#include "GameplayAbilitySpecHandle.h"
#include "PlayerAbilitySet.generated.h"

class UAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class UNREAL10THGAS_API UPlayerAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** 기본 부여할 어빌리티 목록 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<FPlayerAbilityConfig> Abilities;

	void GiveToAbilitySystem(
		UAbilitySystemComponent* InTargetASC, 
		TArray<FGameplayAbilitySpecHandle>& OutGrantedHandles
	) const;
};
