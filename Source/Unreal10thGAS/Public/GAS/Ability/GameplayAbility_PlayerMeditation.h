#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayAbility_PlayerMeditation.generated.h"

UCLASS()
class UNREAL10THGAS_API UGameplayAbility_PlayerMeditation : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGameplayAbility_PlayerMeditation();

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

protected:
	UFUNCTION()
	void OnInputReleased(float TimeHeld);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meditation|Effects")
	TSubclassOf<UGameplayEffect> RecoveryEffectClass;

	FActiveGameplayEffectHandle RecoveryEffectHandle;
	TWeakObjectPtr<UCharacterMovementComponent> LockedMovement;
	EMovementMode SavedMovementMode = MOVE_None;
	uint8 SavedCustomMovementMode = 0;
	bool bMovementLocked = false;
	bool bEnding = false;
};
