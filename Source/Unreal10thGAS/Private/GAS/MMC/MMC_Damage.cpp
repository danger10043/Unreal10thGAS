// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/MMC/MMC_Damage.h"
#include "GAS/StatAttributeSet.h"
#include "GameplayEffectExecutionCalculation.h"

//-------------------------------------------------------------
struct FDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefencePower);
	FDamageStatics()
	{
		// 소스의 공격력을 생성시점기준(snapshot == true)으로 캡쳐한다
		DEFINE_ATTRIBUTE_CAPTUREDEF(UStatAttributeSet, AttackPower, Source, true);

		// 대상의 방어력을 피격시점기준(snapshot == false)으로 캡쳐한다.
		DEFINE_ATTRIBUTE_CAPTUREDEF(UStatAttributeSet, DefencePower, Target, false);
	}
};

static const FDamageStatics& DamageStatics()
{
	static FDamageStatics DStatics;
	return DStatics;
}
//-------------------------------------------------------------


UMMC_Damage::UMMC_Damage()
{
	// 어떤 어트리뷰트를 캡쳐할 것인지 지정
	RelevantAttributesToCapture.Add(DamageStatics().AttackPowerDef);
	RelevantAttributesToCapture.Add(DamageStatics().DefencePowerDef);
}

float UMMC_Damage::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec & Spec) const
{
	FAggregatorEvaluateParameters EvalParams;

	float Attack = 0.0f;
	GetCapturedAttributeMagnitude(DamageStatics().AttackPowerDef, Spec, EvalParams, Attack);

	float Defence = 0.0f;
	GetCapturedAttributeMagnitude(DamageStatics().DefencePowerDef, Spec, EvalParams, Defence);

	float Damage = FMath::Max(1.0f, Attack - Defence);
	return Damage;
}
