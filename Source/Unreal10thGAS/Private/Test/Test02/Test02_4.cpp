// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/Test02/Test02_4.h"
#include "Test/TestCharacter.h"
#include "AbilitySystemComponent.h"

void ATest02_4::ApplyGameplayEffect()
{
	if (!Source) return;
	if (!Target) return;
	if (!GameplayEffectClass) return;

	UAbilitySystemComponent* SourceASC = Source->GetAbilitySystemComponent();
	if (!SourceASC) return;
	UAbilitySystemComponent* TargetASC = Target->GetAbilitySystemComponent();
	if (!TargetASC) return;

	UE_LOG(LogTemp, Log, TEXT("ATest02_4::ApplyGameplayEffect"));

	// 컨택스트 설정(이펙트의 정보들을 설정)
	FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
	EffectContext.AddSourceObject(Source);        // 주로 이펙트의 발생 원인 에셋/데이터
	EffectContext.AddInstigator(Source, Source);  // 이펙트를 발생한 주체와 매체

	// 이팩트 스팩 설정
	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(GameplayEffectClass, EffectLevel, EffectContext);
	if (!SpecHandle.IsValid()) return;

	UE_LOG(LogTemp, Log, TEXT("ATest02_4 - 스팩 생성 완료"));

	// ASC가 대상에게 스팩 적용
	FActiveGameplayEffectHandle ActiveEffectHandle = SourceASC->ApplyGameplayEffectSpecToTarget(
		*SpecHandle.Data.Get(), TargetASC);

	if (ActiveEffectHandle.WasSuccessfullyApplied())
	{
		UE_LOG(LogTemp, Log, TEXT("ATest02_4 - 스팩 적용 성공"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("ATest02_4 - 스팩 적용 실패"));
	}
}

void ATest02_4::AddTag()
{
	if (!Target) return;
	UAbilitySystemComponent* TargetASC = Target->GetAbilitySystemComponent();
	if (!TargetASC) return;

	TargetASC->AddLooseGameplayTag(TagToTarget);
	//TargetASC->AddLooseGameplayTags(TagsToTarget);
}

void ATest02_4::RemoveTag()
{
	if (!Target) return;
	UAbilitySystemComponent* TargetASC = Target->GetAbilitySystemComponent();
	if (!TargetASC) return;

	TargetASC->RemoveLooseGameplayTag(TagToTarget);
}
