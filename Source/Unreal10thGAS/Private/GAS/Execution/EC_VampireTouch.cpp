// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Execution/EC_VampireTouch.h"
#include "GAS/StatAttributeSet.h"
#include "AbilitySystemComponent.h"

//-------------------------------------------------------------
struct FVampireTouchStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower);
	FVampireTouchStatics()
	{
		// 소스의 공격력을 생성시점기준(snapshot == true)으로 캡쳐한다
		DEFINE_ATTRIBUTE_CAPTUREDEF(UStatAttributeSet, AttackPower, Source, true);
	}
};

static const FVampireTouchStatics& VampireTouchStatics()
{
	static FVampireTouchStatics DStatics;
	return DStatics;
}
//-------------------------------------------------------------

UEC_VampireTouch::UEC_VampireTouch()
{
	RelevantAttributesToCapture.Add(VampireTouchStatics().AttackPowerDef);
	TargetWeakTag = FGameplayTag::RequestGameplayTag(FName("GAS.Test.Debuff.Curse"), false);
	SourceAdventageTag = FGameplayTag::RequestGameplayTag(FName("GAS.Test.Type.Undead"), false);
}

void UEC_VampireTouch::Execute_Implementation(const FGameplayEffectCustomExecutionParameters & ExecutionParams, FGameplayEffectCustomExecutionOutput & OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParams;
	EvaluationParams.SourceTags = SourceTags;
	EvaluationParams.TargetTags = TargetTags;

	// 공격력 가져오기
	float SourceAttack = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		VampireTouchStatics().AttackPowerDef, EvaluationParams, SourceAttack);

	// 공격력 기반으로 1차 데미지 계산
	float FinalDamage = FMath::Max(SourceAttack * BaseDamage, 1.0f);
	UE_LOG(LogTemp, Log, TEXT("UEC_VampireTouch - 1차 데미지 : %.1f"), FinalDamage);

	// 특정 태그가 있으면 데미지 2배
	if (TargetTags && TargetTags->HasTag(TargetWeakTag))
	{
		FinalDamage *= 2.0f;
	}
	UE_LOG(LogTemp, Log, TEXT("UEC_VampireTouch - 대상 약점 태그 확인 후 데미지 : %.1f"), FinalDamage);

	if (FinalDamage > 0.0f)
	{
		// 대상에게 데미지 적용
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
				UStatAttributeSet::GetDamageAttribute(), EGameplayModOp::Additive, FinalDamage)
		);

		// 시전자 태그 확인
		UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
		if (SourceTags && SourceTags->HasTag(SourceAdventageTag))
		{
			// 어드밴테이지를 주는 태그가 있다.(FinalDamage의 20%만큼 회복)
			SourceASC->ApplyModToAttributeUnsafe(
				UStatAttributeSet::GetHealthAttribute(),
				EGameplayModOp::Additive, FinalDamage * HealRate
			);
			UE_LOG(LogTemp, Log, TEXT("UEC_VampireTouch - 시전자 태그 확인 후 회복 : %.1f"), FinalDamage * HealRate);
		}
		else
		{
			// 어드밴테이지를 주는 태그가 없다.(FinalDamage의 10%만큼 피해)
			SourceASC->ApplyModToAttributeUnsafe(
				UStatAttributeSet::GetHealthAttribute(),
				EGameplayModOp::Additive, -FinalDamage * RecoilRate
			);
			UE_LOG(LogTemp, Log, TEXT("UEC_VampireTouch - 시전자 태그 확인 후 데미지 : %.1f"), FinalDamage * RecoilRate);
		}

	}
	
}
