// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbility_Sprint.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL10THGAS_API UGameplayAbility_Sprint : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGameplayAbility_Sprint();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, 
		bool bReplicateEndAbility, 
		bool bWasCancelled) override;

	virtual bool CheckCost(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

protected:
	// false면 누르고 있는 동안만 달리기, true면 토글 형식
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sprint|Input")
	bool bToggleMode = false;

	// 이속 증가 버프(Infinite)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sprint|Effects")
	TSubclassOf<UGameplayEffect> SprintBuffEffectClass;

	// 주기적으로 스테미너 감소시키는 이펙트(Infinite인데 Period)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sprint|Effects")
	TSubclassOf<UGameplayEffect> SprintCostEffectClass;

	// 어빌리티 발동에 필요한 최소 스테미너 수치
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sprint|Cost")
	float MinStatimaToActivate = 5.0f;

private:
	// 이속 증가 버프용 핸들
	FActiveGameplayEffectHandle BuffEffectHandle;

	// 주기적 스테미너 감소용 핸들
	FActiveGameplayEffectHandle CostEffectHandle;

	// 스테미너 변경 감시용 델리게이트
	FDelegateHandle StaminaChangedDelegateHandle;

	// 스테미너 변경시 실행될 콜백 함수
	void OnStatminaChanged(const FOnAttributeChangeData& InData);

	// AbilityTast_WaitInpuRelease 용 콜백
	UFUNCTION()
	void OnWatInputReleaseCallback(float InTimeHeld);

	// AbilityTast_WaitInpuPress 용 콜백
	UFUNCTION()
	void OnWatInputPressCallback(float InElapsedTime);
};
