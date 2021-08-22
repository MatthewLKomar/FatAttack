//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2020 (C) Bruno Xavier Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LoadScreen/HUD_SaveLoadScreen.h"
#include "HUD_LoadScreenSplashWidget.h"
#include "HUD_LoadScreenMovieWidget.h"
#include "HUD_LoadScreenBlurWidget.h"

#include "Runtime/Slate/Public/Widgets/SWeakWidget.h"
#include "Runtime/Engine/Classes/Engine/GameViewportClient.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// HUD Load-Screen Constructors:

AHUD_SaviorUI::AHUD_SaviorUI(){}
AHUD_SaviorUI::~AHUD_SaviorUI(){}

TSharedPtr<HUD_LoadScreenBlurWidget>AHUD_SaviorUI::LoadScreenBlurWidget;
TSharedPtr<HUD_LoadScreenMovieWidget>AHUD_SaviorUI::LoadScreenMovieWidget;
TSharedPtr<HUD_LoadScreenSplashWidget>AHUD_SaviorUI::LoadScreenSplashWidget;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load-Screen Interface:

void AHUD_SaviorUI::DisplayBlurLoadScreenHUD(const EThreadSafety TaskMode, const FText FeedbackText, const FSlateFontInfo &FeedbackFont, const FLinearColor &ProgressBarTint, const float BlurPower) {
	if (GEngine==nullptr||GEngine->GameViewport==nullptr) {return;}
	//
	if (!LoadScreenBlurWidget.IsValid()) {
		SAssignNew(LoadScreenBlurWidget,HUD_LoadScreenBlurWidget)
		.ProgressBarTint(ProgressBarTint)
		.FeedbackFont(FeedbackFont)
		.FeedbackText(FeedbackText)
		.TaskMode(TaskMode)
		.Blur(BlurPower);
	}///
	//
	if (LoadScreenBlurWidget.IsValid()) {
		GEngine->GameViewport->AddViewportWidgetContent(LoadScreenBlurWidget.ToSharedRef(),10000);
		//
		LoadScreenBlurWidget->SetFeedbackText(FeedbackText.ToString());
		LoadScreenBlurWidget->SetProgressBarTint(ProgressBarTint);
		LoadScreenBlurWidget->SetFeedBackFont(FeedbackFont);
		LoadScreenBlurWidget->SetTaskMode(TaskMode);
		//
		LoadScreenBlurWidget->SetVisibility(EVisibility::Visible);
	}///
	//
	OnBeganLoadScreen();
	OnBeginLoadScreen.Broadcast();
}

void AHUD_SaviorUI::DisplaySplashLoadScreenHUD(const EThreadSafety TaskMode, const FText FeedbackText, const FSlateFontInfo &FeedbackFont, const FLinearColor &ProgressBarTint, const FStringAssetReference SplashAsset, TEnumAsByte<EStretch::Type> SplashStretch) {
	if (GEngine==nullptr||GEngine->GameViewport==nullptr) {return;}
	//
	if (!LoadScreenSplashWidget.IsValid()) {
		SAssignNew(LoadScreenSplashWidget,HUD_LoadScreenSplashWidget)
		.ProgressBarTint(ProgressBarTint)
		.SplashStretch(SplashStretch)
		.FeedbackFont(FeedbackFont)
		.FeedbackText(FeedbackText)
		.SplashAsset(SplashAsset)
		.TaskMode(TaskMode);
	}///
	//
	if (LoadScreenSplashWidget.IsValid()) {
		GEngine->GameViewport->AddViewportWidgetContent(LoadScreenSplashWidget.ToSharedRef(),10000);
		//
		LoadScreenSplashWidget->SetFeedbackText(FeedbackText.ToString());
		LoadScreenSplashWidget->SetProgressBarTint(ProgressBarTint);
		LoadScreenSplashWidget->SetFeedBackFont(FeedbackFont);
		LoadScreenSplashWidget->SetTaskMode(TaskMode);
		//
		LoadScreenSplashWidget->SetVisibility(EVisibility::Visible);
	}///
	//
	OnBeganLoadScreen();
	OnBeginLoadScreen.Broadcast();
}

void AHUD_SaviorUI::DisplayMovieLoadScreenHUD(const EThreadSafety TaskMode, const FText FeedbackText, const FSlateFontInfo &FeedbackFont, const FLinearColor &ProgressBarTint, const FStringAssetReference SplashMovie, const bool ProgressBarOnMovie) {
	if (GEngine==nullptr||GEngine->GameViewport==nullptr) {return;}
	if (GetMoviePlayer()->IsMovieCurrentlyPlaying()) {return;}
	//
	auto MoviePath = SplashMovie.ToString();
	MoviePath.Split(TEXT("/Movies/"),nullptr,&MoviePath);
	MoviePath.Split(TEXT("."),&MoviePath,nullptr);
	//
	FLoadingScreenAttributes Attributes;
	Attributes.MoviePaths.Add(MoviePath);
	Attributes.bWaitForManualStop = false;
	Attributes.bMoviesAreSkippable = false;
	Attributes.bAllowInEarlyStartup = false;
	Attributes.bAutoCompleteWhenLoadingCompletes = false;
	Attributes.PlaybackType = EMoviePlaybackType::MT_Normal;
	//
	//
	if (ProgressBarOnMovie) {
		if (!LoadScreenMovieWidget.IsValid()) {
			SAssignNew(LoadScreenMovieWidget,HUD_LoadScreenMovieWidget)
			.ProgressBarTint(ProgressBarTint)
			.FeedbackFont(FeedbackFont)
			.FeedbackText(FeedbackText)
			.TaskMode(TaskMode);
		}///
		//
		if (LoadScreenMovieWidget.IsValid()) {
			LoadScreenMovieWidget->SetFeedbackText(FeedbackText.ToString());
			LoadScreenMovieWidget->SetProgressBarTint(ProgressBarTint);
			LoadScreenMovieWidget->SetFeedBackFont(FeedbackFont);
			LoadScreenMovieWidget->SetTaskMode(TaskMode);
			//
			LoadScreenMovieWidget->SetVisibility(EVisibility::Visible);
			Attributes.WidgetLoadingScreen = LoadScreenMovieWidget;
		}///
	}///
	//
	GetMoviePlayer()->SetupLoadingScreen(Attributes);
	if (GetMoviePlayer()->LoadingScreenIsPrepared()) {
		OnFinishMovie = GetMoviePlayer()->OnMoviePlaybackFinished().AddUObject(this,&AHUD_SaviorUI::BreakLoadScreenMovie);
		GetMoviePlayer()->PlayMovie();
	}///
	//
	OnBeganLoadScreen();
	OnBeginLoadScreen.Broadcast();
}

void AHUD_SaviorUI::BreakLoadScreenMovie() {
	if ((USavior3::GetSaveProgress()>=100.f)&&(USavior3::GetLoadProgress()>=100.f)) {
		GetMoviePlayer()->OnMoviePlaybackFinished().Remove(OnFinishMovie); RemoveLoadScreen();
	} else {GetMoviePlayer()->PlayMovie();}
}

void AHUD_SaviorUI::RemoveLoadScreen() {
	AHUD_SaviorUI::StaticRemoveLoadScreen();
	//
	OnFinishedLoadScreen();
	OnFinishLoadScreen.Broadcast();
}

void AHUD_SaviorUI::StaticRemoveLoadScreen() {
	if (LoadScreenBlurWidget.IsValid()) {LoadScreenBlurWidget->SetVisibility(EVisibility::Collapsed);}
	if (LoadScreenMovieWidget.IsValid()) {LoadScreenMovieWidget->SetVisibility(EVisibility::Collapsed);}
	if (LoadScreenSplashWidget.IsValid()) {LoadScreenSplashWidget->SetVisibility(EVisibility::Collapsed);}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Slots Menu Interface:

void AHUD_SaviorUI::ShowSlotPickerHUD_Implementation(const ESlotPickerMode PickerMode) {
	const auto &Settings = GetMutableDefault<USavior3Settings>();
	//
	if (const auto &PC = GetOwningPlayerController()) {
		if (SlotPickerUI==nullptr) {
			SlotPickerUI = CreateWidget<USlotPickerWidget>(PC,SlotPickerWidget);
			SlotPickerUI->PickerMode = PickerMode;
			SlotPickerUI->AddToViewport(1000);
		}///
		//
		SlotPickerUI->SetVisibility(ESlateVisibility::Visible);
		//
		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		//
		PC->SetInputMode(InputMode);
		SlotPickerUI->SetUserFocus(PC);
	}///
}

void AHUD_SaviorUI::HideSlotPickerHUD_Implementation() {
	if (SlotPickerUI) {
		SlotPickerUI->RemoveFromViewport();
		SlotPickerUI->MarkPendingKill();
		SlotPickerUI->ConditionalBeginDestroy();
		SlotPickerUI = nullptr;
	}///
	//
	if (const auto &PC = GetOwningPlayerController()) {
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		//
		PC->SetInputMode(InputMode);
	}///
	//
	FSlateApplication::Get().SetUserFocusToGameViewport(0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////