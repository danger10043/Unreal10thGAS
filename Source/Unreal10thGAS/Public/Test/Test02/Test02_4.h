// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Test/Test02/Test02_2.h"
#include "GameplayTagContainer.h"
#include "Test02_4.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL10THGAS_API ATest02_4 : public ATest02_2
{
	GENERATED_BODY()
public:
	virtual void ApplyGameplayEffect() override;

	UFUNCTION(CallInEditor, Category = "GAS")
	void AddTag();

	UFUNCTION(CallInEditor, Category = "GAS")
	void RemoveTag();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	TObjectPtr<ATestCharacter> Source;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	FGameplayTag TagToTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	FGameplayTagContainer TagsToTarget;
	
};
