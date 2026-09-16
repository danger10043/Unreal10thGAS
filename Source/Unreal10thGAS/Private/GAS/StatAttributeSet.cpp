// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/StatAttributeSet.h"

UStatAttributeSet::UStatAttributeSet()
{
	InitHealth(100.0f);
	InitMaxHealth(100.0f);

	InitStamina(100.0f);
	InitMaxStamina(100.0f);

	InitDefense(10.0f);
	InitDamage(0.0f);
	InitStaminaCost(0.0f);
}

void UStatAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		// Health가 변경되려고 해서 호출되었다.
		NewValue = FMath::Clamp(NewValue, 0, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		// MaxHealth가 변경되려고 해서 호출되었다.
		NewValue = FMath::Max(0, NewValue);

		// 최대 체력이 변경되려고 하면 Health를 10 감소시키는 예시
		//UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
		//ASC->ApplyModToAttributeUnsafe(GetHealthAttribute(), EGameplayModOp::AddBase, -10);

		// 실습 조건
		// 최대 체력이 증가하면 그만큼 현재 채력도 증가한다.
		// 최대 체력이 감소하면 최대 체력 이상분만 제거한다.
	}
	else if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStamina());
	}
	else if (Attribute == GetMaxStaminaAttribute() || Attribute == GetDefenseAttribute())
	{
		NewValue = FMath::Max(0.0f, NewValue);
	}
}

void UStatAttributeSet::PostAttributeChange(const FGameplayAttribute & Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		UE_LOG(LogTemp, Log, TEXT("[UStatAttributeSet] Health 변경됨 : (%.1f) -> (%.1f)"), OldValue, NewValue);
		//GetOwningAbilitySystemComponent();
		//GetOwningActor();
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		const float HealthIncrease = FMath::Max(NewValue - OldValue, 0.0f);
		const float NewHealth = FMath::Clamp(GetHealth() + HealthIncrease, 0.0f, NewValue);
		SetHealth(NewHealth);
	}
	else if (Attribute == GetMaxStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.0f, NewValue));
	}
}

void UStatAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	//if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	//{
	//	// 이팩트로 인해 변경된 어트리뷰트가 Health다
	//}

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float LocalDamage = GetDamage();
		SetDamage(0.0f);

		if (LocalDamage > 0)
		{
			const float FinalDamage = FMath::Max(LocalDamage - FMath::Max(GetDefense(), 0.0f), 0.0f);
			const float NewHealth = FMath::Clamp(GetHealth() - FinalDamage, 0.0f, GetMaxHealth());
			SetHealth(NewHealth);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetStaminaCostAttribute())
	{
		const float LocalStaminaCost = GetStaminaCost();
		SetStaminaCost(0.0f);

		if (LocalStaminaCost > 0.0f)
		{
			const float NewStamina = FMath::Clamp(GetStamina() - LocalStaminaCost, 0.0f, GetMaxStamina());
			SetStamina(NewStamina);
		}
	}
}
