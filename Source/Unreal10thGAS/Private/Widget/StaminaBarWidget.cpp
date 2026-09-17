// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/StaminaBarWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UStaminaBarWidget::SetStamina(float CurrentStamina, float MaxStamina)
{
	if (!IsValid(CurrentStaminaText) || !IsValid(MaxStaminaText) || !IsValid(StaminaProgressBar)) return;

	const float ClampedCurrentStamina = FMath::Clamp(CurrentStamina, 0.0f, MaxStamina);

	const float StaminaRatio = MaxStamina > 0.0f ? ClampedCurrentStamina / MaxStamina : 0.0f;

	CurrentStaminaText->SetText(FText::AsNumber(FMath::RoundToInt(ClampedCurrentStamina)));
	MaxStaminaText->SetText(FText::AsNumber(FMath::RoundToInt(MaxStamina)));
	StaminaProgressBar->SetPercent(StaminaRatio);
}
