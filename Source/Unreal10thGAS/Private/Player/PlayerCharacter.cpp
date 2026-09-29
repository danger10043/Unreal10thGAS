#include "Player/PlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbilitySpec.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GAS/StatAttributeSet.h"
#include "GAS/Data/PlayerAbilitySet.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Widget/HealthBarWidget.h"
#include "Framework/TestGASHUD.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PostUpdateWork;
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 600.0f;
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	HealthBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarComponent"));
	HealthBarComponent->SetupAttachment(RootComponent);
	HealthBarComponent->SetWidgetSpace(EWidgetSpace::World);
	HealthBarComponent->SetDrawSize(FVector2D(900.0f, 200.0f));
	HealthBarComponent->SetPivot(FVector2D(0.5f, 0.5f));
	HealthBarComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 140.0f));
	HealthBarComponent->SetRelativeScale3D(FVector(0.25f));
	HealthBarComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HealthBarComponent->SetGenerateOverlapEvents(false);
	HealthBarComponent->SetCastShadow(false);
	HealthBarComponent->SetVisibility(false);

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	StatAttributeSet = CreateDefaultSubobject<UStatAttributeSet>(TEXT("StatAttributeSet"));

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() == NM_DedicatedServer || !HealthBarComponent)
	{
		return;
	}

	HealthBarComponent->InitWidget();
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetNetMode() == NM_DedicatedServer || !HealthBarComponent)
	{
		return;
	}

	APlayerController* LocalPC = UGameplayStatics::GetPlayerController(this, 0);
	if (!LocalPC || !LocalPC->IsLocalController() || LocalPC->GetPawn() == this)
	{
		HealthBarComponent->SetVisibility(false);
		return;
	}

	UHealthBarWidget* HealthBar = Cast<UHealthBarWidget>(HealthBarComponent->GetUserWidgetObject());
	const UStatAttributeSet* Attributes = GetStatAttributeSet();
	if (!HealthBar || !Attributes)
	{
		HealthBarComponent->SetVisibility(false);
		return;
	}

	const FVector WidgetLocation = GetActorLocation() + FVector(0.0f, 0.0f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 30.0f);
	HealthBarComponent->SetWorldLocation(WidgetLocation);

	FVector CameraLocation;
	FRotator CameraRotation;
	LocalPC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	const FVector ToCamera = CameraLocation - WidgetLocation;
	if (!ToCamera.IsNearlyZero())
	{
		HealthBarComponent->SetWorldRotation(ToCamera.Rotation());
	}

	HealthBar->SetHealth(Attributes->GetHealth(), Attributes->GetMaxHealth());
	HealthBarComponent->SetVisibility(true);
}

UAbilitySystemComponent* APlayerCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

const UStatAttributeSet* APlayerCharacter::GetStatAttributeSet() const
{
	return AbilitySystemComponent
		? AbilitySystemComponent->GetSet<UStatAttributeSet>()
		: nullptr;
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!ensure(EnhancedInput)) return;

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	ULocalPlayer* LocalPlayer = PlayerController ? PlayerController->GetLocalPlayer() : nullptr;
	if (LocalPlayer && PlayerInputMappingContext)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->AddMappingContext(PlayerInputMappingContext, 0);
		}
	}

	if (MoveAction)
	{
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
	}
	if (LookAction)
	{
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
	}
	
	if (DefaultAbilitySet)
	{
		for (const FPlayerAbilityConfig& Config : DefaultAbilitySet->Abilities)
		{
			if (!Config.AbilityClass || !Config.InputAction || Config.InputID == EAbilityInputID::None)
			{
				continue;
			}

			if (Config.AbilityClass->HasAnyClassFlags(CLASS_Abstract))
			{
				continue;
			}

			const int32 InputID = static_cast<int32>(Config.InputID);

			EnhancedInput->BindAction(
				Config.InputAction, ETriggerEvent::Started,
				this, &APlayerCharacter::OnAbilityInputPressed, InputID);

			EnhancedInput->BindAction(
				Config.InputAction, ETriggerEvent::Completed,
				this, &APlayerCharacter::OnAbilityInputReleased, InputID);

			EnhancedInput->BindAction(
				Config.InputAction, ETriggerEvent::Canceled,
				this, &APlayerCharacter::OnAbilityInputCanceled, InputID);
		}
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller) return;

	const FVector2D MovementInput = Value.Get<FVector2D>();
	const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementInput.Y);
	AddMovementInput(RightDirection, MovementInput.X);
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	if (!Controller) return;
	const FVector2D LookInput = Value.Get<FVector2D>();
	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(-LookInput.Y);
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	SpringArm->bUsePawnControlRotation = true;
	Camera->bUsePawnControlRotation = false;

	if (ensure(AbilitySystemComponent && StatAttributeSet))
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		const FGameplayTag JumpChargeTag =
			FGameplayTag::RequestGameplayTag(TEXT("Player.State.JumpCharge"));

		if (!JumpChargeTagDelegateHandle.IsValid())
		{
			JumpChargeTagDelegateHandle = AbilitySystemComponent->RegisterGameplayTagEvent(
				JumpChargeTag, EGameplayTagEventType::NewOrRemoved)
				.AddUObject(this, &APlayerCharacter::OnJumpChargeTagChanged);
		}

		OnJumpChargeTagChanged(JumpChargeTag, AbilitySystemComponent->GetTagCount(JumpChargeTag));

		GiveDefaultAbilities();

		if (APlayerController* PC = Cast<APlayerController>(NewController))
		{
			if (PC->IsLocalController())
			{
				if (ATestGASHUD* HUD = Cast<ATestGASHUD>(PC->GetHUD()))
				{
					HUD->InitHUD(this);
				}
			}
		}
	}
}

void APlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	SpringArm->bUsePawnControlRotation = true;
	Camera->bUsePawnControlRotation = true;

	if (ensure(AbilitySystemComponent && StatAttributeSet))
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		const FGameplayTag JumpChargeTag = FGameplayTag::RequestGameplayTag(TEXT("Player.State.JumpCharge"));

		if (!JumpChargeTagDelegateHandle.IsValid())
		{
			JumpChargeTagDelegateHandle = AbilitySystemComponent->RegisterGameplayTagEvent(
				JumpChargeTag,
				EGameplayTagEventType::NewOrRemoved).AddUObject(
					this, &APlayerCharacter::OnJumpChargeTagChanged
				);

			OnJumpChargeTagChanged(JumpChargeTag, AbilitySystemComponent->GetTagCount(JumpChargeTag));
		}

		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			if (PC->IsLocalController())
			{
				if (ATestGASHUD* HUD = Cast<ATestGASHUD>(PC->GetHUD()))
				{
					HUD->InitHUD(this);
				}
			}
		}
	}
}

void APlayerCharacter::GiveDefaultAbilities()
{
	if (!AbilitySystemComponent || !DefaultAbilitySet)
	{
		return;
	}
	
	for (const FPlayerAbilityConfig& Config : DefaultAbilitySet->Abilities)
	{
		if (!Config.AbilityClass)
		{
			continue;
		}
		if (Config.AbilityClass->HasAnyClassFlags(CLASS_Abstract))
		{
			continue;
		}
		if (AbilitySystemComponent->FindAbilitySpecFromClass(Config.AbilityClass))
		{
			continue;
		}

		const int32 InputID = Config.InputID == EAbilityInputID::None ? INDEX_NONE : static_cast<int32>(Config.InputID);

		FGameplayAbilitySpec AbilitySpec(
			Config.AbilityClass,
			FMath::Max(1, Config.AbilityLevel),
			InputID,
			this
		);

		const FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(AbilitySpec);

		if (Handle.IsValid() && Config.bActivateOnGranted)
		{
			AbilitySystemComponent->TryActivateAbility(Handle);
		}
	}
}

void APlayerCharacter::OnAbilityInputPressed(int32 InputID)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityLocalInputPressed(InputID);
	}
}

void APlayerCharacter::OnAbilityInputReleased(int32 InputID)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityLocalInputReleased(InputID);
	}
}

void APlayerCharacter::OnAbilityInputCanceled(int32 InputID)
{
	if (!AbilitySystemComponent) return;

	TArray<FGameplayAbilitySpecHandle> HandlesToCancel;

	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if (Spec.InputID == InputID && Spec.IsActive())
		{
			HandlesToCancel.Add(Spec.Handle);
		}
	}

	for (const FGameplayAbilitySpecHandle& Handle : HandlesToCancel)
	{
		AbilitySystemComponent->CancelAbilityHandle(Handle);
	}

	AbilitySystemComponent->AbilityLocalInputReleased(InputID);
}

void APlayerCharacter::OnJumpChargeTagChanged(FGameplayTag Tag, int32 NewCount)
{
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (!Movement) return;

	const bool bIsCharging = NewCount > 0;
	if (bIsCharging == bJumpChargeSlowApplied) return;

	bJumpChargeSlowApplied = bIsCharging;

	if (bIsCharging)
	{
		WalkSpeedBeforeJumpCharge = Movement->MaxWalkSpeed;
		CrouchedSpeedBeforeJumpCharge = Movement->MaxWalkSpeedCrouched;

		Movement->MaxWalkSpeed = WalkSpeedBeforeJumpCharge * 0.5f;
		Movement->MaxWalkSpeedCrouched = CrouchedSpeedBeforeJumpCharge * 0.5f;
	}
	else
	{
		Movement->MaxWalkSpeed = WalkSpeedBeforeJumpCharge;
		Movement->MaxWalkSpeedCrouched = CrouchedSpeedBeforeJumpCharge;
	}
}