// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/GameplayAbility_Sprint.h"
#include "GAS/StatAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayEffect.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"

UGameplayAbility_Sprint::UGameplayAbility_Sprint()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGameplayAbility_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo * ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData * TriggerEventData)
{
	// 코스트와 쿨다운 검사 후, 가능하면 적용
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);	// 마지막 true는 정상적인 종료가 아니라는 표시
		return;
	}

	// ASC 없으면 종료
	UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	if (!ASC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	//UE_LOG(LogTemp, Log, TEXT("UGameplayAbility_Sprint : 기본 조건 통과"));

	// 이속 증가 버프 적용
	if (SprintBuffEffectClass)
	{
		FGameplayEffectSpecHandle BuffSpecHandle = MakeOutgoingGameplayEffectSpec(
			Handle, ActorInfo, ActivationInfo, SprintBuffEffectClass, GetAbilityLevel(Handle, ActorInfo));

		if (BuffSpecHandle.IsValid())
		{
			BuffEffectHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, BuffSpecHandle);
			//UE_LOG(LogTemp, Log, TEXT("UGameplayAbility_Sprint : 버프 적용 완료"));
		}
	}

	// 지속 스테미너 소모 이펙트 적용(Period)
	if (SprintCostEffectClass)
	{
		FGameplayEffectSpecHandle CostSpecHandle = MakeOutgoingGameplayEffectSpec(
			Handle, ActorInfo, ActivationInfo, SprintCostEffectClass, GetAbilityLevel(Handle, ActorInfo));

		if (CostSpecHandle.IsValid())
		{
			CostEffectHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, CostSpecHandle);
			//UE_LOG(LogTemp, Log, TEXT("UGameplayAbility_Sprint : 코스트 적용 완료"));
		}
	}

	// 스테미너 변화 감시(0이하가 될 때를 대비)
	FOnGameplayAttributeValueChange& StaminaChangeDelegate = ASC->GetGameplayAttributeValueChangeDelegate(
		UStatAttributeSet::GetStaminaAttribute());
	StaminaChangedDelegateHandle = StaminaChangeDelegate.AddUObject(this, &UGameplayAbility_Sprint::OnStatminaChanged);
	//UE_LOG(LogTemp, Log, TEXT("UGameplayAbility_Sprint : 델리게이트 연결"));

	// 입력 방식에 따라 테스크 등록
	if (bToggleMode)
	{
		UAbilityTask_WaitInputPress* WaitPressTask = UAbilityTask_WaitInputPress::WaitInputPress(this);
		if (WaitPressTask)
		{
			WaitPressTask->OnPress.AddDynamic(this, &UGameplayAbility_Sprint::OnWaitInputPressCallback);
			WaitPressTask->ReadyForActivation();
			//UE_LOG(LogTemp, Log, TEXT("UGameplayAbility_Sprint : 테스크 실행. UAbilityTask_WaitInputPress "));
		}
	}
	else
	{
		UAbilityTask_WaitInputRelease* WaitReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this);
		if (WaitReleaseTask)
		{
			WaitReleaseTask->OnRelease.AddDynamic(this, &UGameplayAbility_Sprint::OnWaitInputReleaseCallback);
			WaitReleaseTask->ReadyForActivation();
			//UE_LOG(LogTemp, Log, TEXT("UGameplayAbility_Sprint : 테스크 실행. UAbilityTask_WaitInputRelease "));
		}
	}
}

void UGameplayAbility_Sprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo * ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (!IsActive())	// 동시 종료에 대비한 방어 코드
	{
		return;	
	}

	UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	if (!ASC) return;

	// 스테미너 감시 델리게이트 해제
	if (StaminaChangedDelegateHandle.IsValid())
	{
		FOnGameplayAttributeValueChange& StaminaChangeDelegate = ASC->GetGameplayAttributeValueChangeDelegate(
			UStatAttributeSet::GetStaminaAttribute());
		StaminaChangeDelegate.Remove(StaminaChangedDelegateHandle);	// 델리게이트 연결 해제하고
		StaminaChangedDelegateHandle.Reset();	// 리셋
	}

	// 이펙트 제거
	if (CostEffectHandle.IsValid())
	{
		ASC->RemoveActiveGameplayEffect(CostEffectHandle);	// 이펙트 제거하고
		CostEffectHandle.Invalidate();	// 핸들 초기화
	}
	if (BuffEffectHandle.IsValid())
	{
		ASC->RemoveActiveGameplayEffect(BuffEffectHandle);
		BuffEffectHandle.Invalidate();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UGameplayAbility_Sprint::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo * ActorInfo, OUT FGameplayTagContainer * OptionalRelevantTags) const
{
	// 필요한 데이터들 있는지 확인
	UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	if (!ASC) return false;

	if (!ASC->HasAttributeSetForAttribute(UStatAttributeSet::GetStaminaAttribute())
		|| !ASC->HasAttributeSetForAttribute(UStatAttributeSet::GetStaminaCostAttribute()))
		return false;

	// 시전자의 현재 Stamina값 가져오기
	const float CurrentStamina = ASC->GetNumericAttribute(UStatAttributeSet::GetStaminaAttribute());
	if (CurrentStamina < MinStatimaToActivate)
	{
		const FGameplayTag& CostTag = UAbilitySystemGlobals::Get().ActivateFailCostTag;
		if (OptionalRelevantTags && CostTag.IsValid())
		{
			OptionalRelevantTags->AddTag(CostTag);
		}
		return false;
	}
	
	return true;
}

void UGameplayAbility_Sprint::OnStatminaChanged(const FOnAttributeChangeData& InData)
{
	// 이 어빌리티가 발동 중이고 스테미너가 0 이하일 때만 처리
	if (InData.NewValue <= 0.0f && IsActive())
	{
		// 정상 종료로 처리
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UGameplayAbility_Sprint::OnWaitInputReleaseCallback(float InTimeHeld)
{
	if (!bToggleMode && IsActive())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UGameplayAbility_Sprint::OnWaitInputPressCallback(float InElapsedTime)
{
	if (bToggleMode && IsActive())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}
