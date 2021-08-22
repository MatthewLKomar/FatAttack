//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2020 (C) Bruno Xavier Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Savior3_Shared.h"
#include "SlotScreen/HUD_SlotPicker.h"

#include "Runtime/MoviePlayer/Public/MoviePlayer.h"
#include "Runtime/Engine/Classes/GameFramework/HUD.h"

#include "HUD_SaveLoadScreen.generated.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class USavior3;
class HUD_LoadScreenBlurWidget;
class HUD_LoadScreenMovieWidget;
class HUD_LoadScreenSplashWidget;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHUD_LoadScreen);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UCLASS(ClassGroup=Synaptech, Category="Synaptech", Blueprintable, BlueprintType, meta=(DisplayName="[SAVIOR] HUD"))
class SAVIOR3_API AHUD_SaviorUI : public AHUD {
	GENERATED_BODY()
	//
	friend class USavior3;
	friend class HUD_LoadScreenBlurWidget;
	friend class HUD_LoadScreenMovieWidget;
	friend class HUD_LoadScreenSplashWidget;
public:
	AHUD_SaviorUI();
	virtual ~AHUD_SaviorUI();
private:
	FDelegateHandle OnFinishMovie;
private:
	UPROPERTY() USlotPickerWidget* SlotPickerUI;
protected:
	static TSharedPtr<HUD_LoadScreenBlurWidget>LoadScreenBlurWidget;
	static TSharedPtr<HUD_LoadScreenMovieWidget>LoadScreenMovieWidget;
	static TSharedPtr<HUD_LoadScreenSplashWidget>LoadScreenSplashWidget;
protected:
	void BreakLoadScreenMovie();
public:
	/// :: PROPERTIES ::
	///
	/** Widget used as Slot Picker. */
	UPROPERTY(Category="Savior", EditAnywhere, BlueprintReadWrite, meta=(DisplayName="UI Slot Picker"))
	TSubclassOf<USlotPickerWidget>SlotPickerWidget;
	//
	//
	/** Event fired when a Load-Screen is created. */
	UPROPERTY(Category="Savior", BlueprintAssignable, meta=(DisplayName="[SAVIOR] On Begin Load-Screen"))
	FHUD_LoadScreen OnBeginLoadScreen;
	//
	/** Event fired when a Load-Screen is destroyed. */
	UPROPERTY(Category="Savior", BlueprintAssignable, meta=(DisplayName="[SAVIOR] On Finish Load-Screen"))
	FHUD_LoadScreen OnFinishLoadScreen;
public:
	/// :: FUNCTIONS ::
	///
	/** Event called when a Load-Screen is shown. */
	UFUNCTION(Category="Savior", BlueprintImplementableEvent, meta=(DisplayName="[SAVIOR] On Begin Load-Screen"))
	void OnBeganLoadScreen();
	//
	/** Event called when a Load-Screen is hidden. */
	UFUNCTION(Category="Savior", BlueprintImplementableEvent, meta=(DisplayName="[SAVIOR] On Finish Load-Screen"))
	void OnFinishedLoadScreen();
	//
	//
	/** Adds an auto-generated Save Slot Picker UI to the Viewport. */
	UFUNCTION(Category="Savior", BlueprintNativeEvent, BlueprintCallable, meta=(DisplayName="[SAVIOR] Show Slots UI"))
	void ShowSlotPickerHUD(const ESlotPickerMode PickerMode);
	//
	/** Removes auto-generated Slot Picker UI from the Viewport. */
	UFUNCTION(Category="Savior", BlueprintNativeEvent, BlueprintCallable, meta=(DisplayName="[SAVIOR] Hide Slots UI"))
	void HideSlotPickerHUD();
	//
	//
	/** Forces a loading screen canvas to draw on HUD. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Invoke Load Screen (Blur)"))
	void DisplayBlurLoadScreenHUD(const EThreadSafety TaskMode, const FText FeedbackText, const FSlateFontInfo &FeedbackFont, const FLinearColor &ProgressBarTint, const float BlurPower);
	//
	/** Forces a loading screen canvas to draw on HUD. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Invoke Load Screen (Splash)"))
	void DisplaySplashLoadScreenHUD(const EThreadSafety TaskMode, const FText FeedbackText, const FSlateFontInfo &FeedbackFont, const FLinearColor &ProgressBarTint, const FStringAssetReference SplashAsset, TEnumAsByte<EStretch::Type>SplashStretch);
	//
	/** Forces a loading screen canvas to draw on HUD. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Invoke Load Screen (Video)"))
	void DisplayMovieLoadScreenHUD(const EThreadSafety TaskMode, const FText FeedbackText, const FSlateFontInfo &FeedbackFont, const FLinearColor &ProgressBarTint, const FStringAssetReference SplashMovie, const bool ProgressBarOnMovie);
	//
	/** Forces a loading screen removal, useful when calculated workload never reached completion. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Remove Load Screen"))
	void RemoveLoadScreen();
public:
	static void StaticRemoveLoadScreen();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////