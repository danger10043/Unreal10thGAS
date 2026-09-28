#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UAbilitySystemComponent;
class UPlayerAbilitySet;
class UStatAttributeSet;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;
struct FGameplayTag;

UCLASS()
class UNREAL10THGAS_API APlayerCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UStatAttributeSet* GetStatAttributeSet() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Jump|Effects")
	void PlayFullChargeFlash();

protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void OnAbilityInputPressed(int32 InputID);
	void OnAbilityInputReleased(int32 InputID);
	void OnAbilityInputCanceled(int32 InputID);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> PlayerInputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UStatAttributeSet> StatAttributeSet;

	void GiveDefaultAbilities();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS|Ability")
	TObjectPtr<UPlayerAbilitySet> DefaultAbilitySet;

	void OnJumpChargeTagChanged(FGameplayTag Tag, int32 NewCount);

	FDelegateHandle JumpChargeTagDelegateHandle;
	bool bJumpChargeSlowApplied = false;
	float WalkSpeedBeforeJumpCharge = 0.0f;
	float CrouchedSpeedBeforeJumpCharge = 0.0f;
};