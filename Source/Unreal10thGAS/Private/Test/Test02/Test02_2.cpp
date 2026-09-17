// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/Test02/Test02_2.h"
#include "AbilitySystemComponent.h"
#include "GAS/StatAttributeSet.h"
#include "Test/TestCharacter.h"

ATest02_2::ATest02_2()
{
    ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
    Stat = CreateDefaultSubobject<UStatAttributeSet>(TEXT("Stat"));
}

void ATest02_2::ApplyGameplayEffect()
{
    if (!Target) return;
    if (!GameplayEffectClass) return;
    if (!ASC) return;
    if (!Stat) return;

    UAbilitySystemComponent* TargetASC = Target->GetAbilitySystemComponent();
    if (!TargetASC) return;

    // 컨택스트 설정(이펙트의 정보들을 설정)
    FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
    EffectContext.AddSourceObject(this);
    EffectContext.AddInstigator(GetInstigator(), this);

    // 이팩트 스팩 설정
    FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GameplayEffectClass, EffectLevel, EffectContext);
    if (!SpecHandle.IsValid()) return;

    // ASC가 대상에게 스팩 적용
    FActiveGameplayEffectHandle ActiveEffectHandle = ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
    //ActiveEffectHandle.WasSuccessfullyApplied()
}

UAbilitySystemComponent* ATest02_2::GetAbilitySystemComponent() const
{
    return ASC;
}

UStatAttributeSet* ATest02_2::GetStatAttributeSet() const
{
    return Stat;
}

void ATest02_2::BeginPlay()
{
    Super::BeginPlay();
    if (ASC)
    {
        UE_LOG(LogTemp, Log, TEXT("BeginPlay"));
        ASC->InitAbilityActorInfo(this, this);
        
        UE_LOG(LogTemp, Log, TEXT("AttackPower : %.1f"), Stat->GetAttackPower());
    }
}
