// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/TestCharacter.h"
#include "AbilitySystemComponent.h"
#include "GAS/StatAttributeSet.h"

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

