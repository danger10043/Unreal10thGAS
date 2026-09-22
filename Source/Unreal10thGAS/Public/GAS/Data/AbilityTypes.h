// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbilityTypes.generated.h"

class UGameplayAbility;
class UInputAction;

/**
 * 어빌리티 입력 식별용 열거형 (GAS InputID로 변환되어 바인딩됨)
 */
UENUM(BlueprintType)
enum class EAbilityInputID : uint8
{
	None = 0	UMETA(DisplayName = "None"),
	Sprint		UMETA(DisplayName = "Sprint"),
	ChargeJump	UMETA(DisplayName = "Charge Jump"),
	Blink		UMETA(DisplayName = "Blink"),
};

/**
 * 어빌리티 개별 등록 설정 구조체
 */
USTRUCT(BlueprintType)
struct FPlayerAbilityConfig
{
	GENERATED_BODY()

	/** 부여할 어빌리티 클래스 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<UGameplayAbility> AbilityClass = nullptr;

	/** 어빌리티 기본 레벨 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability", meta = (ClampMin = "1"))
	int32 AbilityLevel = 1;

	/** 바인딩할 어빌리티 입력 식별자 (입력이 필요 없는 패시브의 경우 None) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	EAbilityInputID InputID = EAbilityInputID::None;

	/** 트리거용 향상된 입력 액션 (IA). 패시브이거나 직접 호출 어빌리티인 경우 비워둘 수 있음 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> InputAction = nullptr;

	/** 어빌리티 부여 즉시 자동 활성화 여부 (패시브 등) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	bool bActivateOnGranted = false;
};
