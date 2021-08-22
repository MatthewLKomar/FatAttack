//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2020 (C) Bruno Xavier Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Engine/Texture2D.h"
#include "Runtime/Slate/Public/SlateBasics.h"
#include "Runtime/SlateCore/Public/Fonts/SlateFontInfo.h"
#include "Runtime/Slate/Public/Widgets/Layout/SScaleBox.h"
#include "Runtime/SlateCore/Public/Widgets/SCompoundWidget.h"
#include "Runtime/Engine/Public/Slate/DeferredCleanupSlateBrush.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SThrobber;
class SProgressBar;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class HUD_LoadScreenSplashWidget : public SCompoundWidget {
private:
	TSharedPtr<FDeferredCleanupSlateBrush>SplashBrush;
	TSharedPtr<SProgressBar>ProgressBar;
	TSharedPtr<STextBlock>Feedback;
	TSharedPtr<SThrobber>Throbber;
	//
	TEnumAsByte<EStretch::Type> SplashStretch;
	FStringAssetReference SplashAsset;
	FLinearColor ProgressBarTint;
	FSlateFontInfo FeedbackFont;
	EThreadSafety TaskMode;
	FText FeedbackText;
	//
	float Ticker = 0.f;
	float DPIScale = 1.f;
	float GetDPIScale() const;
	TOptional<float> GetWorkloadProgress() const;
public:
	SLATE_BEGIN_ARGS(HUD_LoadScreenSplashWidget)
	: _SplashStretch(), _SplashAsset(), _ProgressBarTint(), _FeedbackFont(), _TaskMode(), _FeedbackText()
	{}//
		SLATE_ARGUMENT(TEnumAsByte<EStretch::Type>,SplashStretch);
		SLATE_ARGUMENT(FStringAssetReference,SplashAsset);
		SLATE_ARGUMENT(FLinearColor,ProgressBarTint);
		SLATE_ARGUMENT(FSlateFontInfo,FeedbackFont);
		SLATE_ARGUMENT(EThreadSafety,TaskMode);
		SLATE_ARGUMENT(FText,FeedbackText);
	SLATE_END_ARGS()
	//
	void Construct(const FArguments &InArgs);
	virtual void Tick(const FGeometry &AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
public:
	void SetProgressBarTint(const FLinearColor &NewColor);
	void SetFeedBackFont(const FSlateFontInfo &NewFont);
	void SetTaskMode(const EThreadSafety NewMode);
	void SetFeedbackText(const FString &NewText);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////