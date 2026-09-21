// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/TestPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Framework/TestGASHUD.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "GAS/StatAttributeSet.h"

ATestPlayerCharacter::ATestPlayerCharacter()
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

void ATestPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!MoveSpeedChangedDelegateHandle.IsValid())
	{
		FOnGameplayAttributeValueChange& MoveSpeedChangedDelegate = 
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UStatAttributeSet::GetMoveSpeedAttribute());
		MoveSpeedChangedDelegateHandle = MoveSpeedChangedDelegate.AddUObject(this, &ATestPlayerCharacter::OnMoveSpeedChanged);
	}
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		const float Ratio = (StatAttributeSet ? StatAttributeSet->GetMoveSpeed() : 100.0f) / 100.0f;
		MovementComp->MaxWalkSpeed = BaseWalkSpeed * Ratio;
	}
	
	if (APlayerController* PC = Cast<APlayerController>(NewController))
	{
		// 플레이어 일때만 처리
		if (ATestGASHUD* TestGASHUD = Cast<ATestGASHUD>(PC->GetHUD()))
		{
			TestGASHUD->InitHUD(this);	// 레이스 컨디션 대비
		}
	}

	GiveDefaultAbilities();
}

void ATestPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* Enhanced = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (SprintAction)
		{
			Enhanced->BindAction(SprintAction, ETriggerEvent::Started, this, &ATestPlayerCharacter::OnSprintInputStart);
			Enhanced->BindAction(SprintAction, ETriggerEvent::Completed, this, &ATestPlayerCharacter::OnSprintInputCompleted);
		}
	}
}

void ATestPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!AbilitySystemComponent) return;

	static const FGameplayTag StateMovingTag = FGameplayTag::RequestGameplayTag(FName("GAS.State.Moving"), false);
	const bool bIsMoving = GetVelocity().SizeSquared2D() >= FMath::Square(MoveThreshold);
	const bool bHasMovingTag = AbilitySystemComponent->HasMatchingGameplayTag(StateMovingTag);
	if (bIsMoving && !bHasMovingTag)
	{
		AbilitySystemComponent->AddLooseGameplayTag(StateMovingTag);
	}
	else if (!bIsMoving && bHasMovingTag)
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(StateMovingTag);
	}
	
}

void ATestPlayerCharacter::GiveDefaultAbilities()
{
	if (!AbilitySystemComponent) return;

	if (!AbilitySystemComponent->AbilityActorInfo.IsValid())	// 초기화 되지 않았으면 한번더
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

	if (DefaultAbilityClass)
	{
		FGameplayAbilitySpec Spec(DefaultAbilityClass, DefaultAlilityLevel, SprintInputID);
		FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(Spec);
	}

}

void ATestPlayerCharacter::OnSprintInputStart()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityLocalInputPressed(SprintInputID);
	}
}

void ATestPlayerCharacter::OnSprintInputCompleted()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityLocalInputReleased(SprintInputID);
	}
}

void ATestPlayerCharacter::OnMoveSpeedChanged(const FOnAttributeChangeData & InData)
{
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		const float Ratio = InData.NewValue / 100.0f;
		MovementComp->MaxWalkSpeed = BaseWalkSpeed * Ratio;
	}
}
