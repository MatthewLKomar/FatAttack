//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//			Copyright 2020 (C) Bruno Xavier Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SaviorAsync.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Open Level:

USAVIOR_OpenLevel* USAVIOR_OpenLevel::OpenLevel(UObject* Context, AHUD_SaviorUI* HUD, USavior3* Slot, TSoftObjectPtr<UWorld>LevelToOpen) {
	USAVIOR_OpenLevel* OBJ = NewObject<USAVIOR_OpenLevel>();
	ESaviorResult Result;
	//
	FString Name;
	LevelToOpen.ToString().Split(TEXT("."),nullptr,&Name,ESearchCase::IgnoreCase,ESearchDir::FromEnd);
	//
	OBJ->Level = (LevelToOpen.IsNull()) ? TEXT("None") : *Name;
	OBJ->Outer = Context;
	OBJ->SHUD = HUD;
	//
	if (Slot) {
		if (!Slot->IsInstance()||(Slot->IsInstance()&&(Slot->GetThreadSafety()!=EThreadSafety::IsCurrentlyThreadSafe))) {
			OBJ->Target = USavior3::NewSlotInstance(Context,Slot,Result);
		} else {OBJ->Target=Slot;}
	}///
	//
	return OBJ;
}

void USAVIOR_OpenLevel::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	if (SHUD==nullptr||SHUD->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target HUD."),ELogVerbosity::Error); return;}
	//
	Execute();
}

void USAVIOR_OpenLevel::Execute() {
	switch (Target->LoadScreenMode) {
		case ELoadScreenMode::BackgroundBlur:
		{
			SHUD->DisplayBlurLoadScreenHUD(EThreadSafety::SynchronousLoading,Target->FeedbackLOAD,Target->FeedbackFont,Target->ProgressBarTint,Target->BackBlurPower);
		}	break;
		//
		case ELoadScreenMode::SplashScreen:
		{
			SHUD->DisplaySplashLoadScreenHUD(EThreadSafety::SynchronousLoading,Target->FeedbackLOAD,Target->FeedbackFont,Target->ProgressBarTint,Target->SplashImage,Target->SplashStretch);
		}	break;
		//
		case ELoadScreenMode::MoviePlayer:
		{
			SHUD->DisplayMovieLoadScreenHUD(EThreadSafety::SynchronousLoading,Target->FeedbackLOAD,Target->FeedbackFont,Target->ProgressBarTint,Target->SplashMovie,Target->ProgressBarOnMovie);
		}	break;
	default: break;}
	//
	UGameplayStatics::OpenLevel(Outer,Level);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Open Level [+Callbacks]:

USAVIOR_OpenLevel_Callback* USAVIOR_OpenLevel_Callback::OpenLevel(UObject* Context, AHUD_SaviorUI* HUD, USavior3* Slot, TSoftObjectPtr<UWorld>LevelToOpen, const FBeginDATA &OnOpenCallback) {
	USAVIOR_OpenLevel_Callback* OBJ = NewObject<USAVIOR_OpenLevel_Callback>();
	ESaviorResult Result;
	//
	FString Name;
	LevelToOpen.ToString().Split(TEXT("."),nullptr,&Name,ESearchCase::IgnoreCase,ESearchDir::FromEnd);
	//
	OBJ->OnOpenLevel = OnOpenCallback;
	//
	OBJ->Level = (LevelToOpen.IsNull()) ? TEXT("None") : *Name;
	OBJ->Outer = Context;
	OBJ->SHUD = HUD;
	//
	if (Slot) {
		if (!Slot->IsInstance()||(Slot->IsInstance()&&(Slot->GetThreadSafety()!=EThreadSafety::IsCurrentlyThreadSafe))) {
			OBJ->Target = USavior3::NewSlotInstance(Context,Slot,Result);
		} else {OBJ->Target=Slot;}
	}///
	//
	return OBJ;
}

void USAVIOR_OpenLevel_Callback::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	if (SHUD==nullptr||SHUD->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target HUD."),ELogVerbosity::Error); return;}
	//
	Execute();
}

void USAVIOR_OpenLevel_Callback::Execute() {
	switch (Target->LoadScreenMode) {
		case ELoadScreenMode::BackgroundBlur:
		{
			SHUD->DisplayBlurLoadScreenHUD(EThreadSafety::SynchronousLoading,Target->FeedbackLOAD,Target->FeedbackFont,Target->ProgressBarTint,Target->BackBlurPower);
		}	break;
		//
		case ELoadScreenMode::SplashScreen:
		{
			SHUD->DisplaySplashLoadScreenHUD(EThreadSafety::SynchronousLoading,Target->FeedbackLOAD,Target->FeedbackFont,Target->ProgressBarTint,Target->SplashImage,Target->SplashStretch);
		}	break;
		//
		case ELoadScreenMode::MoviePlayer:
		{
			SHUD->DisplayMovieLoadScreenHUD(EThreadSafety::SynchronousLoading,Target->FeedbackLOAD,Target->FeedbackFont,Target->ProgressBarTint,Target->SplashMovie,Target->ProgressBarOnMovie);
		}	break;
	default: break;}
	//
	OnOpenLevel.ExecuteIfBound();
	//
	UGameplayStatics::OpenLevel(Outer,Level);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Game World:

USAVIOR_SaveGameWorld* USAVIOR_SaveGameWorld::SaveGameWorld(USavior3* Savior, UObject* Context, const bool SaveSlotMeta) {
	USAVIOR_SaveGameWorld* OBJ = NewObject<USAVIOR_SaveGameWorld>();
	ESaviorResult Result;
	//
	OBJ->Outer = Context;
	//
	if (Savior) {
		if (!Savior->IsInstance()||(Savior->IsInstance()&&(Savior->GetThreadSafety()!=EThreadSafety::IsCurrentlyThreadSafe))) {
			OBJ->Target = USavior3::NewSlotInstance(Context,Savior,Result);
		} else {OBJ->Target=Savior;}
	}///
	//
	if (OBJ->Target!=nullptr) {
		OBJ->Target->WriteMetaOnSave = SaveSlotMeta;
	}///
	//
	return OBJ;
}

void USAVIOR_SaveGameWorld::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Target->EVENT_OnFinishDataSAVE.AddDynamic(this,&USAVIOR_SaveGameWorld::Finish);
	//
	Execute();
}

void USAVIOR_SaveGameWorld::Execute() {
	ESaviorResult Result;
	//
	Target->SaveGameWorld(Outer,Result);
}

void USAVIOR_SaveGameWorld::Finish(const bool Succeeded) {
	Target->AbortCurrentTask();
	//
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Game World [+Callbacks]:

USAVIOR_SaveGameWorld_Callback* USAVIOR_SaveGameWorld_Callback::SaveGameWorld(USavior3* Savior, UObject* Context, const bool SaveSlotMeta, const FBeginDATA &BeganSaveCallback, const FEndDATA &FinishedSaveCallback) {
	USAVIOR_SaveGameWorld_Callback* OBJ = NewObject<USAVIOR_SaveGameWorld_Callback>();
	ESaviorResult Result;
	//
	OBJ->Outer = Context;
	OBJ->BeganSave = BeganSaveCallback;
	OBJ->FinishedSave = FinishedSaveCallback;
	//
	if (Savior) {
		if (!Savior->IsInstance()||(Savior->IsInstance()&&(Savior->GetThreadSafety()!=EThreadSafety::IsCurrentlyThreadSafe))) {
			OBJ->Target = USavior3::NewSlotInstance(Context,Savior,Result);
		} else {OBJ->Target=Savior;}
	}///
	//
	if (OBJ->Target!=nullptr) {
		OBJ->Target->WriteMetaOnSave = SaveSlotMeta;
	}///
	//
	return OBJ;
}

void USAVIOR_SaveGameWorld_Callback::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Target->EVENT_OnFinishDataSAVE.AddDynamic(this,&USAVIOR_SaveGameWorld_Callback::Finish);
	//
	Execute();
}

void USAVIOR_SaveGameWorld_Callback::Execute() {
	BeganSave.ExecuteIfBound();
	ESaviorResult Result;
	//
	Target->SaveGameWorld(Outer,Result);
}

void USAVIOR_SaveGameWorld_Callback::Finish(const bool Succeeded) {
	Target->AbortCurrentTask();
	//
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
	//
	FinishedSave.ExecuteIfBound(Succeeded);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Game World:

USAVIOR_LoadGameWorld* USAVIOR_LoadGameWorld::LoadGameWorld(USavior3* Savior, UObject* Context, const bool ResetLevelOnLoad) {
	USAVIOR_LoadGameWorld* OBJ = NewObject<USAVIOR_LoadGameWorld>();
	ESaviorResult Result;
	//
	OBJ->Outer = Context;
	//
	if (Savior) {
		if (!Savior->IsInstance()||(Savior->IsInstance()&&(Savior->GetThreadSafety()!=EThreadSafety::IsCurrentlyThreadSafe))) {
			OBJ->Target = USavior3::NewSlotInstance(Context,Savior,Result);
		} else {OBJ->Target=Savior;}
	}///
	//
	if (OBJ->Target!=nullptr) {
		OBJ->ResetLevel = ResetLevelOnLoad;
	}///
	//
	return OBJ;
}

void USAVIOR_LoadGameWorld::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Target->EVENT_OnFinishDataLOAD.AddDynamic(this,&USAVIOR_LoadGameWorld::Finish);
	//
	Execute();
}

void USAVIOR_LoadGameWorld::Execute() {
	ESaviorResult Result;
	//
	Target->LoadGameWorld(Outer,ResetLevel,Result);
}

void USAVIOR_LoadGameWorld::Finish(const bool Succeeded) {
	Target->AbortCurrentTask();
	//
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Game World [+Callbacks]:

USAVIOR_LoadGameWorld_Callback* USAVIOR_LoadGameWorld_Callback::LoadGameWorld(USavior3* Savior, UObject* Context, const bool ResetLevelOnLoad, const FBeginDATA &BeganLoadCallback, const FEndDATA &FinishedLoadCallback) {
	USAVIOR_LoadGameWorld_Callback* OBJ = NewObject<USAVIOR_LoadGameWorld_Callback>();
	ESaviorResult Result;
	//
	OBJ->Outer = Context;
	OBJ->BeganLoad = BeganLoadCallback;
	OBJ->FinishedLoad = FinishedLoadCallback;
	//
	if (Savior) {
		if (!Savior->IsInstance()||(Savior->IsInstance()&&(Savior->GetThreadSafety()!=EThreadSafety::IsCurrentlyThreadSafe))) {
			OBJ->Target = USavior3::NewSlotInstance(Context,Savior,Result);
		} else {OBJ->Target=Savior;}
	}///
	//
	if (OBJ->Target!=nullptr) {
		OBJ->ResetLevel = ResetLevelOnLoad;
	}///
	//
	return OBJ;
}

void USAVIOR_LoadGameWorld_Callback::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Target->EVENT_OnFinishDataLOAD.AddDynamic(this,&USAVIOR_LoadGameWorld_Callback::Finish);
	//
	Execute();
}

void USAVIOR_LoadGameWorld_Callback::Execute() {
	BeganLoad.ExecuteIfBound();
	ESaviorResult Result;
	//
	Target->LoadGameWorld(Outer,ResetLevel,Result);
}

void USAVIOR_LoadGameWorld_Callback::Finish(const bool Succeeded) {
	Target->AbortCurrentTask();
	//
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
	//
	FinishedLoad.ExecuteIfBound(Succeeded);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Level:

USAVIOR_SaveLevel* USAVIOR_SaveLevel::SaveLevel(USavior3* Savior, TSoftObjectPtr<UWorld>LevelToSave, UObject* Context, const bool SaveSlotMeta, const bool WriteSlotFile) {
	USAVIOR_SaveLevel* OBJ = NewObject<USAVIOR_SaveLevel>();
	ESaviorResult Result;
	//
	OBJ->Outer = Context;
	OBJ->Level = LevelToSave;
	OBJ->WriteFile = WriteSlotFile;
	//
	if (Savior) {
		if (!Savior->IsInstance()||(Savior->IsInstance()&&(Savior->GetThreadSafety()!=EThreadSafety::IsCurrentlyThreadSafe))) {
			OBJ->Target = USavior3::NewSlotInstance(Context,Savior,Result);
		} else {OBJ->Target=Savior;}
	}///
	//
	if (OBJ->Target!=nullptr) {
		OBJ->Target->WriteMetaOnSave = SaveSlotMeta;
	}///
	//
	return OBJ;
}

void USAVIOR_SaveLevel::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Target->EVENT_OnFinishDataSAVE.AddDynamic(this,&USAVIOR_SaveLevel::Finish);
	//
	Execute();
}

void USAVIOR_SaveLevel::Execute() {
	ESaviorResult Result;
	//
	Target->SaveLevel(Outer,Level,WriteFile,Result);
}

void USAVIOR_SaveLevel::Finish(const bool Succeeded) {
	Target->AbortCurrentTask();
	//
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Level [+Callbacks]:

USAVIOR_SaveLevel_Callback* USAVIOR_SaveLevel_Callback::SaveLevel(USavior3* Savior, TSoftObjectPtr<UWorld>LevelToSave, UObject* Context, const bool SaveSlotMeta, const bool WriteSlotFile, const FBeginDATA &BeganSaveCallback, const FEndDATA &FinishedSaveCallback) {
	USAVIOR_SaveLevel_Callback* OBJ = NewObject<USAVIOR_SaveLevel_Callback>();
	ESaviorResult Result;
	//
	FString Name;
	LevelToSave.ToString().Split(TEXT("."),nullptr,&Name,ESearchCase::IgnoreCase,ESearchDir::FromEnd);
	//
	OBJ->FinishedSave = FinishedSaveCallback;
	OBJ->BeganSave = BeganSaveCallback;
	OBJ->WriteFile = WriteSlotFile;
	OBJ->Level = LevelToSave;
	OBJ->Outer = Context;
	//
	if (Savior) {
		if (!Savior->IsInstance()||(Savior->IsInstance()&&(Savior->GetThreadSafety()!=EThreadSafety::IsCurrentlyThreadSafe))) {
			OBJ->Target = USavior3::NewSlotInstance(Context,Savior,Result);
		} else {OBJ->Target=Savior;}
	}///
	//
	if (OBJ->Target!=nullptr) {
		OBJ->Target->WriteMetaOnSave = SaveSlotMeta;
	}///
	//
	return OBJ;
}

void USAVIOR_SaveLevel_Callback::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Target->EVENT_OnFinishDataSAVE.AddDynamic(this,&USAVIOR_SaveLevel_Callback::Finish);
	//
	Execute();
}

void USAVIOR_SaveLevel_Callback::Execute() {
	BeganSave.ExecuteIfBound();
	ESaviorResult Result;
	//
	Target->SaveLevel(Outer,Level,WriteFile,Result);
}

void USAVIOR_SaveLevel_Callback::Finish(const bool Succeeded) {
	Target->AbortCurrentTask();
	//
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
	//
	FinishedSave.ExecuteIfBound(Succeeded);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Level:

USAVIOR_LoadLevel* USAVIOR_LoadLevel::LoadLevel(USavior3* Savior, TSoftObjectPtr<UWorld>LevelToLoad, UObject* Context, const bool StreamLevelOnLoad, const bool ReadSlotFile) {
	USAVIOR_LoadLevel* OBJ = NewObject<USAVIOR_LoadLevel>();
	ESaviorResult Result;
	//
	OBJ->Target = USavior3::NewSlotInstance(Context,Savior,Result);
	//
	OBJ->Outer = Context;
	OBJ->Level = LevelToLoad;
	OBJ->ReadFile = ReadSlotFile;
	OBJ->StreamLevel = StreamLevelOnLoad;
	//
	if (Savior) {
		if (!Savior->IsInstance()||(Savior->IsInstance()&&(Savior->GetThreadSafety()!=EThreadSafety::IsCurrentlyThreadSafe))) {
			OBJ->Target = USavior3::NewSlotInstance(Context,Savior,Result);
		} else {OBJ->Target=Savior;}
	}///
	//
	return OBJ;
}

void USAVIOR_LoadLevel::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Target->EVENT_OnFinishDataLOAD.AddDynamic(this,&USAVIOR_LoadLevel::Finish);
	//
	Execute();
}

void USAVIOR_LoadLevel::Execute() {
	ESaviorResult Result;
	//
	if (StreamLevel) {
		FLatentActionInfo LevelInfo; FString Name;
		Level.ToString().Split(TEXT("."),nullptr,&Name,ESearchCase::IgnoreCase,ESearchDir::FromEnd);
		//
		UGameplayStatics::LoadStreamLevel(Outer,*Name,true,true,LevelInfo);
	}///
	//
	Target->LoadLevel(Outer,Level,ReadFile,Result);
}

void USAVIOR_LoadLevel::Finish(const bool Succeeded) {
	Target->AbortCurrentTask();
	//
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Level [+Callbacks]:

USAVIOR_LoadLevel_Callback* USAVIOR_LoadLevel_Callback::LoadLevel(USavior3* Savior, TSoftObjectPtr<UWorld>LevelToLoad, UObject* Context, const bool StreamLevelOnLoad, const bool ReadSlotFile, const FBeginDATA &BeganLoadCallback, const FEndDATA &FinishedLoadCallback) {
	USAVIOR_LoadLevel_Callback* OBJ = NewObject<USAVIOR_LoadLevel_Callback>();
	ESaviorResult Result;
	//
	FString Name;
	LevelToLoad.ToString().Split(TEXT("."),nullptr,&Name,ESearchCase::IgnoreCase,ESearchDir::FromEnd);
	//
	OBJ->BeganLoad = BeganLoadCallback;
	OBJ->FinishedLoad = FinishedLoadCallback;
	//
	OBJ->Outer = Context;
	OBJ->Level = LevelToLoad;
	OBJ->ReadFile = ReadSlotFile;
	OBJ->StreamLevel = StreamLevelOnLoad;
	//
	if (Savior) {
		if (!Savior->IsInstance()||(Savior->IsInstance()&&(Savior->GetThreadSafety()!=EThreadSafety::IsCurrentlyThreadSafe))) {
			OBJ->Target = USavior3::NewSlotInstance(Context,Savior,Result);
		} else {OBJ->Target=Savior;}
	}///
	//
	return OBJ;
}

void USAVIOR_LoadLevel_Callback::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Target->EVENT_OnFinishDataLOAD.AddDynamic(this,&USAVIOR_LoadLevel_Callback::Finish);
	//
	Execute();
}

void USAVIOR_LoadLevel_Callback::Execute() {
	BeganLoad.ExecuteIfBound();
	ESaviorResult Result;
	//
	if (StreamLevel) {
		FLatentActionInfo LevelInfo; FString Name;
		Level.ToString().Split(TEXT("."),nullptr,&Name,ESearchCase::IgnoreCase,ESearchDir::FromEnd);
		//
		UGameplayStatics::LoadStreamLevel(Outer,*Name,true,true,LevelInfo);
	}///
	//
	Target->LoadLevel(Outer,Level,ReadFile,Result);
}

void USAVIOR_LoadLevel_Callback::Finish(const bool Succeeded) {
	Target->AbortCurrentTask();
	//
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
	//
	FinishedLoad.ExecuteIfBound(Succeeded);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Game Mode:

USAVIOR_SaveGameMode* USAVIOR_SaveGameMode::SaveGameMode(USavior3* Savior, UObject* Context, const bool SaveSlotMeta, const bool WithGameInstance) {
	USAVIOR_SaveGameMode* OBJ = NewObject<USAVIOR_SaveGameMode>();
	ESaviorResult Result;
	//
	OBJ->Outer = Context;
	OBJ->SaveWithGameInstance = WithGameInstance;
	//
	if (Savior) {
		if (!Savior->IsInstance()||(Savior->IsInstance()&&(Savior->GetThreadSafety()!=EThreadSafety::IsCurrentlyThreadSafe))) {
			OBJ->Target = USavior3::NewSlotInstance(Context,Savior,Result);
		} else {OBJ->Target=Savior;}
	}///
	//
	if (OBJ->Target!=nullptr) {
		OBJ->Target->WriteMetaOnSave = SaveSlotMeta;
	}///
	//
	return OBJ;
}

void USAVIOR_SaveGameMode::Activate() {
	if (!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Target->EVENT_OnFinishDataSAVE.AddDynamic(this,&USAVIOR_SaveGameMode::Finish);
	//
	Execute();
}

void USAVIOR_SaveGameMode::Execute() {
	ESaviorResult Result;
	//
	Target->SaveGameMode(Outer,SaveWithGameInstance,Result);
}

void USAVIOR_SaveGameMode::Finish(const bool Succeeded) {
	Target->AbortCurrentTask();
	//
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Game Mode [+Callbacks]:

USAVIOR_SaveGameMode_Callback* USAVIOR_SaveGameMode_Callback::SaveGameMode(USavior3* Savior, UObject* Context, const bool SaveSlotMeta, const bool WithGameInstance, const FBeginDATA &BeganSaveCallback, const FEndDATA &FinishedSaveCallback) {
	USAVIOR_SaveGameMode_Callback* OBJ = NewObject<USAVIOR_SaveGameMode_Callback>();
	ESaviorResult Result;
	//
	OBJ->Outer = Context;
	OBJ->BeganSave = BeganSaveCallback;
	OBJ->FinishedSave = FinishedSaveCallback;
	OBJ->SaveWithGameInstance = WithGameInstance;
	//
	if (Savior) {
		if (!Savior->IsInstance()||(Savior->IsInstance()&&(Savior->GetThreadSafety()!=EThreadSafety::IsCurrentlyThreadSafe))) {
			OBJ->Target = USavior3::NewSlotInstance(Context,Savior,Result);
		} else {OBJ->Target=Savior;}
	}///
	//
	if (OBJ->Target!=nullptr) {
		OBJ->Target->WriteMetaOnSave = SaveSlotMeta;
	}///
	//
	return OBJ;
}

void USAVIOR_SaveGameMode_Callback::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Target->EVENT_OnFinishDataSAVE.AddDynamic(this,&USAVIOR_SaveGameMode_Callback::Finish);
	//
	Execute();
}

void USAVIOR_SaveGameMode_Callback::Execute() {
	BeganSave.ExecuteIfBound();
	ESaviorResult Result;
	//
	Target->SaveGameMode(Outer,SaveWithGameInstance,Result);
}

void USAVIOR_SaveGameMode_Callback::Finish(const bool Succeeded) {
	Target->AbortCurrentTask();
	//
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
	//
	FinishedSave.ExecuteIfBound(Succeeded);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Game Mode:

USAVIOR_LoadGameMode* USAVIOR_LoadGameMode::LoadGameMode(USavior3* Savior, UObject* Context, const bool IgnoreMainPawnTransform, const bool WithGameInstance) {
	USAVIOR_LoadGameMode* OBJ = NewObject<USAVIOR_LoadGameMode>();
	ESaviorResult Result;
	//
	OBJ->Outer = Context;
	OBJ->LoadWithGameInstance = WithGameInstance;
	//
	if (Savior) {
		if (!Savior->IsInstance()||(Savior->IsInstance()&&(Savior->GetThreadSafety()!=EThreadSafety::IsCurrentlyThreadSafe))) {
			OBJ->Target = USavior3::NewSlotInstance(Context,Savior,Result);
		} else {OBJ->Target=Savior;}
	}///
	//
	if (OBJ->Target!=nullptr) {
		OBJ->Target->IgnorePawnTransformOnLoad = IgnoreMainPawnTransform;
	}///
	//
	return OBJ;
}

void USAVIOR_LoadGameMode::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Target->EVENT_OnFinishDataLOAD.AddDynamic(this,&USAVIOR_LoadGameMode::Finish);
	//
	Execute();
}

void USAVIOR_LoadGameMode::Execute() {
	ESaviorResult Result;
	//
	Target->LoadGameMode(Outer,LoadWithGameInstance,Result);
}

void USAVIOR_LoadGameMode::Finish(const bool Succeeded) {
	Target->AbortCurrentTask();
	//
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Game Mode [+Callbacks]:

USAVIOR_LoadGameMode_Callback* USAVIOR_LoadGameMode_Callback::LoadGameMode(USavior3* Savior, UObject* Context, const bool IgnoreMainPawnTransform, const bool WithGameInstance, const FBeginDATA &BeganLoadCallback, const FEndDATA &FinishedLoadCallback) {
	USAVIOR_LoadGameMode_Callback* OBJ = NewObject<USAVIOR_LoadGameMode_Callback>();
	ESaviorResult Result;
	//
	OBJ->Outer = Context;
	OBJ->BeganLoad = BeganLoadCallback;
	OBJ->FinishedLoad = FinishedLoadCallback;
	OBJ->LoadWithGameInstance = WithGameInstance;
	//
	if (Savior) {
		if (!Savior->IsInstance()||(Savior->IsInstance()&&(Savior->GetThreadSafety()!=EThreadSafety::IsCurrentlyThreadSafe))) {
			OBJ->Target = USavior3::NewSlotInstance(Context,Savior,Result);
		} else {OBJ->Target=Savior;}
	}///
	//
	if (OBJ->Target!=nullptr) {
		OBJ->Target->IgnorePawnTransformOnLoad = IgnoreMainPawnTransform;
	}///
	//
	return OBJ;
}

void USAVIOR_LoadGameMode_Callback::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Target->EVENT_OnFinishDataLOAD.AddDynamic(this,&USAVIOR_LoadGameMode_Callback::Finish);
	//
	Execute();
}

void USAVIOR_LoadGameMode_Callback::Execute() {
	BeganLoad.ExecuteIfBound();
	ESaviorResult Result;
	//
	Target->LoadGameMode(Outer,LoadWithGameInstance,Result);
}

void USAVIOR_LoadGameMode_Callback::Finish(const bool Succeeded) {
	Target->AbortCurrentTask();
	//
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
	//
	FinishedLoad.ExecuteIfBound(Succeeded);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Game Instance:

USAVIOR_SaveGameInstance* USAVIOR_SaveGameInstance::SaveGameInstance(USavior3* Savior, UObject* Context, const bool SaveSlotMeta) {
	USAVIOR_SaveGameInstance* OBJ = NewObject<USAVIOR_SaveGameInstance>();
	ESaviorResult Result;
	//
	OBJ->Outer = Context;
	//
	if (Savior) {
		if (!Savior->IsInstance()||(Savior->IsInstance()&&(Savior->GetThreadSafety()!=EThreadSafety::IsCurrentlyThreadSafe))) {
			OBJ->Target = USavior3::NewSlotInstance(Context,Savior,Result);
		} else {OBJ->Target=Savior;}
	}///
	//
	if (OBJ->Target!=nullptr) {
		OBJ->Target->WriteMetaOnSave = SaveSlotMeta;
	}///
	//
	return OBJ;
}

void USAVIOR_SaveGameInstance::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Target->EVENT_OnFinishDataSAVE.AddDynamic(this,&USAVIOR_SaveGameInstance::Finish);
	//
	Execute();
}

void USAVIOR_SaveGameInstance::Execute() {
	ESaviorResult Result;
	//
	Target->SaveGameInstance(Outer,Result);
}

void USAVIOR_SaveGameInstance::Finish(const bool Succeeded) {
	Target->AbortCurrentTask();
	//
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Game Instance [+Callbacks]:

USAVIOR_SaveGameInstance_Callback* USAVIOR_SaveGameInstance_Callback::SaveGameInstance(USavior3* Savior, UObject* Context, const bool SaveSlotMeta, const FBeginDATA &BeganSaveCallback, const FEndDATA &FinishedSaveCallback) {
	USAVIOR_SaveGameInstance_Callback* OBJ = NewObject<USAVIOR_SaveGameInstance_Callback>();
	ESaviorResult Result;
	//
	OBJ->Outer = Context;
	OBJ->BeganSave = BeganSaveCallback;
	OBJ->FinishedSave = FinishedSaveCallback;
	//
	if (Savior) {
		if (!Savior->IsInstance()||(Savior->IsInstance()&&(Savior->GetThreadSafety()!=EThreadSafety::IsCurrentlyThreadSafe))) {
			OBJ->Target = USavior3::NewSlotInstance(Context,Savior,Result);
		} else {OBJ->Target=Savior;}
	}///
	//
	if (OBJ->Target!=nullptr) {
		OBJ->Target->WriteMetaOnSave = SaveSlotMeta;
	}///
	//
	return OBJ;
}

void USAVIOR_SaveGameInstance_Callback::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Target->EVENT_OnFinishDataSAVE.AddDynamic(this,&USAVIOR_SaveGameInstance_Callback::Finish);
	//
	Execute();
}

void USAVIOR_SaveGameInstance_Callback::Execute() {
	BeganSave.ExecuteIfBound();
	ESaviorResult Result;
	//
	Target->SaveGameInstance(Outer,Result);
}

void USAVIOR_SaveGameInstance_Callback::Finish(const bool Succeeded) {
	Target->AbortCurrentTask();
	//
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
	//
	FinishedSave.ExecuteIfBound(Succeeded);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Game Instance:

USAVIOR_LoadGameInstance* USAVIOR_LoadGameInstance::LoadGameInstance(USavior3* Savior, UObject* Context) {
	USAVIOR_LoadGameInstance* OBJ = NewObject<USAVIOR_LoadGameInstance>();
	ESaviorResult Result;
	//
	OBJ->Outer = Context;
	//
	if (Savior) {
		if (!Savior->IsInstance()||(Savior->IsInstance()&&(Savior->GetThreadSafety()!=EThreadSafety::IsCurrentlyThreadSafe))) {
			OBJ->Target = USavior3::NewSlotInstance(Context,Savior,Result);
		} else {OBJ->Target=Savior;}
	}///
	//
	return OBJ;
}

void USAVIOR_LoadGameInstance::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Target->EVENT_OnFinishDataLOAD.AddDynamic(this,&USAVIOR_LoadGameInstance::Finish);
	//
	Execute();
}

void USAVIOR_LoadGameInstance::Execute() {
	ESaviorResult Result;
	//
	Target->LoadGameInstance(Outer,Result);
}

void USAVIOR_LoadGameInstance::Finish(const bool Succeeded) {
	Target->AbortCurrentTask();
	//
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Game Instance [+Callbacks]:

USAVIOR_LoadGameInstance_Callback* USAVIOR_LoadGameInstance_Callback::LoadGameInstance(USavior3* Savior, UObject* Context, const FBeginDATA &BeganLoadCallback, const FEndDATA &FinishedLoadCallback) {
	USAVIOR_LoadGameInstance_Callback* OBJ = NewObject<USAVIOR_LoadGameInstance_Callback>();
	ESaviorResult Result;
	//
	OBJ->Outer = Context;
	OBJ->BeganLoad = BeganLoadCallback;
	OBJ->FinishedLoad = FinishedLoadCallback;
	//
	if (Savior) {
		if (!Savior->IsInstance()||(Savior->IsInstance()&&(Savior->GetThreadSafety()!=EThreadSafety::IsCurrentlyThreadSafe))) {
			OBJ->Target = USavior3::NewSlotInstance(Context,Savior,Result);
		} else {OBJ->Target=Savior;}
	}///
	//
	return OBJ;
}

void USAVIOR_LoadGameInstance_Callback::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Target->EVENT_OnFinishDataLOAD.AddDynamic(this,&USAVIOR_LoadGameInstance_Callback::Finish);
	//
	Execute();
}

void USAVIOR_LoadGameInstance_Callback::Execute() {
	BeganLoad.ExecuteIfBound();
	ESaviorResult Result;
	//
	Target->LoadGameInstance(Outer,Result);
}

void USAVIOR_LoadGameInstance_Callback::Finish(const bool Succeeded) {
	Target->AbortCurrentTask();
	//
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
	//
	FinishedLoad.ExecuteIfBound(Succeeded);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////