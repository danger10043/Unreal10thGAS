// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Test/TestCharacter.h"
#include "GAS/Data/AbilityTypes.h"
#include "GameplayAbilitySpecHandle.h"
#include "TestPlayerCharacter03_2.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UPlayerAbilitySet;
class UEnhancedInputComponent;

/**
 * 기본 어빌리티 셋을 설정하기 위한 최소 코드만 구현(어빌리티 발동 안됨)
 */
UCLASS()
class UNREAL10THGAS_API ATestPlayerCharacter03_2 : public ATestCharacter
{
	GENERATED_BODY()

public:
	ATestPlayerCharacter03_2();

protected:
	// 주요 가상함수 -------------------------------------------------------------------------
	virtual void PossessedBy(AController* NewController) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	// --------------------------------------------------------------------------------------

	// 기본 어빌리티 부여용 함수 --------------------------------------------------------------
	/** DefaultAbilitySet에 등록된 어빌리티들을 부여합니다.(기존에 부여된 기본 어빌리티가 있으면 제거 후 다시 부여) */
	virtual void GiveDefaultAbilities();

	/** 부여되어 있던 기본 어빌리티들을 모두 제거합니다. */
	virtual void RemoveDefaultAbilities();

	/** 런타임에 어빌리티 세트를 교체하고 즉시 반영합니다. */
	void SetDefaultAbilitySet(UPlayerAbilitySet* NewAbilitySet);

	/** DefaultAbilitySet에 설정된 InputAction들을 EnhancedInputComponent에 바인딩합니다. */
	virtual void SetupAbilityInputs(UEnhancedInputComponent* EnhancedInputComponent);
	// --------------------------------------------------------------------------------------

	// 콜백 함수들----------------------------------------------------------------------------
	/** 어빌리티 입력 시작 콜백 */
	virtual void OnAbilityInputPressed(EAbilityInputID InputID);

	/** 어빌리티 입력 종료 콜백 */
	virtual void OnAbilityInputReleased(EAbilityInputID InputID);
	// --------------------------------------------------------------------------------------
	

protected:
	// 3인칭 카메라용 컴포넌트들
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	/** 게임 시작/빙의 시 기본 부여할 어빌리티 세트 데이터 에셋 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS|Ability")
	TObjectPtr<UPlayerAbilitySet> DefaultAbilitySet;

private:
	/** 부여된 디폴트 어빌리티들의 핸들 목록 */
	UPROPERTY(Transient)
	TArray<FGameplayAbilitySpecHandle> GrantedDefaultAbilityHandles;
};
