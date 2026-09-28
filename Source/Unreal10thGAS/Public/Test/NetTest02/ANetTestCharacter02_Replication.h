// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Test/Test03/TestPlayerCharacter03.h"
#include "ANetTestCharacter02_Replication.generated.h"

class UInputMappingContext;
/**
 * 
 */
UCLASS()
class UNREAL10THGAS_API AANetTestCharacter02_Replication : public ATestPlayerCharacter03
{
	GENERATED_BODY()
	
protected:
	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRepNotify_Level();

	UFUNCTION(CallInEditor, Category = "Test")
	void TestLevelUp();

private:
	UFUNCTION()
	void Test1();

	UFUNCTION()
	void Test2();

	UFUNCTION()
	void Test3();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Test", ReplicatedUsing = OnRepNotify_Level)	// Level이 리플리케이션이 될 떄 OnRepNotify_Level이 실행
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, Category = "Test", Replicated)	// 리플리케이션이 된다고 표시
	float Health = 100.0f;
	
	UPROPERTY(VisibleAnywhere, Category = "Test", Replicated)
	float Exp = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> TestMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Test1;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Test2;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Test3;
};
