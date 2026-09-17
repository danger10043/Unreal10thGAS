// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Test/Test02/Test02_1.h"
#include "AbilitySystemInterface.h"
#include "Test02_2.generated.h"

class UStatAttributeSet;
/**
 * 
 */
UCLASS()
class UNREAL10THGAS_API ATest02_2 : public ATest02_1, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	ATest02_2();

	virtual void ApplyGameplayEffect() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UStatAttributeSet* GetStatAttributeSet() const;

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStatAttributeSet> Stat;
};
