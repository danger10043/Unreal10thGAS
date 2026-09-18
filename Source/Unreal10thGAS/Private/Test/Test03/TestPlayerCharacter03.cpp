// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/Test03/TestPlayerCharacter03.h"
#include "AbilitySystemComponent.h"

void ATestPlayerCharacter03::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

	// 어빌리티 주기
}

void ATestPlayerCharacter03::PossessedBy(AController * NewController)
{
	Super::PossessedBy(NewController);
	// 어빌리티 주기
}

void ATestPlayerCharacter03::GiveTestAbility()
{
	if (!TestAbilityClass) return;

	// 이미 부여된 어빌리티를 다시 부여하는 상황인지 확인
	if (FGameplayAbilitySpec* ExistingSpec = AbilitySystemComponent->FindAbilitySpecFromClass(TestAbilityClass))
	{
		ExistingSpec->Level = TestAbilityLevel;
		AbilitySystemComponent->MarkAbilitySpecDirty(*ExistingSpec);	// 스펙 변경됨을 알리기
		return;
	}

	// 새로 어빌리티를 부여
	FGameplayAbilitySpec Spec(TestAbilityClass, TestAbilityLevel);
	FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(Spec);
	//if (Handle.IsValid())
	//{
	//	// 어빌리티 부여 성공
	//}
	//else
	//{
	//	// 어빌리티 부여 실패
	//}

}

void ATestPlayerCharacter03::ClearTestAbility()
{
	if (!TestAbilityClass) return;
	if (FGameplayAbilitySpec* ExistingSpec = AbilitySystemComponent->FindAbilitySpecFromClass(TestAbilityClass))
	{
		AbilitySystemComponent->ClearAbility(ExistingSpec->Handle);
	}
}

void ATestPlayerCharacter03::ClearAllTestAbility()
{
	AbilitySystemComponent->ClearAllAbilities();
}

void ATestPlayerCharacter03::ActivateTestAbility()
{
	if (!TestAbilityClass) return;

	const bool bSuccess = AbilitySystemComponent->TryActivateAbilityByClass(TestAbilityClass);
	//if (bSuccess)
	//{
	//	// 발동 성공
	//}
	//else
	//{
	//	// 발동 실패
	//}
}

void ATestPlayerCharacter03::DeactivateTestAbility()
{
	if (!TestAbilityClass) return;

	FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromClass(TestAbilityClass);
	if (Spec && Spec->IsActive())
	{
		AbilitySystemComponent->CancelAbilityHandle(Spec->Handle);	// 강제로 취소 시키기
	}
}
