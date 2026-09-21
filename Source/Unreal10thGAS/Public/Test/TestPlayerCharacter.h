// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Test/TestCharacter.h"
#include "GameplayAbilitySpecHandle.h"
#include "TestPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
/**
 * 
 */
UCLASS()
class UNREAL10THGAS_API ATestPlayerCharacter : public ATestCharacter
{
	GENERATED_BODY()
	
public:
	ATestPlayerCharacter();

protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;

	// 기본 어빌리티를 부여하는 함수
	virtual void GiveDefaultAbilities();

	void OnSprintInputStart();		// 스프린트 입력 시작 콜백
	void OnSprintInputCompleted();	// 스프린트 입력 종료 콜백
	void OnMoveSpeedChanged(const struct FOnAttributeChangeData& InData);	// MoveSpeed 어트리뷰트 변경 콜백

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float BaseWalkSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float MoveThreshold = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<UGameplayAbility> DefaultAbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability", meta = (Clamp = "1"))
	int32 DefaultAlilityLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> SprintAction;

private:
	UPROPERTY(Transient)
	FGameplayAbilitySpecHandle SprintAbilityHandle;

	FDelegateHandle MoveSpeedChangedDelegateHandle;

	static constexpr int32 SprintInputID = 100;
};
