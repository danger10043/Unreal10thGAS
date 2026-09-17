// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/Test02/Test02_3.h"
#include "Test/TestCharacter.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"


ATest02_3::ATest02_3()
{
	SetByCallerTag = FGameplayTag::RequestGameplayTag(FName("GAS.Test.SetByCaller"), false);
}

void ATest02_3::ApplyGameplayEffect()
{
    if (!Target) return;
    if (!GameplayEffectClass) return;
    if (!ASC) return;
    if (!Stat) return;

    UAbilitySystemComponent* TargetASC = Target->GetAbilitySystemComponent();
    if (!TargetASC) return;

    // 컨택스트 설정(이펙트의 정보들을 설정)
    FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
    EffectContext.AddSourceObject(this);        // 주로 이펙트의 발생 원인 에셋/데이터
    EffectContext.AddInstigator(this, this);    // 이펙트를 발생한 주체와 매체

    // 이팩트 스팩 설정
    FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GameplayEffectClass, EffectLevel, EffectContext);
    if (!SpecHandle.IsValid()) return;

    // 태그 확인
    if (!SetByCallerTag.IsValid())
    {
        SetByCallerTag = FGameplayTag::RequestGameplayTag(FName("GAS.Test.SetByCaller"), false);
    }
    // SetByCaller를 이용해 값 주입
    if (SetByCallerTag.IsValid())
    {
        SpecHandle.Data->SetSetByCallerMagnitude(SetByCallerTag, SetByCallerMagnitude);
    }

    // ASC가 대상에게 스팩 적용
    FActiveGameplayEffectHandle ActiveEffectHandle = ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);

    
}
