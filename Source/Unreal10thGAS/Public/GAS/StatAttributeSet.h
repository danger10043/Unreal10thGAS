// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "StatAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL10THGAS_API UStatAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UStatAttributeSet();
	// CurrentValue 변경 전에 실행되는 함수
	// 값의 Clamping용도로 사용
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	
	// CurrentValue 변경 후에 실행되는 함수
	// 값의 변화 감지나, UI에 반영하기 위해 사용
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	// 이펙트가 적용 된 후에 실행되는 함수
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	// ------------------------------------------------------------------
	UPROPERTY(BlueprintReadOnly, Category = "Base Stat")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS_BASIC(UStatAttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, Category = "Base Stat")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS_BASIC(UStatAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, Category = "Base Stat")
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS_BASIC(UStatAttributeSet, Stamina);

	UPROPERTY(BlueprintReadOnly, Category = "Base Stat")
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS_BASIC(UStatAttributeSet, MaxStamina);
	// ------------------------------------------------------------------
	UPROPERTY(BlueprintReadOnly, Category = "Jump")
	FGameplayAttributeData CurrentJumpCharge;
	ATTRIBUTE_ACCESSORS_BASIC(UStatAttributeSet, CurrentJumpCharge);

	UPROPERTY(BlueprintReadOnly, Category = "Jump")
	FGameplayAttributeData MaxJumpCharge;
	ATTRIBUTE_ACCESSORS_BASIC(UStatAttributeSet, MaxJumpCharge);
	// ------------------------------------------------------------------
	UPROPERTY(BlueprintReadOnly, Category = "Attack Stat")
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS_BASIC(UStatAttributeSet, AttackPower);

	UPROPERTY(BlueprintReadOnly, Category = "Attack Stat")
	FGameplayAttributeData CriticalChance;	// 치명타 확률(0.0 ~ 1.0)
	ATTRIBUTE_ACCESSORS_BASIC(UStatAttributeSet, CriticalChance);

	UPROPERTY(BlueprintReadOnly, Category = "Attack Stat")
	FGameplayAttributeData DefencePower;
	ATTRIBUTE_ACCESSORS_BASIC(UStatAttributeSet, DefencePower);
	// ------------------------------------------------------------------
	UPROPERTY(BlueprintReadOnly, Category = "Movement Stat")
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS_BASIC(UStatAttributeSet, MoveSpeed);
	// ------------------------------------------------------------------
	UPROPERTY(BlueprintReadOnly, Category = "Meta Attribute")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS_BASIC(UStatAttributeSet, Damage);

	UPROPERTY(BlueprintReadOnly, Category = "Meta Attribute")
	FGameplayAttributeData StaminaCost;
	ATTRIBUTE_ACCESSORS_BASIC(UStatAttributeSet, StaminaCost);

protected:
	// 최대값이 변경 되었을 때 현재 값을 보정하는 함수
	void AdjustAttributeForMaxChange(
		float InOldValue,
		float InNewMaxValue,
		const FGameplayAttribute& AffectedAttributeProperty);
};

/*
* 어빌리티 시스템 디버깅 툴
	1. 어퍼스트로피 키(')
	   - 한 캐릭터만 자세히 볼 때 유용
	   - 보고 싶은 캐릭터 변경하려면 그 캐릭터 앞으로 이동해서 어퍼스트로피 키를 길게 누르기

	2. AbilitySystem.DebugAttributes 어트리뷰트이름 어트리뷰트이름 어트리뷰트이름
	   - 특정 속성을 계속 보고 싶을 때
	   - 게임 내 해당 속성을 가진 모든 캐릭터에서 보임

	3. ShowDebug AbilitySystem
	   - 캐릭터를 쉽게 변경하며 보고 싶을 때
*/