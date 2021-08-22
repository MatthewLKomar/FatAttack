//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//			Copyright 2020 (C) Bruno Xavier Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ISavior3.h"

#include "Savior3.h"
#include "Savior3_Shared.h"
#include "LoadScreen/HUD_LoadScreenStyle.h"

#if WITH_EDITORONLY_DATA
 #include "ISettingsModule.h"
 #include "ISettingsSection.h"
 #include "ISettingsContainer.h"
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define LOCTEXT_NAMESPACE "Synaptech"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class FSavior3 : public ISavior3 {
private:
	bool HandleSettingsSaved() {
	  #if WITH_EDITORONLY_DATA
		const auto &Settings = GetMutableDefault<USavior3Settings>();
		Settings->SaveConfig(); return true;
	  #endif
	return false;}
	//
	void RegisterSettings() {
	  #if WITH_EDITORONLY_DATA
		if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings")) {
			ISettingsContainerPtr SettingsContainer = SettingsModule->GetContainer("Project");
			SettingsContainer->DescribeCategory("Synaptech",LOCTEXT("SynaptechCategoryName","Synaptech"),
			LOCTEXT("SynaptechCategoryDescription","Configuration of Synaptech Systems."));
			//
			ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project","Synaptech","Savior3Settings",
				LOCTEXT("Savior3SettingsName","Savior 3 Settings"),
				LOCTEXT("Savior3SettingsDescription","General settings for the Savior 3 Plugin"),
			GetMutableDefault<USavior3Settings>());
			//
			if (SettingsSection.IsValid()) {SettingsSection->OnModified().BindRaw(this,&FSavior3::HandleSettingsSaved);}
		}///
	  #endif
	}
	//
	void UnregisterSettings() {
	  #if WITH_EDITORONLY_DATA
		if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings")) {
			SettingsModule->UnregisterSettings("Project","Synaptech","Savior3Settings");
		}///
	  #endif
	}
public:
	virtual void StartupModule() override {
		RegisterSettings();
		FSaviorLoadScreenStyle::Initialize();
		//
		const auto VER = FString(TEXT("{ Savior 3 } Initializing Plugin:  v"))+PLUGIN_VERSION;
		LOG_SV(true,ESeverity::Info,VER);
	}///
	//
	virtual void ShutdownModule() override {
		FSaviorLoadScreenStyle::Shutdown();
		if (UObjectInitialized()) {UnregisterSettings();}
	}///
	//
    virtual bool SupportsDynamicReloading() override {return true;}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
IMPLEMENT_GAME_MODULE(FSavior3,Savior3);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////