// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Execution/EC_StackDamage.h"
#include "GAS/StatAttributeSet.h"
#include "AbilitySystemComponent.h"

//-------------------------------------------------------------
struct FStackDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalChance);
	FStackDamageStatics()
	{
		// 소스의 공격력을 생성시점기준(snapshot == true)으로 캡쳐한다
		DEFINE_ATTRIBUTE_CAPTUREDEF(UStatAttributeSet, AttackPower, Source, true);

		// 소스의 크리티컬 확률을 생성시점 기준으로 캡쳐한다.
		DEFINE_ATTRIBUTE_CAPTUREDEF(UStatAttributeSet, CriticalChance, Source, true);
	}
};

static const FStackDamageStatics& StackDamageStatics()
{
	static FStackDamageStatics DStatics;
	return DStatics;
}
//-------------------------------------------------------------

UEC_StackDamage::UEC_StackDamage()
{
	RelevantAttributesToCapture.Add(StackDamageStatics().AttackPowerDef);
	RelevantAttributesToCapture.Add(StackDamageStatics().CriticalChanceDef);
}

void UEC_StackDamage::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams, 
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	if (!TargetASC) return;		

	// 스택 카운트 세기
	int32 StackCount = 0;
	if (DebuffEffectClass)
	{
		FGameplayEffectQuery Query;
		Query.EffectDefinition = DebuffEffectClass;
		TArray<FActiveGameplayEffectHandle> ActiveHandles = TargetASC->GetActiveEffects(Query);
		for (const FActiveGameplayEffectHandle& Handle : ActiveHandles)
		{
			if (const FActiveGameplayEffect* ActiveGE = TargetASC->GetActiveGameplayEffect(Handle))
			{
				StackCount += ActiveGE->Spec.GetStackCount();
			}
		}
	}

	FAggregatorEvaluateParameters EvaluationParamaters;		

	//const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();		// 태그 확인하는 법
	//EvaluationParamaters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	//EvaluationParamaters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	//const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	//TargetTags->HasTag(FGameplayTag::RequestGameplayTag(FName("GAS.Test.Debuff.Curse")));	// 저주 태그가 붙어 있는지 확인하기

	// 공격력과 크리티컬 확률 가져오기
	float AttackPower = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		StackDamageStatics().AttackPowerDef, EvaluationParamaters, AttackPower);
	float CriticalChance = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		StackDamageStatics().CriticalChanceDef, EvaluationParamaters, CriticalChance);
	float CriticalModifier = 1.0f;
	if (FMath::FRand() < CriticalChance)
	{
		CriticalModifier *= CriticalRate;
	}

	StackCount = FMath::Clamp(StackCount, 0, 30);	// 부호비트 침범 방지
	const float StackMultiflier = static_cast<float>(1 << StackCount);
	const float FinalDamage = (BaseDamage + AttackPower) * CriticalModifier * StackMultiflier;
	if (FinalDamage > 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(UStatAttributeSet::GetDamageAttribute(), EGameplayModOp::Additive, FinalDamage)
		);
	}

	
}
