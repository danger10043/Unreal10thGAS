// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/Test03/TestPlayerCharacter03_2.h"
#include "GAS/Data/PlayerAbilitySet.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "GAS/StatAttributeSet.h"


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

	UpdateGroundedTag();
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

void ATestPlayerCharacter03_2::BeginPlay()
{
	Super::BeginPlay();

	UpdateGroundedTag();

	if (AbilitySystemComponent && StatAttributeSet)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			UStatAttributeSet::GetMoveSpeedAttribute()).AddUObject(
				this, &ATestPlayerCharacter03_2::OnMoveSpeedChanged);

		GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed * StatAttributeSet->GetMoveSpeed() / 100.0f;
	}
}

void ATestPlayerCharacter03_2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!AbilitySystemComponent) return;

	static const FGameplayTag MovingTag = FGameplayTag::RequestGameplayTag(FName("GAS.State.Moving"));
	constexpr float MoveThreshold = 10.0f;
	const bool bIsMoving = GetVelocity().SizeSquared2D() >= FMath::Square(MoveThreshold);
	const bool bHasMovingTag = AbilitySystemComponent->HasMatchingGameplayTag(MovingTag);

	if (bIsMoving && !bHasMovingTag)
	{
		AbilitySystemComponent->AddLooseGameplayTag(MovingTag);
	}
	else if (!bIsMoving && bHasMovingTag)
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(MovingTag);
	}
}

void ATestPlayerCharacter03_2::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	UpdateGroundedTag();
}

void ATestPlayerCharacter03_2::UpdateGroundedTag()
{
	if (!AbilitySystemComponent) return;

	static const FGameplayTag GroundedTag = FGameplayTag::RequestGameplayTag(FName("GAS.State.Grounded"));

	const bool bOnGround = GetCharacterMovement()->IsMovingOnGround();
	const bool bHasGroundTag = AbilitySystemComponent->HasMatchingGameplayTag(GroundedTag);

	if (bOnGround && !bHasGroundTag)
	{
		AbilitySystemComponent->AddLooseGameplayTag(GroundedTag);
	}
	else if (!bOnGround && bHasGroundTag)
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(GroundedTag);
	}
}

void ATestPlayerCharacter03_2::OnMoveSpeedChanged(const FOnAttributeChangeData& InData)
{
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed * InData.NewValue / 100.0f;
}
