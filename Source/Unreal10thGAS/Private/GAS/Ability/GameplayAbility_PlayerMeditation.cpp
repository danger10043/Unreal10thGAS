#include "GAS/Ability/GameplayAbility_PlayerMeditation.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameplayEffect.h"
#include "NativeGameplayTags.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Player_State_Meditation, "Player.State.Meditation");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Ability_Meditation, "Ability.Meditation");

UGameplayAbility_PlayerMeditation::UGameplayAbility_PlayerMeditation()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bRetriggerInstancedAbility = false;

	SetAssetTags(FGameplayTagContainer(TAG_Ability_Meditation));
	ActivationOwnedTags.AddTag(TAG_Player_State_Meditation);

	const FGameplayTag AbilityRoot = FGameplayTag::RequestGameplayTag(TEXT("Ability"));
	BlockAbilitiesWithTag.AddTag(AbilityRoot);
	CancelAbilitiesWithTag.AddTag(AbilityRoot);
}

void UGameplayAbility_PlayerMeditation::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	bEnding = false;

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	UCharacterMovementComponent* Movement = Character ? Character->GetCharacterMovement() : nullptr;
	const UGameplayEffect* RecoveryEffect = RecoveryEffectClass ? RecoveryEffectClass->GetDefaultObject<UGameplayEffect>() : nullptr;

	if (!ASC || !Movement || !RecoveryEffect || RecoveryEffect->DurationPolicy != EGameplayEffectDurationType::Infinite)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (Spec.Handle != Handle && Spec.IsActive())
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!IsActive()) return;

	UAbilityTask_WaitInputRelease* WaitReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, true);

	if (!WaitReleaseTask)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	LockedMovement = Movement;
	SavedMovementMode = Movement->MovementMode;
	SavedCustomMovementMode = Movement->CustomMovementMode;
	bMovementLocked = true;
	Movement->StopMovementImmediately();
	Movement->DisableMovement();
	if (!IsActive()) return;

	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(
		Handle, ActorInfo, ActivationInfo, RecoveryEffectClass, GetAbilityLevel(Handle, ActorInfo)
	);

	if (!SpecHandle.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	RecoveryEffectHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);

	if (!IsActive())
	{
		ASC->RemoveActiveGameplayEffect(RecoveryEffectHandle);
		RecoveryEffectHandle.Invalidate();
		return;
	}
	if (!RecoveryEffectHandle.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	WaitReleaseTask->OnRelease.AddDynamic(
		this, &UGameplayAbility_PlayerMeditation::OnInputReleased
	);

	WaitReleaseTask->ReadyForActivation();
}

void UGameplayAbility_PlayerMeditation::OnInputReleased(float TimeHeld)
{
	if (IsActive())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UGameplayAbility_PlayerMeditation::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (!IsActive() || bEnding) return;
	bEnding = true;

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (RecoveryEffectHandle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(RecoveryEffectHandle);
		}
	}
	RecoveryEffectHandle.Invalidate();

	if (bMovementLocked)
	{
		bMovementLocked = false;
		if (UCharacterMovementComponent* Movement = LockedMovement.Get())
		{
			Movement->ConsumeInputVector();
			Movement->SetMovementMode(SavedMovementMode, SavedCustomMovementMode);
		}
	}
	LockedMovement.Reset();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
