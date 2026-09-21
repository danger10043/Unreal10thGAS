#include "GAS/Ability/GameplayAbility_PlayerJump.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "AbilitySystemComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/StatAttributeSet.h"
#include "Player/PlayerCharacter.h"

UGameplayAbility_PlayerJump::UGameplayAbility_PlayerJump()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bRetriggerInstancedAbility = false;
}

void UGameplayAbility_PlayerJump::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ChargeStartTime = World->GetTimeSeconds();
	UpdateCharge();

	if (!IsActive()) return;

	UAbilityTask_WaitInputRelease* WaitReleaseTask =
		UAbilityTask_WaitInputRelease::WaitInputRelease(this, true);

	if (!WaitReleaseTask)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	World->GetTimerManager().SetTimer(
		ChargeTimerHandle,
		this,
		&UGameplayAbility_PlayerJump::UpdateCharge,
		0.02f,
		true);

	WaitReleaseTask->OnRelease.AddDynamic(this, &UGameplayAbility_PlayerJump::OnInputReleased);
	WaitReleaseTask->ReadyForActivation();
}

void UGameplayAbility_PlayerJump::OnInputReleased(float TimeHeld)
{
	if (!IsActive()) return;

	UpdateCharge();
	if (!IsActive()) return;

	APlayerCharacter* Character = Cast<APlayerCharacter>(GetAvatarActorFromActorInfo());
	UStatAttributeSet* Stats = Character ? Character->GetStatAttributeSet() : nullptr;

	if (!Character || !Stats)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	const float MaxCharge = Stats->GetMaxJumpCharge();
	const float JumpRatio = MaxCharge > 0.0f
		? FMath::Clamp(Stats->GetCurrentJumpCharge() / MaxCharge, 0.0f, 1.0f)
		: 0.0f;

	const float BaseJumpSpeed = FMath::Max(0.0f, Character->GetCharacterMovement()->JumpZVelocity);
	const float ChargedJumpSpeed = BaseJumpSpeed * FMath::Sqrt(1.0f + JumpRatio);

	if (!CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	if (!IsActive()) return;

	Character->LaunchCharacter(FVector(0.0f, 0.0f, ChargedJumpSpeed), false, true);

	UE_LOG(LogTemp, Log, TEXT("[PlayerJump] Ratio=%.2f, LaunchSpeed=%.2f"),
		JumpRatio, ChargedJumpSpeed);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

bool UGameplayAbility_PlayerJump::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !Super::CanActivateAbility(
		Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	return Character
		&& Character->GetStatAttributeSet()
		&& Character->GetCharacterMovement()->IsMovingOnGround()
		&& Character->CanJump();
}

void UGameplayAbility_PlayerJump::UpdateCharge()
{
	if (!IsActive()) return;

	APlayerCharacter* Character = Cast<APlayerCharacter>(GetAvatarActorFromActorInfo());
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	UWorld* World = GetWorld();
	UStatAttributeSet* Stats = Character ? Character->GetStatAttributeSet() : nullptr;

	if (!Character || !ASC || !World || !Stats
		|| !Character->GetCharacterMovement()->IsMovingOnGround()
		|| !Character->CanJump())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	const float ElapsedTime = FMath::Max(0.0f, World->GetTimeSeconds() - ChargeStartTime);
	const float Progress = FMath::Clamp(ElapsedTime / FMath::Max(ChargeDuration, 0.01f), 0.0f, 1.0f);
	const float MaxCharge = FMath::Max(0.0f, Stats->GetMaxJumpCharge());

	ASC->SetNumericAttributeBase(UStatAttributeSet::GetCurrentJumpChargeAttribute(), MaxCharge * Progress);
}

void UGameplayAbility_PlayerJump::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	if (!IsActive()) return;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ChargeTimerHandle);
	}

	UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	if (ASC && ASC->HasAttributeSetForAttribute(UStatAttributeSet::GetCurrentJumpChargeAttribute()))
	{
		ASC->SetNumericAttributeBase(UStatAttributeSet::GetCurrentJumpChargeAttribute(), 0.0f);
	}
	ChargeStartTime = 0.0f;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}