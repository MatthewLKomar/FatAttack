//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2020 (C) Bruno Xavier Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "HUD_LoadScreenBlurWidget.h"
#include "HUD_LoadScreenStyle.h"

#include "Runtime/SlateCore/Public/Widgets/SOverlay.h"
#include "Runtime/SlateCore/Public/Widgets/SBoxPanel.h"
#include "Runtime/Slate/Public/Widgets/Layout/SSpacer.h"
#include "Runtime/Slate/Public/Widgets/Text/STextBlock.h"
#include "Runtime/Slate/Public/Widgets/Images/SThrobber.h"
#include "Runtime/Slate/Public/Widgets/Layout/SSafeZone.h"
#include "Runtime/Slate/Public/Widgets/Layout/SDPIScaler.h"
#include "Runtime/Slate/Public/Widgets/Layout/SBackgroundBlur.h"
#include "Runtime/Slate/Public/Widgets/Notifications/SProgressBar.h"

#include "Engine/UserInterfaceSettings.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define LOCTEXT_NAMESPACE "HUD_LoadScreen"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HUD_LoadScreenBlurWidget::Construct(const FArguments &InArgs) {
	ProgressBarTint = InArgs._ProgressBarTint;
	FeedbackFont = InArgs._FeedbackFont;
	FeedbackText = InArgs._FeedbackText;
	TaskMode = InArgs._TaskMode;
	Blur = InArgs._Blur;
	DPIScale = 1.f;
	//
	//
	TSharedRef<SOverlay>HUD_Overlay = SNew(SOverlay);
	//
	HUD_Overlay->AddSlot(10000)
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	[
		SAssignNew(Background,SBackgroundBlur)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.BlurStrength(Blur)
	];
	//
	HUD_Overlay->AddSlot(10001)
	.VAlign(VAlign_Bottom)
	.HAlign(HAlign_Fill)
	[
		SNew(SSafeZone)
		.IsTitleSafe(true)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Bottom)
		[
			SNew(SDPIScaler)
			.DPIScale(this,&HUD_LoadScreenBlurWidget::GetDPIScale)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot().AutoHeight()
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					.Padding(FMargin(10,0.0f,0,0))
					.VAlign(VAlign_Center)
					.AutoWidth()
					[
						SNew(SCircularThrobber)
						.Radius((FeedbackFont.Size*96.f/72.f)/2.f)
					]
					+SHorizontalBox::Slot()
					.Padding(FMargin(10.0f,0.0f,0,0))
					.VAlign(VAlign_Center)
					.AutoWidth()
					[
						SAssignNew(Feedback,STextBlock)
						.Text(FeedbackText)
						.Font(FeedbackFont)
					]
					+SHorizontalBox::Slot()
					.HAlign(HAlign_Fill)
					.FillWidth(1)
					[
						SNew(SSpacer)
						.Size(FVector2D(1.0f,1.0f))
					]
				]
				+SVerticalBox::Slot().AutoHeight()
				[
					SAssignNew(ProgressBar,SProgressBar)
					.FillColorAndOpacity(FSlateColor(ProgressBarTint))
					.Style(&FSaviorLoadScreenStyle::IGet().GetWidgetStyle<FProgressBarStyle>("SaviorProgressBar"))
				]
			]
		]
	];
	//
	//
	ChildSlot
	[
		HUD_Overlay
	];
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HUD_LoadScreenBlurWidget::Tick(const FGeometry &AllottedGeometry, const double InCurrentTime, const float InDeltaTime) {
	const FVector2D &LocalSize = AllottedGeometry.GetLocalSize();
	FIntPoint Size((int32)LocalSize.X,(int32)LocalSize.Y);
	//
	const float NewDPI = GetDefault<UUserInterfaceSettings>()->GetDPIScaleBasedOnSize(Size);
	if (NewDPI!=DPIScale) {DPIScale=NewDPI; SlatePrepass(1.0f);}
	//
	if (InCurrentTime > Ticker) {
		Ticker = InCurrentTime+1.f;
		if (FeedbackText.ToString().Contains(TEXT("..."))) {FeedbackText = FText::FromString(FeedbackText.ToString().Replace(TEXT("..."),TEXT(".  ")));} else
		if (FeedbackText.ToString().Contains(TEXT(".. "))) {FeedbackText = FText::FromString(FeedbackText.ToString().Replace(TEXT(".. "),TEXT("...")));} else
		if (FeedbackText.ToString().Contains(TEXT(".  "))) {FeedbackText = FText::FromString(FeedbackText.ToString().Replace(TEXT(".  "),TEXT(".. ")));}
		Feedback->SetText(FeedbackText);
	}///
	//
	ProgressBar->SetPercent(GetWorkloadProgress());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float HUD_LoadScreenBlurWidget::GetDPIScale() const {
	return DPIScale;
}

TOptional<float> HUD_LoadScreenBlurWidget::GetWorkloadProgress() const {
	switch (TaskMode) {
		case EThreadSafety::SynchronousSaving:
		case EThreadSafety::AsynchronousSaving:
			return TOptional<float>(USavior3::GetSaveProgress()/100);
		break;
		case EThreadSafety::SynchronousLoading:
		case EThreadSafety::AsynchronousLoading:
			return TOptional<float>(USavior3::GetLoadProgress()/100);
		break;
	default: return 0.f; break;}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HUD_LoadScreenBlurWidget::SetProgressBarTint(const FLinearColor &NewColor) {
	ProgressBarTint = NewColor;
}

void HUD_LoadScreenBlurWidget::SetFeedBackFont(const FSlateFontInfo &NewFont) {
	FeedbackFont = NewFont;
}

void HUD_LoadScreenBlurWidget::SetFeedbackText(const FString &NewText) {
	FeedbackText = FText::FromString(NewText);
}

void HUD_LoadScreenBlurWidget::SetTaskMode(const EThreadSafety NewMode) {
	TaskMode = NewMode;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////