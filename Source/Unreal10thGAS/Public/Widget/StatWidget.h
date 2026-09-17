// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget/OverHeadWidget.h"
#include "StatWidget.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL10THGAS_API UStatWidget : public UOverHeadWidget
{
	GENERATED_BODY()

public:
	virtual void InitializeWithAbilitySystem(AActor* InActor) override;

protected:
	virtual void OnStaminaChanged(const FOnAttributeChangeData& InData);
	virtual void OnMaxStaminaChanged(const FOnAttributeChangeData& InData);
	virtual void UpdateStaminaUI(float InCurrent, float InMax);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnStaminaChanged"))
	void BP_OnStaminaChanged(float InCurrent, float InMax, float InPercent);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<class UStaminaBarWidget> StaminaBarWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentStamina = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxStamina = 100.0f;
};
