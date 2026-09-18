// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/MMC/MMC_Critical_False.h"
#include "GAS/StatAttributeSet.h"
#include "GameplayEffectExecutionCalculation.h"

//-------------------------------------------------------------
struct FCriticalFalseStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalChance);
	FCriticalFalseStatics()
	{
		// 소스의 공격력을 생성시점기준(snapshot == true)으로 캡쳐한다
		DEFINE_ATTRIBUTE_CAPTUREDEF(UStatAttributeSet, AttackPower, Source, true);

		// 소스의 크리티컬 확률을 생성시점 기준으로 캡쳐한다.
		DEFINE_ATTRIBUTE_CAPTUREDEF(UStatAttributeSet, CriticalChance, Source, true);
	}
};

static const FCriticalFalseStatics& CriticalFalseStatics()
{
	static FCriticalFalseStatics DStatics;
	return DStatics;
}
//-------------------------------------------------------------

UMMC_Critical_False::UMMC_Critical_False()
{
	RelevantAttributesToCapture.Add(CriticalFalseStatics().AttackPowerDef);
	RelevantAttributesToCapture.Add(CriticalFalseStatics().CriticalChanceDef);
}

float UMMC_Critical_False::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec & Spec) const
{
	// 잘못된 사용 방법
	// MMC는 순수 계산 함수여야 한다.
	// 멀티플레이에서 예측과 동기화에 문제가 생길 수 있다.
	// 외부로 크리티컬이 터졌다는 것을 알릴 수가 없디/
	FAggregatorEvaluateParameters EvalParams;

	float Attack = 0.0f;
	GetCapturedAttributeMagnitude(CriticalFalseStatics().AttackPowerDef, Spec, EvalParams, Attack);
	
	float Damage = FMath::Max(1.0f, Attack * 2.0f);

	float CriticalChance = 0.0f;
	GetCapturedAttributeMagnitude(CriticalFalseStatics().CriticalChanceDef, Spec, EvalParams, CriticalChance);
	if (FMath::FRand() < CriticalChance)
	{
		Damage *= CriticalMultiflier;	// 크리티컬이 터지면 CriticalMultiflier배 데미지
	}

	return Damage;
}
