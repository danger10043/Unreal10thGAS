// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Data/PlayerAbilitySet.h"
#include "AbilitySystemComponent.h"

void UPlayerAbilitySet::GiveToAbilitySystem(UAbilitySystemComponent* InTargetASC, TArray<FGameplayAbilitySpecHandle>& OutGrantedHandles) const
{
	if (!InTargetASC) return;

	for (const FPlayerAbilityConfig& Config : Abilities)
	{
		if (!Config.AbilityClass) continue;

		FGameplayAbilitySpec Spec(Config.AbilityClass, Config.AbilityLevel, static_cast<int32>(Config.InputID));

		FGameplayAbilitySpecHandle Handle = InTargetASC->GiveAbility(Spec);
		OutGrantedHandles.Add(Handle);

		if (Config.bActivateOnGranted)
		{
			InTargetASC->TryActivateAbility(Handle);
		}
	}
}
