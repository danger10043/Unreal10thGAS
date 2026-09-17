// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/Test02/Test02_1.h"
#include "Test/TestCharacter.h"
#include "AbilitySystemComponent.h"
#include "GAS/StatAttributeSet.h"
#include "GameplayEffect.h"
#include "NativeGameplayTags.h"
#include "Engine/World.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Test02_Damage, "Data.Damage");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Test02_Defense, "Data.Defense");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Test02_StaminaCost, "Data.StaminaCost");

void ATest02_1::ApplyGameplayEffect()
{
	if (!GetWorld() || !GetWorld()->IsGameWorld())
	{
		return;
	}
	if (!IsValid(Target) || !Target->HasAuthority())
	{
		return;
	}
	if (!GameplayEffectClass)
	{
		return;
	}

	UAbilitySystemComponent* TargetASC = Target->GetAbilitySystemComponent();
	if (!IsValid(TargetASC))
	{
		return;
	}

	UStatAttributeSet* Stat = Target->GetStatAttribute();
	if (!IsValid(Stat))
	{
		return;
	}

	if (!FMath::IsFinite(EffectLevel) || EffectLevel <= 0.0f) return;
	if (!FMath::IsFinite(MaxHealthMultiplier) || MaxHealthMultiplier <= 0.0f) return;
	if (!FMath::IsFinite(DamageAmount) || DamageAmount < 0.0f) return;
	if (!FMath::IsFinite(DefenseAmount) || DefenseAmount < 0.0f) return;
	if (!FMath::IsFinite(StaminaCostAmount) || StaminaCostAmount < 0.0f) return;

	const float NewMaxHealth = Stat->GetMaxHealth() * MaxHealthMultiplier;
	if (!FMath::IsFinite(NewMaxHealth)) return;

	const FGameplayTag MaxHealthTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Data.MaxHealthValue")));

	TMap<FGameplayTag, float> Magnitudes;
	Magnitudes.Add(MaxHealthTag, NewMaxHealth);
	Magnitudes.Add(TAG_Test02_Damage, DamageAmount);
	Magnitudes.Add(TAG_Test02_Defense, DefenseAmount);
	Magnitudes.Add(TAG_Test02_StaminaCost, StaminaCostAmount);

	// 컨택스트 설정(이펙트의 정보들을 설정)
	FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	EffectContext.AddInstigator(this, this);

	// 이팩트 스팩 설정
	FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, EffectLevel, EffectContext);
	if (!SpecHandle.IsValid()) return;

	for (const TPair<FGameplayTag, float>& Magnitude : Magnitudes)
	{
		SpecHandle.Data->SetSetByCallerMagnitude(Magnitude.Key, Magnitude.Value);
	}

	// 같은 테스트 액터가 적용한 Infinite Effect는 값을 갱신한다.
	if (SpecHandle.Data->Def->DurationPolicy == EGameplayEffectDurationType::Infinite &&
		SpecHandle.Data->GetPeriod() <= 0.0f)
	{
		FGameplayEffectQuery Query;
		Query.EffectDefinition = GameplayEffectClass;
		Query.EffectSource = this;

		const TArray<FActiveGameplayEffectHandle> ActiveHandles = TargetASC->GetActiveEffects(Query);

		if (ActiveHandles.Num() == 1)
		{
			TargetASC->UpdateActiveGameplayEffectSetByCallerMagnitudes(ActiveHandles[0], Magnitudes);
			return;
		}
	}

	FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());



	//if (ActiveEffectHandle.WasSuccessfullyApplied())
	//{
	//	// 성공
	//}
	//else
	//{
	//	// 실패
	//}
	
	// 이펙트 개별 제거는 아래와 같이
	//TargetASC->RemoveActiveGameplayEffect(ActiveEffectHandle);
}

void ATest02_1::RemoveAllEffects()
{
	if (!Target) return;

	UAbilitySystemComponent* TargetASC = Target->GetAbilitySystemComponent();
	if (!TargetASC) return;

	FGameplayEffectQuery Query;	// 모든 것
	TargetASC->RemoveActiveEffects(Query);	// 모든 이펙트 삭제
}

void ATest02_1::ResetTarget()
{
	if (!GetWorld() || !GetWorld()->IsGameWorld()) return;
	if (!IsValid(Target) || !Target->HasAuthority()) return;

	UStatAttributeSet* Stat = Target->GetStatAttribute();
	if (!IsValid(Stat)) return;

	RemoveAllEffects();

	Stat->SetHealth(Stat->GetMaxHealth());
	Stat->SetStamina(Stat->GetMaxStamina());
	Stat->SetDamage(0.0f);
	Stat->SetStaminaCost(0.0f);
}
