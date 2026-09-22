// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/Test03/TestPlayerCharacter03_2.h"
#include "GAS/Data/PlayerAbilitySet.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"


ATestPlayerCharacter03_2::ATestPlayerCharacter03_2()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
}

void ATestPlayerCharacter03_2::PossessedBy(AController * NewController)
{
	Super::PossessedBy(NewController);

	GiveDefaultAbilities();
}

void ATestPlayerCharacter03_2::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		SetupAbilityInputs(EnhancedInputComponent);
	}
}

void ATestPlayerCharacter03_2::GiveDefaultAbilities()
{
	if (!AbilitySystemComponent || !DefaultAbilitySet) return;

	if (!AbilitySystemComponent->AbilityActorInfo.IsValid())
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

	// 기존에 부여된 기본 어빌리티가 있다면 모두 제거 후 재부여
	RemoveDefaultAbilities();

	DefaultAbilitySet->GiveToAbilitySystem(AbilitySystemComponent, GrantedDefaultAbilityHandles);
}

void ATestPlayerCharacter03_2::RemoveDefaultAbilities()
{
	if (!AbilitySystemComponent) return;

	for (const FGameplayAbilitySpecHandle& Handle : GrantedDefaultAbilityHandles)
	{
		if (Handle.IsValid())
		{
			AbilitySystemComponent->ClearAbility(Handle);
		}
	}
	GrantedDefaultAbilityHandles.Empty();
}

void ATestPlayerCharacter03_2::SetDefaultAbilitySet(UPlayerAbilitySet * NewAbilitySet)
{
	if (DefaultAbilitySet == NewAbilitySet)
	{
		return;
	}

	DefaultAbilitySet = NewAbilitySet;

	GiveDefaultAbilities();
}

void ATestPlayerCharacter03_2::SetupAbilityInputs(UEnhancedInputComponent * EnhancedInputComponent)
{
	if (!EnhancedInputComponent || !DefaultAbilitySet) return;

	for (const FPlayerAbilityConfig& Config : DefaultAbilitySet->Abilities)
	{
		if (Config.InputAction && Config.InputID != EAbilityInputID::None)
		{
			EnhancedInputComponent->BindAction(
				Config.InputAction,
				ETriggerEvent::Started,
				this,
				&ATestPlayerCharacter03_2::OnAbilityInputPressed,
				Config.InputID
			);

			EnhancedInputComponent->BindAction(
				Config.InputAction,
				ETriggerEvent::Completed,
				this,
				&ATestPlayerCharacter03_2::OnAbilityInputReleased,
				Config.InputID
			);
		}
	}
}

void ATestPlayerCharacter03_2::OnAbilityInputPressed(EAbilityInputID InputID)
{
	if (AbilitySystemComponent && InputID != EAbilityInputID::None)
	{
		AbilitySystemComponent->AbilityLocalInputPressed(static_cast<int32>(InputID));
	}
}

void ATestPlayerCharacter03_2::OnAbilityInputReleased(EAbilityInputID InputID)
{
	if (AbilitySystemComponent && InputID != EAbilityInputID::None)
	{
		AbilitySystemComponent->AbilityLocalInputReleased(static_cast<int32>(InputID));
	}
}
