// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/GameplayAbility_Blink.h"
#include "GAS/StatAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayEffect.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"

UGameplayAbility_Blink::UGameplayAbility_Blink()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	//NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted; // 네트워크 게임일때의 정책
}

void UGameplayAbility_Blink::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo * ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData * TriggerEventData)
{
	// 코스트와 쿨다운 검사 후, 가능하면 적용
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);	// 마지막 true는 정상적인 종료가 아니라는 표시
		return;
	}

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		// 캐릭터가 아닌 액터가 이 어빌리티를 사용했다.
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 시작 위치와 현재 회전
	const FVector StartLocation = Character->GetActorLocation();
	const FRotator CurrentRotation = Character->GetActorRotation();
	
	// 목적지 위치 계산
	const float CurrentLevel = GetAbilityLevel(Handle, ActorInfo);
	const float Distance = BlinkDistance.GetValueAtLevel(CurrentLevel);
	const FVector Destination = CalcuateBlinkDestination(Character, Distance);

	const bool bTeleported = Character->TeleportTo(Destination, CurrentRotation, false, true);
	if (!bTeleported)
	{
		Character->SetActorLocation(Destination, false, nullptr, ETeleportType::TeleportPhysics);
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);	// 성공적으로 끝났다.
}

bool UGameplayAbility_Blink::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	// 필요한 데이터들 있는지 확인
	UGameplayEffect* CostGE = GetCostGameplayEffect();
	if (!CostGE) return true;

	UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	if (!ASC) return false;

	if(!ASC->HasAttributeSetForAttribute(UStatAttributeSet::GetStaminaAttribute())
		|| !ASC->HasAttributeSetForAttribute(UStatAttributeSet::GetStaminaCostAttribute()) )
		return false;

	// 시전자의 현재 Stamina값 가져오기
	const float CurrentStamina = ASC->GetNumericAttribute(UStatAttributeSet::GetStaminaAttribute());
	
	// 필요 소모량을 알기 위해 Spec 생성
	const FGameplayEffectContextHandle EffectContext = MakeEffectContext(Handle, ActorInfo);
	const float AbilityLevel = GetAbilityLevel(Handle, ActorInfo);
	FGameplayEffectSpec Spec(CostGE, EffectContext, AbilityLevel);
	Spec.CalculateModifierMagnitudes();	// 이 스펙에 해당하는 모디파이어 계산

	// GE에서 StaminaCost 추출하기
	float StaminaCost = 0.0f;
	bool bFoundStaminaModifier = false;
	for (int32 ModIndex = 0; ModIndex < Spec.Modifiers.Num(); ModIndex++)
	{
		if (Spec.Def && Spec.Def->Modifiers.IsValidIndex(ModIndex))
		{
			const FGameplayModifierInfo& ModDef = Spec.Def->Modifiers[ModIndex];
			if (ModDef.Attribute == UStatAttributeSet::GetStaminaCostAttribute())
			{
				const FModifierSpec& ModSpec = Spec.Modifiers[ModIndex];
				StaminaCost += ModSpec.GetEvaluatedMagnitude();
				bFoundStaminaModifier = true;
			}
		}
	}

	// GE에 StaminaCost관련 모디파이어가 없으면 기본 로직 실행
	if (!bFoundStaminaModifier)
	{
		return Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);
	}

	// 충분한 스테미너가 있으면 성공
	if (StaminaCost <= CurrentStamina)
	{
		return true;
	}

	// 스테미너가 없어서 실패했다고 OptionalRelevantTags에 ActivateFailCostTag 기록하기
	const FGameplayTag& CostTag = UAbilitySystemGlobals::Get().ActivateFailCostTag;
	if (OptionalRelevantTags && CostTag.IsValid())
	{
		OptionalRelevantTags->AddTag(CostTag);
	}
	return false;
}

FVector UGameplayAbility_Blink::CalcuateBlinkDestination(const ACharacter * InCharacter, float InDistance) const
{
	if (!InCharacter) return FVector::ZeroVector;
	UWorld* World = InCharacter->GetWorld();
	if (!World) return InCharacter->GetActorLocation();

	// 이론상 목표 위치
	const FVector StartLocation = InCharacter->GetActorLocation();
	const FVector DesiredDestination = StartLocation + (InCharacter->GetActorForwardVector() * InDistance);
	
	// 충돌 체크용 캡슐 만들기
	const UCapsuleComponent* Capsule = InCharacter->GetCapsuleComponent();
	const float Radius = Capsule->GetScaledCapsuleRadius();
	const float HalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(Radius, HalfHeight);

	// 스윕 체크할 시작 위치와 종료 위치 계산
	const FVector SweepStart = StartLocation + FVector(0.0f, 0.0f, StepOffset);
	const FVector SweepEnd = DesiredDestination + FVector(0.0f, 0.0f, StepOffset);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(InCharacter);

	FHitResult SweepHit;
	bool bHit = World->SweepSingleByChannel(
		SweepHit, SweepStart, SweepEnd,
		FQuat::Identity, ECollisionChannel::ECC_Visibility, CapsuleShape,
		QueryParams);

	// 최종 위치 설정
	FVector FinalDestination = DesiredDestination;
	if (bHit && SweepHit.bBlockingHit && !SweepHit.bStartPenetrating && SweepHit.Time > 0.005f)
	{
		FinalDestination = SweepHit.Location + (SweepHit.ImpactNormal * CollisionOffset);
		FinalDestination.Z = StartLocation.Z;
	}

	if (bTraceFloor)
	{
		// 공중에 떠있지 않게 하기
		FHitResult FloorHit;
		const FVector FloorStart = FinalDestination + FVector(0.0f, 0.0f, 50.0f);
		const FVector FloorEnd = FinalDestination - FVector(0.0f, 0.0f, TraceFloorDistance);
		if (World->LineTraceSingleByChannel(FloorHit, FloorStart, FloorEnd, ECC_Visibility, QueryParams))
		{
			FinalDestination.Z = FloorHit.ImpactPoint.Z + HalfHeight;
		}
	}

	return FinalDestination;
}
