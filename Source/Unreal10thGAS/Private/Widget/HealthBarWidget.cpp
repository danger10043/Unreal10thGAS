// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/HealthBarWidget.h"

#include "Animation/WidgetAnimation.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UHealthBarWidget::SetHealth(float CurrentHealth, float MaxHealth)
{
	if (!IsValid(CurrentHealthText) || !IsValid(MaxHealthText) || !IsValid(HealthProgressBar))
	{
		return;
	}

	const bool bHealthDecreased = bHasPreviousHealth && CurrentHealth < PreviousCurrentHealth;
	PreviousCurrentHealth = CurrentHealth;
	bHasPreviousHealth = true;

	const float ClampedCurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);

	CurrentHealthText->SetText(FText::AsNumber(FMath::RoundToInt(ClampedCurrentHealth)));
	MaxHealthText->SetText(FText::AsNumber(FMath::RoundToInt(MaxHealth)));

	const float HealthRatio = MaxHealth > 0.0f ? FMath::Clamp(ClampedCurrentHealth / MaxHealth, 0.0f, 1.0f) : 0.0f;
	constexpr float ProgressStep = 0.05f;
	const float RoundedUpRatio = FMath::Clamp(FMath::CeilToFloat(HealthRatio / ProgressStep) * ProgressStep, 0.0f, 1.0f);

	HealthProgressBar->SetPercent(RoundedUpRatio);

	if (bHealthDecreased)
	{
		PlayTakeDamageAnimation();
	}
}

void UHealthBarWidget::PlayTakeDamageAnimation()
{
	if (!IsValid(TakeDamageAnimation)) return;

	StopAnimation(TakeDamageAnimation);
	PlayAnimation(TakeDamageAnimation);
}

