// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbility_ChargingJump.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL10THGAS_API UGameplayAbility_ChargingJump : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGameplayAbility_ChargingJump();

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

	// MinStamicaToActivate 때문에 체크
	virtual bool CheckCost(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

private:
	// WaitInputRelease 태스크용 콜백
	UFUNCTION()
	void OnWaitInputReleaseCallback(float TimeHeld);

	// WaitMovementModeChange 태스크용 콜백
	UFUNCTION()
	void OnMovementModeChangedCallback(EMovementMode NewMovementMode);

	// 실제 점프 처리용 함수
	void ExecuteChargeJump(float InTimeHeld);

	//// 차지 진행 처리용 타이머 콜백(UI용)
	//void UpdateChargeProgress();

	// 각종 리소스 정리용 함수
	void CleanupChargingState();

protected:
	// 최소 점프 높이
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ChargingJump|Physics", meta = (ClampMin = "100.0"))
	float MinJumpVelocity = 420.0f;

	// 최대 점프 높이
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ChargingJump|Physics", meta = (ClampMin = "100.0"))
	float MaxJumpVelocity = 1200.0f;

	// 최소 충전 시간(이 값 이하면 최소 점프 높이로 점프)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ChargingJump|Charge", meta = (ClampMin = "0.0"))
	float MinHoldTime = 0.05f;

	// 최대 충전 시간(이 값 이상이면 최대 점프 높이로 점프)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ChargingJump|Charge", meta = (ClampMin = "0.1"))
	float MaxHoldTime = 1.0f;

	// 최대 충전 시간에 도달하면 자동으로 점프 할지 여부(true면 자동 점프)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ChargingJump|Charge")
	bool bAutoReleaseOnMaxHold = true;

	// 차징 중 이속 감소를 처리할 이팩트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ChargingJump|Effects")
	TSubclassOf<UGameplayEffect> ChargingSlowEffectClass;

	// 차징 중 지속적으로 스테미너를 감소시킬 이팩트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ChargingJump|Effects")
	TSubclassOf<UGameplayEffect> ChargingCostEffectClass;

	// 차징 시작에 필요한 최소 수치
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ChargingJump|Cost", meta = (ClampMin = "0.0"))
	float MinStamicaToActivate = 10.0f;

private:
	// 슬로우 이팩트 핸들
	FActiveGameplayEffectHandle ChargingSlowEffectHandle;
	// 차지 중 스테미너 소비 이팩트 핸들
	FActiveGameplayEffectHandle ChargingCostEffectHandle;

	// 차지 시작 시간
	float ChargeStartTime = 0.0f;

	//// 차지 진행용 타이머 핸들
	//FTimerHandle ChargePregressTimerHandle;

	// 풀차지시 자동 점프 타이머 핸들
	FTimerHandle AutoReleaseTimerHandle;

	// 현재 점프 실행 여부
	bool bJumpExecuted = false;
};
