// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/TestCharacter.h"
#include "AbilitySystemComponent.h"
#include "GAS/StatAttributeSet.h"
#include "GameplayEffect.h"
#include "NativeGameplayTags.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Data_MaxHealthValue, "Data.MaxHealthValue");

// Sets default values
ATestCharacter::ATestCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	StatAttributeSet = CreateDefaultSubobject<UStatAttributeSet>(TEXT("Stat"));

}

UAbilitySystemComponent* ATestCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UStatAttributeSet* ATestCharacter::GetStatAttribute() const
{
	return StatAttributeSet;
}

bool ATestCharacter::ApplyMaxHealthMultiplier(float Multiplier)
{
	if (!HasAuthority())
	{
		return false;
	}
	if (!IsValid(AbilitySystemComponent) || !IsValid(StatAttributeSet))
	{
		return false;
	}
	if (!FMath::IsFinite(Multiplier) || Multiplier <= 0.0f)
	{
		return false;
	}
	if (!MaxHealthEffectClass)
	{
		return false;
	}

	const float NewMaxHealth = StatAttributeSet->GetMaxHealth() * Multiplier;
	if (!FMath::IsFinite(NewMaxHealth))
	{
		return false;
	}

	if (AbilitySystemComponent->GetActiveGameplayEffect(MaxHealthEffectHandle))
	{
		AbilitySystemComponent->UpdateActiveGameplayEffectSetByCallerMagnitude(
			MaxHealthEffectHandle, TAG_Data_MaxHealthValue, NewMaxHealth
		);
		return true;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	EffectContext.AddInstigator(this, this);

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
		MaxHealthEffectClass, 1.0f, EffectContext);
	if (!SpecHandle.IsValid())
	{
		return false;
	}

	SpecHandle.Data->SetSetByCallerMagnitude(TAG_Data_MaxHealthValue, NewMaxHealth);
	MaxHealthEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

	return MaxHealthEffectHandle.IsValid();
}

// Called when the game starts or when spawned
void ATestCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ATestCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		//FOnGameplayAttributeValueChange& HealthChange = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UStatAttributeSet::GetHealthAttribute());
		//HealthChange.AddUObject(this, &ATestCharacter::OnHealthChanged);
	}
}

//void ATestCharacter::OnHealthChanged(const FOnAttributeChangeData& InData)
//{
//	UE_LOG(LogTemp, Log, TEXT("[ATestCharacter] 체력이 변경되었습니다. (%.1f) -> (%.1f)"), InData.OldValue, InData.NewValue);
//}

