// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/StatAttributeSet.h"

UStatAttributeSet::UStatAttributeSet()
{
	InitHealth(100.0f);
	InitMaxHealth(100.0f);

	InitStamina(100.0f);
	InitMaxStamina(100.0f);

	InitAttackPower(10.0f);
	InitCriticalChance(0.2f);
	InitDefencePower(5.0f);
	InitMoveSpeed(100.0f);	// 100이 보통 속도. 150이면 원래 속도의 1.5배


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
	}
	else if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0, GetMaxStamina());
	}
	else if (Attribute == GetMaxStaminaAttribute())
	{
		NewValue = FMath::Max(0, NewValue);
	}
	else if (Attribute == GetMoveSpeedAttribute())
	{
		NewValue = FMath::Max(0, NewValue);
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
		// 최대 체력이 변경되려고 하면 Health를 10 감소시키는 예시
		//UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
		//ASC->ApplyModToAttributeUnsafe(GetHealthAttribute(), EGameplayModOp::AddBase, -10);

		// 실습 조건
		// 최대 체력이 증가하면 그만큼 현재 채력도 증가한다.
		// 최대 체력이 감소하면 최대 체력 이상분만 제거한다.

		AdjustAttributeForMaxChange(OldValue, NewValue, GetHealthAttribute());
	}
}

void UStatAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	// BaseValue가 변경될 때만 실행된다.
	Super::PostGameplayEffectExecute(Data);
		
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// 이팩트로 인해 변경된 어트리뷰트가 Damage다
		const float LocalDamage = GetDamage();
		SetDamage(0.0f);	// [가장 중요] : 메타어트리뷰트는 사용했으면 비워야 한다.

		if (LocalDamage > 0)
		{
			float FinalDamage = LocalDamage - GetDefencePower();	// 각종 계산 추가(방어력, 최소대미지보장, 쉴드, 피해 증가 등등)
			FinalDamage = FMath::Max(1.0f, FinalDamage);			// 최소대미지 보장

			const float NewHealth = FMath::Clamp(GetHealth() - FinalDamage, 0.0f, GetMaxHealth());
			SetHealth(NewHealth);

			// 값에 따른 추가 처리
		}
	}
	else if (Data.EvaluatedData.Attribute == GetStaminaCostAttribute())
	{
		const float LocalCost = GetStaminaCost();
		SetStaminaCost(0.0f);	// [가장 중요] : 메타어트리뷰트는 사용했으면 비워야 한다.

		if (LocalCost > 0)
		{
			float FinalCost = LocalCost;
			FinalCost = FMath::Max(0.0f, FinalCost);			// 0 이하는 안됨

			const float NewStamina = FMath::Clamp(GetStamina() - FinalCost, 0.0f, GetMaxStamina());
			SetStamina(NewStamina);

			// 값에 따른 추가 처리
		}
	}
}

void UStatAttributeSet::AdjustAttributeForMaxChange(float InOldValue, float InNewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	if (!ASC) return;

	float Delta = InNewMaxValue - InOldValue;
	if (Delta > 0)
	{
		ASC->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, Delta);
	}
	else
	{
		bool bFound = false;
		const float Current = ASC->GetGameplayAttributeValue(AffectedAttributeProperty, bFound);
		if (InNewMaxValue < Current)
		{
			Delta = InNewMaxValue - Current;
			ASC->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, Delta);
		}
	}
}
