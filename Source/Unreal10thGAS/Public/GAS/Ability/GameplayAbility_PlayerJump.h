#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "TimerManager.h"
#include "GameplayAbility_PlayerJump.generated.h"

UCLASS()
class UNREAL10THGAS_API UGameplayAbility_PlayerJump : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGameplayAbility_PlayerJump();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

	virtual bool CheckCost(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

protected:
	UFUNCTION()
	void OnInputReleased(float TimeHeld);

	void UpdateCharge();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jump", meta = (ClampMin = "0.01"))
	float ChargeDuration = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jump|Effects")
	TSubclassOf<UGameplayEffect> ChargingCostEffectClass;

private:
	FTimerHandle ChargeTimerHandle;
	float ChargeStartTime = 0.0f;
	bool bFullChargeNotified = false;

	FActiveGameplayEffectHandle ChargingCostEffectHandle;
};