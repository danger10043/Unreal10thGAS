// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/OverHeadWidget.h"
#include "GAS/StatAttributeSet.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Widget/HealthBarWidget.h"

void UOverHeadWidget::InitializeWithAbilitySystem(AActor* InActor)
{
	if (!InActor) return;
	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(InActor);
	if (!ASI) return;
	UAbilitySystemComponent* AbilitySystemComp = ASI->GetAbilitySystemComponent();
	if (!AbilitySystemComp) return;
	ASC = AbilitySystemComp;

	FOnGameplayAttributeValueChange& HealthChange = ASC->GetGameplayAttributeValueChangeDelegate(UStatAttributeSet::GetHealthAttribute());
	HealthChange.AddUObject(this, &UOverHeadWidget::OnHealthChanged);

	FOnGameplayAttributeValueChange& MaxHealthChange = ASC->GetGameplayAttributeValueChangeDelegate(UStatAttributeSet::GetMaxHealthAttribute());
	MaxHealthChange.AddUObject(this, &UOverHeadWidget::OnMaxHealthChanged);

	bool bFound = false;
	const float TempCurrent = ASC->GetGameplayAttributeValue(UStatAttributeSet::GetHealthAttribute(), bFound);
	CurrentHealth = bFound ? TempCurrent : 0.0f;	// 못찾았으면 0

	bFound = false;
	const float TempMax = ASC->GetGameplayAttributeValue(UStatAttributeSet::GetMaxHealthAttribute(), bFound);
	MaxHealth = bFound ? TempMax : 100.0f;	// 못찾았으면 100

	UpdateHealthUI(CurrentHealth, MaxHealth);

}

void UOverHeadWidget::OnHealthChanged(const FOnAttributeChangeData & InData)
{
	CurrentHealth = InData.NewValue;
	UpdateHealthUI(CurrentHealth, MaxHealth);
}

void UOverHeadWidget::OnMaxHealthChanged(const FOnAttributeChangeData & InData)
{
	MaxHealth = InData.NewValue;
	UpdateHealthUI(CurrentHealth, MaxHealth);
}

void UOverHeadWidget::UpdateHealthUI(float InCurrent, float InMax)
{
	const float Percent = FMath::IsNearlyZero(InMax) ? 0.0f : FMath::Clamp(InCurrent / InMax, 0.0f, 1.0f);
	if (IsValid(HealthBarWidget))
	{
		HealthBarWidget->SetHealth(InCurrent, InMax);
	}
	else
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("OverHeadWidget::UpdateHealthUI - HealthBarWidget이 유효하지 않습니다.")
		);
	}
	BP_OnHealthChanged(InCurrent, InMax, Percent);
}
