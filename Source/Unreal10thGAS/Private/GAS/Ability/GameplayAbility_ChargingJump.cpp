// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/GameplayAbility_ChargingJump.h"
#include "GAS/StatAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Abilities/Tasks/AbilityTask_WaitMovementModeChange.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"

UGameplayAbility_ChargingJump::UGameplayAbility_ChargingJump()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	FGameplayTag GroundedTag = FGameplayTag::RequestGameplayTag(FName("GAS.State.Grounded"), false);
	if (GroundedTag.IsValid())
	{
		ActivationRequiredTags.AddTag(GroundedTag);	// 땅에 있을 때만 활성화 가능
	}
	FGameplayTag JumpingTag = FGameplayTag::RequestGameplayTag(FName("GAS.State.Jumping"), false);
	if(JumpingTag.IsValid())
	{ 
		ActivationOwnedTags.AddTag(JumpingTag);		// 활성화 되면 JumpingTag 부여
		ActivationBlockedTags.AddTag(JumpingTag);	// JumpingTag있으면 활성화 불가능
	}
}

void UGameplayAbility_ChargingJump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo * ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData * TriggerEventData)
{
	//UE_LOG(LogTemp, Log, TEXT("어빌리티 활성화"));
	// 코스트와 쿨다운 검사 후, 가능하면 적용
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);	// 마지막 true는 정상적인 종료가 아니라는 표시
		return;
	}

	//UE_LOG(LogTemp, Log, TEXT("커밋 통과"));

	// 기본 변수 초기화
	bJumpExecuted = false;
	ChargeStartTime = GetWorld()->GetTimeSeconds();

	// 스테미너 지속 감소 이팩트 적용
	if (ChargingCostEffectClass)
	{
		FGameplayEffectSpecHandle CostSpecHandle = MakeOutgoingGameplayEffectSpec(
			Handle, ActorInfo, ActivationInfo, ChargingCostEffectClass, GetAbilityLevel(Handle, ActorInfo)
		);
		if (CostSpecHandle.IsValid())
		{
			ChargingCostEffectHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, CostSpecHandle);
		}
	}

	// 이속 감소 이팩트 적용
	if (ChargingSlowEffectClass)
	{
		FGameplayEffectSpecHandle SlowSpecHandle = MakeOutgoingGameplayEffectSpec(
			Handle, ActorInfo, ActivationInfo, ChargingSlowEffectClass, GetAbilityLevel(Handle, ActorInfo)
		);
		if (SlowSpecHandle.IsValid())
		{
			ChargingSlowEffectHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SlowSpecHandle);
		}
	}

	//FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	//TimerManager.SetTimer(
	//	ChargePregressTimerHandle,
	//	this,
	//	&UGameplayAbility_ChargingJump::UpdateChargeProgress,
	//	0.033f,
	//	true
	//);

	// bAutoReleaseOnMaxHold가 세팅되어 있을 경우 타이머로 자동 점프하게 등록
	if (bAutoReleaseOnMaxHold && MaxHoldTime > 0.0f)
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();

		// ExecuteChargeJump에 무조건 MaxHoldTime가 파라메터로 들어가는 void() 형식의 함수를 바인딩 한것과 마찬가지
		FTimerDelegate AutoReleaseDelegate;
		AutoReleaseDelegate.BindUObject(this, &UGameplayAbility_ChargingJump::ExecuteChargeJump, MaxHoldTime);
		TimerManager.SetTimer(
			AutoReleaseTimerHandle,
			AutoReleaseDelegate,
			MaxHoldTime,
			false
		);
	}

	// 태스크로 입력때기 대기
	UAbilityTask_WaitInputRelease* WaitReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, false);
	if (WaitReleaseTask)
	{
		WaitReleaseTask->OnRelease.AddDynamic(this, &UGameplayAbility_ChargingJump::OnWaitInputReleaseCallback);
		WaitReleaseTask->ReadyForActivation();
	}
}

void UGameplayAbility_ChargingJump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo * ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (!IsActive()) return;
	
	CleanupChargingState();	// 정리할 것들 정리

	bJumpExecuted = false;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UGameplayAbility_ChargingJump::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo * ActorInfo, OUT FGameplayTagContainer * OptionalRelevantTags) const
{
	if (!Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags))
	{
		return false;
	}

	const UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	if (ASC && MinStamicaToActivate > 0.0f)
	{
		const float CurrentStamina = ASC->GetNumericAttribute(UStatAttributeSet::GetStaminaAttribute());
		if (CurrentStamina < MinStamicaToActivate)
		{
			const FGameplayTag& CostTag = UAbilitySystemGlobals::Get().ActivateFailCostTag;
			if (OptionalRelevantTags && CostTag.IsValid())
			{
				OptionalRelevantTags->AddTag(CostTag);
			}
			return false;
		}
	}
	return true;
}

void UGameplayAbility_ChargingJump::OnWaitInputReleaseCallback(float TimeHeld)
{
	ExecuteChargeJump(TimeHeld);
}

void UGameplayAbility_ChargingJump::OnMovementModeChangedCallback(EMovementMode NewMovementMode)
{
	if (NewMovementMode == MOVE_Walking)
	{
		// 착지를 완료해야 어빌리티가 정상 종료 되었다.
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UGameplayAbility_ChargingJump::ExecuteChargeJump(float InTimeHeld)
{
	if (bJumpExecuted) return;	// 두번 실행 방지
	bJumpExecuted = true;

	CleanupChargingState();		// 타이머와 이팩트 제거

	// 충전율 계산( 0.0 ~ 1.0 )
	const float EffectiveTime = FMath::Clamp(InTimeHeld, 0.0f, MaxHoldTime);
	float ChargeAlpha = 0.0f;
	if (MaxHoldTime > MinHoldTime)
	{
		ChargeAlpha = FMath::Clamp((EffectiveTime - MinHoldTime) / (MaxHoldTime - MinHoldTime), 0.0f, 1.0f);
	}

	// 최종 발사력 계산
	const float FinalZVelocity = FMath::Lerp(MinJumpVelocity, MaxJumpVelocity, ChargeAlpha);

	// 실제 점프 처리
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (Character)
	{
		Character->LaunchCharacter(FVector(0, 0, FinalZVelocity), false, true);
	}

	// 착지 감지 태스크 만들기
	UAbilityTask_WaitMovementModeChange* WaitLandingTask = 
		UAbilityTask_WaitMovementModeChange::CreateWaitMovementModeChange(this, MOVE_Walking);
	if (WaitLandingTask)
	{
		WaitLandingTask->OnChange.AddDynamic(this, &UGameplayAbility_ChargingJump::OnMovementModeChangedCallback);
		WaitLandingTask->ReadyForActivation();
	}
}

//void UGameplayAbility_ChargingJump::UpdateChargeProgress()
//{
//	if (!IsActive() || bJumpExecuted) return;	// 비활성화 상태이거나 점프 중일 때는 차징이 있을 수 없음
//
//	const float CurrentTime = GetWorld()->GetTimeSeconds();
//	const float ElapsedTime = CurrentTime - ChargeStartTime;
//	const float Ratio = (MaxHoldTime > 0.0f) ? FMath::Clamp(ElapsedTime / MaxHoldTime, 0.0f, 1.0f) : 1.0f;
//
//	// UI 업데이트 처리
//}

void UGameplayAbility_ChargingJump::CleanupChargingState()
{
	// 타이머 정리
	if (UWorld* World = GetWorld())
	{
		FTimerManager& TimerManager = World->GetTimerManager();
		//TimerManager.ClearTimer(ChargePregressTimerHandle);
		TimerManager.ClearTimer(AutoReleaseTimerHandle);
	}

	// 이팩트 해제
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (ChargingSlowEffectHandle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(ChargingSlowEffectHandle);
		}
		ChargingSlowEffectHandle.Invalidate();

		if (ChargingCostEffectHandle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(ChargingCostEffectHandle);
		}
		ChargingCostEffectHandle.Invalidate();
	}	
}
