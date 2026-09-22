// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Test/TestBaseActor.h"
#include "Test04.generated.h"

class UGameplayEffect;
/**
 * 
 */
UCLASS()
class UNREAL10THGAS_API ATest04 : public ATestBaseActor
{
	GENERATED_BODY()
	
public:

	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(CallInEditor, Category = "GAS")
	void TestScanAndHit();

	UFUNCTION(CallInEditor, Category = "GAS")
	void TestDebuff();

protected:
	void DrawDebugRadius();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> HitEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> DebuffEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	float EffectLevel = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	float Radius = 500.0f;
};
