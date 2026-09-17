// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/StatWidget.h"
#include "GAS/StatAttributeSet.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Widget/StaminaBarWidget.h"

void UStatWidget::InitializeWithAbilitySystem(AActor* InActor)
{
	Super::InitializeWithAbilitySystem(InActor);

	if (!ASC.IsValid()) return;

	FOnGameplayAttributeValueChange& StaminaChange = ASC->GetGameplayAttributeValueChangeDelegate(UStatAttributeSet::GetStaminaAttribute());
	StaminaChange.AddUObject(this, &UStatWidget::OnStaminaChanged);

	FOnGameplayAttributeValueChange& MaxStaminaChange = ASC->GetGameplayAttributeValueChangeDelegate(UStatAttributeSet::GetMaxStaminaAttribute());
	MaxStaminaChange.AddUObject(this, &UStatWidget::OnMaxStaminaChanged);

	bool bFound = false;
	const float TempCurrent = ASC->GetGameplayAttributeValue(UStatAttributeSet::GetStaminaAttribute(), bFound);
	CurrentStamina = bFound ? TempCurrent : 0.0f;	// 못찾았으면 0

	bFound = false;
	const float TempMax = ASC->GetGameplayAttributeValue(UStatAttributeSet::GetMaxStaminaAttribute(), bFound);
	MaxStamina = bFound ? TempMax : 100.0f;	// 못찾았으면 100

	UpdateStaminaUI(CurrentStamina, MaxStamina);
}

void UStatWidget::OnStaminaChanged(const FOnAttributeChangeData & InData)
{
	CurrentStamina = InData.NewValue;
	UpdateStaminaUI(CurrentStamina, MaxStamina);
}

void UStatWidget::OnMaxStaminaChanged(const FOnAttributeChangeData & InData)
{
	MaxStamina = InData.NewValue;
	UpdateStaminaUI(CurrentStamina, MaxStamina);
}

void UStatWidget::UpdateStaminaUI(float InCurrent, float InMax)
{
	const float Percent = FMath::IsNearlyZero(InMax) ? 0.0f : FMath::Clamp(InCurrent / InMax, 0.0f, 1.0f);
	if (IsValid(StaminaBarWidget))
	{
		StaminaBarWidget->SetStamina(InCurrent, InMax);
	}
	BP_OnStaminaChanged(InCurrent, InMax, Percent);
}
