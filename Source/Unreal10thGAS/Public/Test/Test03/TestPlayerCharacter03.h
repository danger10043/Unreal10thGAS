// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Test/TestPlayerCharacter.h"
#include "TestPlayerCharacter03.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL10THGAS_API ATestPlayerCharacter03 : public ATestPlayerCharacter
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION(CallInEditor, Category = "GAS|Ability")
	void GiveTestAbility();

	UFUNCTION(CallInEditor, Category = "GAS|Ability")
	void ClearTestAbility();

	UFUNCTION(CallInEditor, Category = "GAS|Ability")
	void ClearAllTestAbility();

	UFUNCTION(CallInEditor, Category = "GAS|Ability")
	void ActivateTestAbility();

	UFUNCTION(CallInEditor, Category = "GAS|Ability")
	void DeactivateTestAbility();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS|Ability")
	TSubclassOf<UGameplayAbility> TestAbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS|Ability", meta = (ClampMin = "1"))
	int32 TestAbilityLevel = 1;
};
