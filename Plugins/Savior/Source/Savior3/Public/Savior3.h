//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//		Copyright 2020 (C) Bruno Xavier Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Reflector.h"
#include "SaviorTypes.h"
#include "SaviorMetaData.h"
#include "ISAVIOR_Procedural.h"
#include "ISAVIOR_Serializable.h"

#include "Runtime/Core/Public/Core.h"
#include "Runtime/Core/Public/Misc/SecureHash.h"
#include "Runtime/CoreUObject/Public/Misc/PackageName.h"
#include "Runtime/Core/Public/Async/TaskGraphInterfaces.h"
#include "Runtime/CoreUObject/Public/UObject/SoftObjectPtr.h"

#include "Runtime/Engine/Classes/GameFramework/HUD.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"
#include "Runtime/Engine/Classes/Animation/AnimInstance.h"
#include "Runtime/Engine/Classes/GameFramework/SaveGame.h"
#include "Runtime/Engine/Classes/GameFramework/GameMode.h"
#include "Runtime/Engine/Classes/GameFramework/GameState.h"
#include "Runtime/Engine/Classes/GameFramework/Character.h"
#include "Runtime/Engine/Classes/Components/ActorComponent.h"
#include "Runtime/Engine/Classes/Components/LightComponent.h"
#include "Runtime/Engine/Classes/Components/CapsuleComponent.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"

#include "Runtime/Engine/Public/Subsystems/GameInstanceSubsystem.h"

#include "Runtime/SlateCore/Public/Fonts/SlateFontInfo.h"
#include "Runtime/Slate/Public/Widgets/Layout/SScaleBox.h"

#include "Savior3.generated.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SerializeLevel"),STAT_FSimpleDelegateGraphTask_SerializeLevel,STATGROUP_TaskGraphTasks);
DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.DeserializeLevel"),STAT_FSimpleDelegateGraphTask_DeserializeLevel,STATGROUP_TaskGraphTasks);
DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SerializeGameMode"),STAT_FSimpleDelegateGraphTask_SerializeGameMode,STATGROUP_TaskGraphTasks);
DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SerializeGameWorld"),STAT_FSimpleDelegateGraphTask_SerializeGameWorld,STATGROUP_TaskGraphTasks);
DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.DeserializeGameMode"),STAT_FSimpleDelegateGraphTask_DeserializeGameMode,STATGROUP_TaskGraphTasks);
DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.DeserializeGameWorld"),STAT_FSimpleDelegateGraphTask_DeserializeGameWorld,STATGROUP_TaskGraphTasks);
DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.RespawnActorFromData"),STAT_FSimpleDelegateGraphTask_RespawnActorFromData,STATGROUP_TaskGraphTasks);
DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SerializeGameInstance"),STAT_FSimpleDelegateGraphTask_SerializeGameInstance,STATGROUP_TaskGraphTasks);
DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.DeserializeGameInstance"),STAT_FSimpleDelegateGraphTask_DeserializeGameInstance,STATGROUP_TaskGraphTasks);
DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.RespawnComponentFromData"),STAT_FSimpleDelegateGraphTask_RespawnComponentFromData,STATGROUP_TaskGraphTasks);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSavior_BeginDataSAVE);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSavior_FinishDataSAVE,const bool,Success);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSavior_BeginDataLOAD);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSavior_FinishDataLOAD,const bool,Success);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior 3 Internals:

class USavior3;
class USaviorSubsystem;

class USAVIOR_OpenLevel;
class USAVIOR_SaveLevel;
class USAVIOR_LoadLevel;
class USAVIOR_SaveGameMode;
class USAVIOR_LoadGameMode;
class USAVIOR_SaveGameWorld;
class USAVIOR_LoadGameWorld;
class USAVIOR_SaveGameInstance;
class USAVIOR_LoadGameInstance;
class USAVIOR_OpenLevel_Callback;
class USAVIOR_SaveLevel_Callback;
class USAVIOR_LoadLevel_Callback;
class USAVIOR_SaveGameMode_Callback;
class USAVIOR_LoadGameMode_Callback;
class USAVIOR_SaveGameWorld_Callback;
class USAVIOR_LoadGameWorld_Callback;
class USAVIOR_SaveGameInstance_Callback;
class USAVIOR_LoadGameInstance_Callback;

class TASK_LaunchLoadScreen;
class TASK_SerializeGameWorld;
class TASK_DeserializeGameWorld;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior Default Settings:

UCLASS(ClassGroup=Synaptech, Category="Synaptech", config=Game)
class SAVIOR3_API USavior3Settings : public UObject {
	GENERATED_BODY()
	//
	USavior3Settings();
public:
	/// :: SETTINGS ::
	///
	/** Default Player Controller ID. */
	UPROPERTY(Category="General Settings", config, EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0"))
	int32 DefaultPlayerID;
	//
	/** Local Player's Level. If the Game is Progression-based, this is important UI information.
	Many Game genres won't use this; If your Game needs, you have to set value manually before saving. */
	UPROPERTY(Category="General Settings", config, EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0"))
	int32 DefaultPlayerLevel;
	//
	/** Local Player's Alias. 
	Many Game genres won't use this; If your Game needs, you have to set value manually before saving. */
	UPROPERTY(Category="General Settings", config, EditDefaultsOnly, BlueprintReadOnly)
	FString DefaultPlayerName;
	//
	/** Story Chapter Name.
	Many Game genres won't use this; It's mainly for story-driven games. */
	UPROPERTY(Category="General Settings", config, EditDefaultsOnly, BlueprintReadOnly)
	FString DefaultChapter;
	//
	/** Name of Map, Level, that we are saving data from. */
	UPROPERTY(Category="General Settings", config, EditDefaultsOnly, BlueprintReadOnly)
	FString DefaultLocation;
	///
	/// :: PERFORMANCE ::
	///
	/** If enabled, Savior Objects will sort Records corresponding to Actors that aren't present in World.
	If a Record exists in Slot, but doesn't currently exists in World, then a Respawn action is triggered.
	Enabled by default, but disabling this option significantly decreases Loading times.
	This is only relevant for the "Load Game World" function. */
	UPROPERTY(Category="Performance", config, EditDefaultsOnly, BlueprintReadOnly)
	bool RespawnDynamicActors;
	//
	/** If enabled, Savior Objects will sort Records corresponding to Components that aren't present in World.
	If a Record exists in Slot, but doesn't currently exists in World, then a Respawn action is triggered.
	Enabled by default, but disabling this option significantly decreases Loading times.
	This is only relevant for the "Load Game World" function. */
	UPROPERTY(Category="Performance", config, EditDefaultsOnly, BlueprintReadOnly)
	bool RespawnDynamicComponents;
	//
	/** Determines Auto-Instance behaviour for Classes in World.
	Only References children of selected Classes will automatically re-instantiate before loading Properties. */
	UPROPERTY(Category="Reflector", config, EditDefaultsOnly, BlueprintReadOnly)
	TSet<TSubclassOf<UObject>>InstanceScope;
	//
	/** Determines Auto-Respawn behaviour for Classes in World.
	Only References children of selected Classes will automatically respawn before loading Properties. */
	UPROPERTY(Category="Reflector", config, EditDefaultsOnly, BlueprintReadOnly)
	TSet<TSubclassOf<UObject>>RespawnScope;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior Core:

UCLASS(ClassGroup=Synaptech, Category="Synaptech", Blueprintable, BlueprintType, meta=(DisplayName="[SAVIOR] Auto-Instanced Object"))
class SAVIOR3_API UAutoInstanced : public UObject, public ISAVIOR_Serializable {
	GENERATED_BODY()
};

UCLASS(ClassGroup=Synaptech, Category="Synaptech", BlueprintType, meta=(DisplayName="Savior III"))
class SAVIOR3_API USavior3 : public UObject, public Reflector {
	GENERATED_BODY()
	//
	friend class USaviorSubsystem;
	//
	friend class USAVIOR_OpenLevel;
	friend class USAVIOR_SaveLevel;
	friend class USAVIOR_LoadLevel;
	friend class USAVIOR_SaveGameMode;
	friend class USAVIOR_LoadGameMode;
	friend class USAVIOR_SaveGameWorld;
	friend class USAVIOR_LoadGameWorld;
	friend class USAVIOR_SaveGameInstance;
	friend class USAVIOR_LoadGameInstance;
	friend class USAVIOR_OpenLevel_Callback;
	friend class USAVIOR_SaveLevel_Callback;
	friend class USAVIOR_LoadLevel_Callback;
	friend class USAVIOR_SaveGameMode_Callback;
	friend class USAVIOR_LoadGameMode_Callback;
	friend class USAVIOR_SaveGameWorld_Callback;
	friend class USAVIOR_LoadGameWorld_Callback;
	friend class USAVIOR_SaveGameInstance_Callback;
	friend class USAVIOR_LoadGameInstance_Callback;
	//
	friend class TASK_LaunchLoadScreen;
	friend class TASK_SerializeGameWorld;
	friend class TASK_DeserializeGameWorld;
public:
	USavior3();
	virtual ~USavior3();
private:
	/// :: NATIVE ::
	///
	static float SS_Progress;
	static float SS_Workload;
	static float SS_Complete;
	static float SL_Progress;
	static float SL_Workload;
	static float SL_Complete;
	//
	static EThreadSafety ThreadSafety;
	static EThreadSafety LastThreadState;
	//
	//
	UPROPERTY() UWorld* World;
	UPROPERTY() bool Instanced;
protected:
	/// :: DATA ::
	///
	UPROPERTY() FSlotMeta SlotMeta;
	UPROPERTY() FSlotData SlotData;
protected:
	/** Load-Screen Timer Handle. */
	static FTimerHandle TH_LoadScreen;
protected:
	/// :: NATIVE ::
	///
	/** Checks if this Object is member of World. */
	virtual bool CheckInstance();
	//
	/** Cancels a malformed operation. */
	virtual void AbortCurrentTask();
	//
	/** Setup Slot to perform a Save action. */
	virtual const ESaviorResult PrepareSlotToSave(const UObject* Context);
	//
	/** Setup Slot to perform a Load action. */
	virtual const ESaviorResult PrepareSlotToLoad(const UObject* Context);
	//
	/** Returns Property Name mapped to a Class Version Redirector, if any. */
	virtual FName GetMappedPropertyName(TSubclassOf<UObject> Class, const FName Property) const;
	//
	/** Determines Workload based on Class Filters. */
	virtual float CalculateWorkload() const;
	//
	/** Load-Screen Setup. */
	virtual void LaunchLoadScreen(const EThreadSafety Mode, const FText Info);
protected:
	/** Loads Game World from Static Data.
	Triggered by Persistent Level transitions. */
	virtual void StaticLoadGameWorld(UWorld* InWorld);
protected:
	UMetaSlot* SlotMetaToOBJ();
	UDataSlot* SlotDataToOBJ();
public:
	bool IsInstance() const;
	FString GetSlotName() const;
public:
	void Reset();
	void ClearWorkload();
	void RemoveLoadScreen();
public:
	const FSlotMeta &GetSlotMeta() const {return SlotMeta;}
	const FSlotData &GetSlotData() const {return SlotData;}
	//
	void SetSlotMeta(const FSlotMeta &Meta) {SlotMeta=Meta;}
	void SetSlotData(const FSlotData &Data) {SlotData=Data;}
public:
	void SetWorld(UWorld* InWorld);
public:
	/// :: NATIVE ::
	///
	bool WriteMetaOnSave;
	bool IgnorePawnTransformOnLoad;
	//
	//
	virtual void PostLoad() override;
	virtual void BeginDestroy() override;
	virtual void PostInitProperties() override;
	//
	virtual UWorld* GetWorld() const override;
	///
	/// :: CONFIG ::
	///
	/** Enables Debug Logs. */
	UPROPERTY(Category="Savior", EditAnywhere, BlueprintReadWrite, AdvancedDisplay)
	bool Debug;
	//
	/** Enables Debug Logs within serialization processes. */
	UPROPERTY(Category="Savior", EditAnywhere, BlueprintReadWrite, AdvancedDisplay)
	bool DeepLogs;
	//
	/** Dictates which thread serialization methods are executed within.
	If you're saving and loading many Object Properties, you should consider run the Save-Game in Threaded Mode.
	If you need absolute control of when saving and loading starts and finishes, run the Save-Game in Synchronous Mode (locks Game Thread when loading). */
	UPROPERTY(Category="Savior", EditAnywhere, BlueprintReadWrite)
	ERuntimeMode RuntimeMode;
	//
	/** Dictates which data type this slot will target.
	Complex slots store large information and support auto-respawn of Actors on load;
	Minimal data sets generate smaller files, auto-respawn of Actors is not supported. */
	UPROPERTY(Category="Savior", EditAnywhere, BlueprintReadWrite)
	ERecordType Compression;
	//
	/** Limits serialization actions to determined Classes of Objects in World.
	Only Objects based on selected Classes will have properties saved or loaded.
	Object Classes in Scope are required to implement the 'SAVIOR Serializable' Interface. */
	UPROPERTY(Category="Savior|Scope", EditAnywhere, BlueprintReadWrite)
	TSet<TSubclassOf<UObject>>ObjectScope;
	//
	/** Limits serialization actions to determined Classes of Components in World.
	Only Objects based on selected Classes will have properties saved or loaded. */
	UPROPERTY(Category="Savior|Scope", EditAnywhere, BlueprintReadWrite)
	TSet<TSubclassOf<UActorComponent>>ComponentScope;
	//
	/** Limits serialization actions to determined Classes of Actors in World.
	Only Objects based on selected Classes will have properties saved or loaded. */
	UPROPERTY(Category="Savior|Scope", EditAnywhere, BlueprintReadWrite)
	TSet<TSubclassOf<AActor>>ActorScope;
	///
	/// :: VERSIONING ::
	///
	/** Class Property Versioning. Used as 'Property Name Redirector' for Identity;
	If a Property's name have changed in a Class, Property cannot be loaded from a Save-Game Record using old ID;
	Using a Property Redirector of 'Old Name' mapped to 'New Name' provides a way to load records from outdated Save-Games.
	Only Name Redirectors are supported, type mismatching will generate errors when loading a Property from the Slot. */
	UPROPERTY(Category="Version Control", EditAnywhere)
	TMap<TSubclassOf<UObject>,FSaviorRedirector>Redirectors;
	///
	/// :: LOAD-SCREEN ::
	///
	/** Automatically setup a Load-Screen of selected type when loading. */
	UPROPERTY(Category="Load Screen", EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Screen Mode"))
	ELoadScreenMode LoadScreenMode;
	//
	/** Display Load-Screen when loading, saving or both. */
	UPROPERTY(Category="Load Screen", EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Screen Trigger"))
	ELoadScreenTrigger LoadScreenTrigger;
	//
	/** Text to display with Progress Bar when 'Saving'. */
	UPROPERTY(Category="Load Screen", EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Feedback Text: SAVE"))
	FText FeedbackSAVE;
	//
	/** Text to display with Progress Bar when 'Loading'. */
	UPROPERTY(Category="Load Screen", EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Feedback Text: LOAD"))
	FText FeedbackLOAD;
	//
	/** Minimum time in seconds a Load-Screen HUD should be displayed when loading. */
	UPROPERTY(Category="Load Screen", EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Load-Screen Duration", ClampMin="1.0", UIMin="1.0", ClampMax="100.0", UIMax="100.0"))
	float LoadScreenTimer;
	//
	/** If Load-Screen Mode is a 'Background Blur', dictates how strong the blur frame will be while loading. */
	UPROPERTY(Category="Load Screen", EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Background-Blur Power", ClampMin="1.0", UIMin="1.0", ClampMax="100.0", UIMax="100.0"))
	float BackBlurPower;
	//
	/** Image used to display as background when 'Loading' or 'Saving' if Mode is 'Splash Screen'. */
	UPROPERTY(Category="Load Screen", EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Splash-Screen", AllowedClasses="Texture2D"))
	FSoftObjectPath SplashImage;
	//
	/** Image used to display as background when 'Loading' or 'Saving' if Mode is 'Splash Screen'. */
	UPROPERTY(Category="Load Screen", EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Splash-Movie", AllowedClasses="FileMediaSource"))
	FSoftObjectPath SplashMovie;
	//
	/** If enabled, when Mode is 'Movie Player', Progress Bar will still be displayed on top of video. */
	UPROPERTY(Category="Load Screen", EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Progress-Bar on Movies"))
	bool ProgressBarOnMovie;
	//
	/** If enabled, Threaded Save or Load calls will pause Game Thread. */
	UPROPERTY(Category="Load Screen", EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Pause Game"))
	bool PauseGameOnLoad;
	//
	/** Color tint applied on top of Progress Bar's filler image. */
	UPROPERTY(Category="Load Screen", EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta=(DisplayName="Progress-Bar Tint"))
	FLinearColor ProgressBarTint;
	//
	/* Stretching mode of Splash Image, if applicable. */
	UPROPERTY(Category="Load Screen", EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta=(DisplayName="Splash Stretch-Mode"))
	TEnumAsByte<EStretch::Type>SplashStretch;
	//
	/** Font used to display 'Loading' or 'Saving' feedback text. */
	UPROPERTY(Category="Load Screen", EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta=(DisplayName="Feedback Font"))
	FSlateFontInfo FeedbackFont;
	///
	/// :: UI/UX ::
	///
	/** Name for the (*.sav) file stored on disk.
	If no name provided, by default Slot Asset's name is used. */
	UPROPERTY(Category="UI/UX", EditDefaultsOnly, BlueprintReadWrite)
	FText SlotFileName;
	//
	/** Thumbnail Image used to display as Screenshot when this Object is displayed on UI/UX Elements. */
	UPROPERTY(Category="UI/UX", BlueprintReadWrite, meta=(AllowedClasses="Texture2D"))
	FSoftObjectPath SlotThumbnail;
	//
	/** List of Texture Assets used as thumbnail for each corresponding Level.
	When a Slot is saved within a Level, Thumbnail will be added to Slot.
	These Textures are only loaded to memory when requested by UI. */
	UPROPERTY(Category="UI/UX", EditDefaultsOnly, BlueprintReadWrite)
	TMap<TSoftObjectPtr<UWorld>,TSoftObjectPtr<UTexture2D>>LevelThumbnails;
	///
	/// :: EVENTS ::
	///
	/** Blueprint Assignable Event: On Data Save. */
	UPROPERTY(Category = "Events", BlueprintAssignable, meta = (DisplayName = "[SAVIOR] On Begin Data Save:"))
	FSavior_BeginDataSAVE EVENT_OnBeginDataSAVE;
	//
	/** Blueprint Assignable Event: On Finish Data Save. */
	UPROPERTY(Category = "Events", BlueprintAssignable, meta = (DisplayName = "[SAVIOR] On Finish Data Save:"))
	FSavior_FinishDataSAVE EVENT_OnFinishDataSAVE;
	//
	/** Blueprint Assignable Event: On Data Load. */
	UPROPERTY(Category = "Events", BlueprintAssignable, meta = (DisplayName = "[SAVIOR] On Begin Data Load:"))
	FSavior_BeginDataLOAD EVENT_OnBeginDataLOAD;
	//
	/** Blueprint Assignable Event: On Finish Data Load. */
	UPROPERTY(Category = "Events", BlueprintAssignable, meta = (DisplayName = "[SAVIOR] On Finish Data Load:"))
	FSavior_FinishDataLOAD EVENT_OnFinishDataLOAD;
	///
	/// :: FUNCTIONS ::
	///
	/** Writes all Data stored in this 'SaveGame' Object into a '.sav' file.
	@PlayerID: ID of Player Controller for recording. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Write Slot to File (.SAV)", ExpandEnumAsExecs="Result"))
	void WriteSlotToFile(const int32 PlayerID, ESaviorResult &Result);
	//
	/** Reads all Data from the '.sav' file into this 'SaveGame' Object.
	@PlayerID: ID of Player Controller in Slot. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Read Slot from File (.SAV)", ExpandEnumAsExecs="Result"))
	void ReadSlotFromFile(const int32 PlayerID, ESaviorResult &Result);
	//
	/** Deletes this 'SaveGame' Object's '.sav' file if exists.
	@PlayerID: ID of Player Controller in Slot. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Delete Slot's File (.SAV)", ExpandEnumAsExecs="Result"))
	void DeleteSlotFile(const int32 PlayerID, ESaviorResult &Result);
	//
	/** Checks if this 'SaveGame' Object's '.sav' file exists. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Find Slot's File (.SAV)", ExpandEnumAsExecs="Result", Keywords="File,Exist"))
	void FindSlotFile(ESaviorResult &Result);
	//
	/** Saves Slot's Meta-Data to File (*.sav).
	Game Object's data will NOT be serialized. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Save Slot Meta-Data (.SAV)", ExpandEnumAsExecs="Result"))
	void SaveSlotMetaData(ESaviorResult &Result);
	//
	/** Loads Slot's Meta-Data from File (*.sav).
	Game Object's data will NOT be serialized. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Slot Meta-Data (.SAV)", ExpandEnumAsExecs="Result"))
	void LoadSlotMetaData(ESaviorResult &Result);
	//
	/** Gets Reference to the Meta-Data Object from this Slot. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Get Slot Meta-Data"))
	const FSlotMeta &GetSlotMetaData() const;
	//
	//
	/** Saves to Slot all Properties marked 'SaveGame' of Target Object.
	This is NOT a call to 'Write Slot to File' function, call it manually.
	@Object: Object Reference Properties are read from. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Save Object", ExpandEnumAsExecs="Result"))
	void SaveObject(UObject* Object, ESaviorResult &Result);
	//
	/** Loads from Slot all Properties marked 'SaveGame' of Target Object.
	This is NOT a call to 'Read Slot from File' function, call it manually.
	@Object: Object Reference loaded Properties are written to. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Object", ExpandEnumAsExecs="Result"))
	void LoadObject(UObject* Object, ESaviorResult &Result);
	//
	//
	/** Saves to Slot all Properties marked 'SaveGame' of Target Object and its Children.
	This is NOT a call to 'Write Slot to File' function, call it manually.
	@Object: Root Object Reference Properties are read from. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Save Object Hierarchy", ExpandEnumAsExecs="Result"))
	void SaveObjectHierarchy(UObject* Object, ESaviorResult &Result, ESaviorResult &HierarchyResult);
	//
	/** Loads from Slot all Properties marked 'SaveGame' of Target Object and its Children.
	This is NOT a call to 'Read Slot from File' function, call it manually.
	@Object: Root Object Reference loaded Properties are written to. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Object Hierarchy", ExpandEnumAsExecs="Result"))
	void LoadObjectHierarchy(UObject* Object, ESaviorResult &Result, ESaviorResult &HierarchyResult);
	//
	//
	/** Saves to Slot all Properties marked 'SaveGame' of Target Object Class.
	This is NOT a call to 'Write Slot to File' function, call it manually.
	@Object: Root Object Reference Properties are read from. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Save Objects of Class", ExpandEnumAsExecs="Result"))
	void SaveObjectsOfClass(TSubclassOf<UObject>Class, const bool SaveHierarchy, ESaviorResult &Result, ESaviorResult &HierarchyResult);
	//
	/** Loads from Slot all Properties marked 'SaveGame' of Target Object Class.
	This is NOT a call to 'Read Slot from File' function, call it manually.
	@Object: Root Object Reference loaded Properties are written to. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Objects of Class", ExpandEnumAsExecs="Result"))
	void LoadObjectsOfClass(TSubclassOf<UObject>Class, const bool LoadHierarchy, ESaviorResult &Result, ESaviorResult &HierarchyResult);
	//
	//
	/** Saves to Slot all Properties marked 'SaveGame' of Target Character's Animation.
	This is NOT a call to 'Write Slot to File' function, call it manually.
	@Character: Object Reference Properties are read from. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Save Animation", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void SaveAnimation(UObject* Context, ACharacter* Character, ESaviorResult &Result);
	//
	/** Loads from Slot all Properties marked 'SaveGame' of Target Character's Animation.
	This is NOT a call to 'Read Slot from File' function, call it manually.
	@Actor: Object Reference loaded Properties are written to. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Animation", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void LoadAnimation(UObject* Context, ACharacter* Character, ESaviorResult &Result);
	//
	//
	/** Saves to Slot all Properties of Target Actor's Dynamic Material Instances.
	This is NOT a call to 'Write Slot to File' function, call it manually.
	@Actor: Object Reference Material Properties are read from. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Save Dynamic Materials (Actor)", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void SaveActorMaterials(UObject* Context, AActor* Actor, ESaviorResult &Result);
	//
	/** Loads from Slot all Properties of Target Character's Dynamic Material Instances.
	This is NOT a call to 'Read Slot from File' function, call it manually.
	@Actor: Object Reference loaded Material Properties are written to. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Dynamic Materials (Actor)", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void LoadActorMaterials(UObject* Context, AActor* Actor, ESaviorResult &Result);
	//
	//
	/** Saves to Slot all Properties marked 'SaveGame' of Target Component.
	This is NOT a call to 'Write Slot to File' function, call it manually.
	@Component: Object Reference Properties are read from. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Save Component", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void SaveComponent(UObject* Context, UActorComponent* Component, ESaviorResult &Result);
	//
	/** Loads from Slot all Properties marked 'SaveGame' of Target Component.
	This is NOT a call to 'Read Slot from File' function, call it manually.
	@Component: Object Reference loaded Properties are written to. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Component", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void LoadComponent(UObject* Context, UActorComponent* Component, ESaviorResult &Result);
	//
	/** Loads from Slot all Properties marked 'SaveGame' of Component from given 'SGUID'.
	Only COMPLEX data set store SGUID info. Minimal set will always fail this.
	This is NOT a call to 'Read Slot from File' function, call it manually.
	@SGUID: Target Component must contain a 'SGUID' Property of type FGuid marked 'SaveGame'. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Component by GUID", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void LoadComponentWithGUID(UObject* Context, const FGuid &SGUID, ESaviorResult &Result);
	//
	//
	/** Saves to Slot all Properties marked 'SaveGame' of Target Actor.
	This is NOT a call to 'Write Slot to File' function, call it manually.
	@Actor: Object Reference Properties are read from. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Save Actor", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void SaveActor(UObject* Context, AActor* Actor, ESaviorResult &Result);
	//
	/** Loads from Slot all Properties marked 'SaveGame' of Target Actor.
	This is NOT a call to 'Read Slot from File' function, call it manually.
	@Actor: Object Reference loaded Properties are written to. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Actor", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void LoadActor(UObject* Context, AActor* Actor, ESaviorResult &Result);
	//
	/** Loads from Slot all Properties marked 'SaveGame' of Actor from given 'SGUID'.
	Only COMPLEX data set store SGUID info. Minimal set will always fail this.
	This is NOT a call to 'Read Slot from File' function, call it manually.
	@SGUID: Target Actor must contain a 'SGUID' Property of type FGuid marked 'SaveGame'. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Actor by GUID", WorldContext="Context", ExpandEnumAsExecs="Result"))
	AActor* LoadActorWithGUID(UObject* Context, const FGuid &SGUID, ESaviorResult &Result);
	//
	//
	/** Saves to Slot all Properties marked 'SaveGame' of Target Actor and its Children.
	This is NOT a call to 'Write Slot to File' function, call it manually.
	@Actor: Object Reference Properties are read from. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Save Actor Hierarchy", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void SaveActorHierarchy(UObject* Context, AActor* Actor, ESaviorResult &Result, ESaviorResult &HierarchyResult);
	//
	/** Loads from Slot all Properties marked 'SaveGame' of Target Actor and its Children.
	This is NOT a call to 'Read Slot from File' function, call it manually.
	@Actor: Object Reference loaded Properties are written to. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Actor Hierarchy", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void LoadActorHierarchy(UObject* Context, AActor* Actor, const bool IgnoreTransform, ESaviorResult &Result, ESaviorResult &HierarchyResult);
	//
	/** Loads from Slot all Properties marked 'SaveGame' of Actor and its children from given 'SGUID'.
	Only COMPLEX data set store SGUID info. Minimal set will always fail this.
	This is NOT a call to 'Read Slot from File' function, call it manually.
	@SGUID: Target Actor must contain a 'SGUID' Property of type FGuid marked 'SaveGame'. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Actor Hierarchy by GUID", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void LoadActorHierarchyWithGUID(UObject* Context, const FGuid &SGUID, ESaviorResult &Result, ESaviorResult &HierarchyResult);
	//
	//
	/** Saves to Slot all Properties marked 'SaveGame' of Target Actor Class.
	This is NOT a call to 'Write Slot to File' function, call it manually.
	@Actor: Object Reference Properties are read from. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Save Actors of Class", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void SaveActorsOfClass(UObject* Context, TSubclassOf<AActor>Class, const bool SaveHierarchy, ESaviorResult &Result, ESaviorResult &HierarchyResult);
	//
	/** Loads from Slot all Properties marked 'SaveGame' of Target Actor Class.
	This is NOT a call to 'Read Slot from File' function, call it manually.
	@Actor: Object Reference loaded Properties are written to. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Actors of Class", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void LoadActorsOfClass(UObject* Context, TSubclassOf<AActor>Class, const bool LoadHierarchy, ESaviorResult &Result, ESaviorResult &HierarchyResult);
	//
	//
	/** Saves the Player State group of Classes to Slot.
	This is NOT a call to 'Write Slot to File' function, call it manually. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Save Player State", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void SavePlayerState(UObject* Context, APlayerController* Controller, ESaviorResult &Result);
	//
	/** Loads Player State group of Classes from Slot.
	Dynamically created Objects will NOT auto respawn.
	This is NOT a call to 'Write Slot to File' function, call it manually. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Player State", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void LoadPlayerState(UObject* Context, APlayerController* Controller, ESaviorResult &Result);
	//
	//
	/** Saves to Slot all Properties marked 'SaveGame' of defined Game Instance Class.
	This is NOT a call to 'Write Slot to File' function, call it manually.
	@Instance: GI Reference Properties are read from. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Save GI Singleton", ExpandEnumAsExecs="Result"))
	void SaveGameInstanceSingleTon(UGameInstance* Instance, ESaviorResult &Result);
	//
	//
	/** Loads from Slot all Properties marked 'SaveGame' of defined Game Instance Class.
	This is NOT a call to 'Read Slot from File' function, call it manually.
	@Instance: GI Reference loaded Properties are written to. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load GI Singleton", ExpandEnumAsExecs="Result"))
	void LoadGameInstanceSingleTon(UGameInstance* Instance, ESaviorResult &Result);
	//
	//
	/** Saves World's Game-Instance to Slot.
	Automatically calls 'Write Slot to File' function. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Save Game Instance", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void SaveGameInstance(UObject* Context, ESaviorResult &Result);
	//
	/** Loads World's Game-Instance from Slot.
	Dynamically created Objects will NOT auto respawn.
	Automatically calls 'Read Slot from File' function. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Game Instance", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void LoadGameInstance(UObject* Context, ESaviorResult &Result);
	//
	//
	/** Saves the Game-Mode Classes to Slot.
	Automatically calls 'Write Slot to File' function. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Save Game Mode", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void SaveGameMode(UObject* Context, bool WithGameInstance, ESaviorResult &Result);
	//
	/** Loads Game-Mode Classes from Slot.
	Dynamically created Objects will NOT auto respawn.
	Automatically calls 'Read Slot from File' function. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Game Mode", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void LoadGameMode(UObject* Context, bool WithGameInstance, ESaviorResult &Result);
	//
	//
	/** Saves the whole Game World to Slot.
	Automatically calls 'Write Slot to File' function. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Save Game World", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void SaveGameWorld(UObject* Context, ESaviorResult &Result);
	//
	/** Loads the whole Game World from Slot.
	Automatically calls 'Read Slot from File' function.
	(Complex) supports respawn of dynamically created Actors & Components. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Game World", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void LoadGameWorld(UObject* Context, const bool ResetLevelOnLoad, ESaviorResult &Result);
	//
	//
	/** Saves the whole target Level to Slot.
	@LevelToSave: The Name of Level to be serialized.
	@WriteFile: Automatically calls 'Write Slot to File' on Slot. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Save Level", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void SaveLevel(UObject* Context, TSoftObjectPtr<UWorld>LevelToSave, bool WriteFile, ESaviorResult &Result);
	//
	/** Loads the whole target Level from Slot.
	Dynamically created Objects will NOT auto respawn.
	@LevelToLoad: The Name of Level to be deserialized.
	@ReadFile: Automatically calls 'Read Slot from File' on Slot. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Level", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void LoadLevel(UObject* Context, TSoftObjectPtr<UWorld>LevelToSave, bool ReadFile, ESaviorResult &Result);
	//
	//
	/** Generates (Complex) Object Data-Record to be stored in this Slot.
	@Object: Target Object for Code Reflection Analysis. */
	UFUNCTION(Category="Savior", meta=(DisplayName="[SAVIOR] Generate (Complex) Record : OBJECT", BlueprintInternalUseOnly=true))
	FSaviorRecord GenerateRecord_Object(const UObject* Object);
	//
	/** Generates (Complex) Component Data-Record to be stored in this Slot.
	@Component: Target Component for Code Reflection Analysis. */
	UFUNCTION(Category="Savior", meta=(DisplayName="[SAVIOR] Generate (Complex) Record : COMPONENT", BlueprintInternalUseOnly=true))
	FSaviorRecord GenerateRecord_Component(const UActorComponent* Component);
	//
	/** Generates (Complex) Actor Data-Record to be stored in this Slot.
	@Actor: Target Actor for Code Reflection Analysis. */
	UFUNCTION(Category="Savior", meta=(DisplayName="[SAVIOR] Generate (Complex) Record : ACTOR", BlueprintInternalUseOnly=true))
	FSaviorRecord GenerateRecord_Actor(const AActor* Actor);
	//
	//
	/** Unpacks (Complex) Object Data-Record from this Slot.
	@Object: Target Object for Code Reflection Analysis. */
	UFUNCTION(Category="Savior", meta=(DisplayName="[SAVIOR] Unpack (Complex) Record : OBJECT", BlueprintInternalUseOnly=true, ExpandEnumAsExecs="Result"))
	void UnpackRecord_Object(const FSaviorRecord &Record, UObject* Object, ESaviorResult &Result);
	//
	/** Unpacks (Complex) Component Data-Record from this Slot.
	@Component: Target Component for Code Reflection Analysis. */
	UFUNCTION(Category="Savior", meta=(DisplayName="[SAVIOR] Unpack (Complex) Record : COMPONENT", BlueprintInternalUseOnly=true, ExpandEnumAsExecs="Result"))
	void UnpackRecord_Component(const FSaviorRecord &Record, UActorComponent* Component, ESaviorResult &Result);
	//
	/** Unpacks (Complex) Actor Data-Record from this Slot.
	@Actor: Target Actor for Code Reflection Analysis. */
	UFUNCTION(Category="Savior", meta=(DisplayName="[SAVIOR] Unpack (Complex) Record : ACTOR", BlueprintInternalUseOnly=true, ExpandEnumAsExecs="Result"))
	void UnpackRecord_Actor(const FSaviorRecord &Record, AActor* Actor, ESaviorResult &Result);
	//
	//
	/** Generates (Minimal) Object Data-Record to be stored in this Slot.
	@Object: Target Object for Code Reflection Analysis. */
	UFUNCTION(Category="Savior", meta=(DisplayName="[SAVIOR] Generate (Minimal) Record : OBJECT", BlueprintInternalUseOnly=true))
	FSaviorMinimal GenerateMinimalRecord_Object(const UObject* Object);
	//
	/** Generates (Minimal) Component Data-Record to be stored in this Slot.
	@Component: Target Component for Code Reflection Analysis. */
	UFUNCTION(Category="Savior", meta=(DisplayName="[SAVIOR] Generate (Minimal) Record : COMPONENT", BlueprintInternalUseOnly=true))
	FSaviorMinimal GenerateMinimalRecord_Component(const UActorComponent* Component);
	//
	/** Generates (Minimal) Actor Data-Record to be stored in this Slot.
	@Actor: Target Actor for Code Reflection Analysis. */
	UFUNCTION(Category="Savior", meta=(DisplayName="[SAVIOR] Generate (Minimal) Record : ACTOR", BlueprintInternalUseOnly=true))
	FSaviorMinimal GenerateMinimalRecord_Actor(const AActor* Actor);
	//
	//
	/** Unpacks (Minimal) Object Data-Record from this Slot.
	@Object: Target Object for Code Reflection Analysis. */
	UFUNCTION(Category="Savior", meta=(DisplayName="[SAVIOR] Unpack (Minimal) Record : OBJECT", BlueprintInternalUseOnly=true, ExpandEnumAsExecs="Result"))
	void UnpackMinimalRecord_Object(const FSaviorMinimal &Record, UObject* Object, ESaviorResult &Result);
	//
	/** Unpacks (Minimal) Component Data-Record from this Slot.
	@Component: Target Component for Code Reflection Analysis. */
	UFUNCTION(Category="Savior", meta=(DisplayName="[SAVIOR] Unpack (Minimal) Record : COMPONENT", BlueprintInternalUseOnly=true, ExpandEnumAsExecs="Result"))
	void UnpackMinimalRecord_Component(const FSaviorMinimal &Record, UActorComponent* Component, ESaviorResult &Result);
	//
	/** Unpacks (Minimal) Actor Data-Record from this Slot.
	@Actor: Target Actor for Code Reflection Analysis. */
	UFUNCTION(Category="Savior", meta=(DisplayName="[SAVIOR] Unpack (Minimal) Record : ACTOR", BlueprintInternalUseOnly=true, ExpandEnumAsExecs="Result"))
	void UnpackMinimalRecord_Actor(const FSaviorMinimal &Record, AActor* Actor, ESaviorResult &Result);
	//
	//
	/** Retrieves a copy of Slot's Data. */
	UFUNCTION(Category="Savior", BlueprintPure, meta=(DisplayName="[SAVIOR] Get Slot Data"))
	TMap<FName,FSaviorRecord>GetSlotDataCopy() const;
	//
	/** Searches record data by Object ID on this Slot.
	Returns empty record if nothing was found. */
	UFUNCTION(Category="Savior|Data OPS", BlueprintPure, meta=(DisplayName="[SAVIOR] Find Record (by name)"))
	FSaviorRecord FindRecordByName(const FName &ID) const;
	//
	/** Searches record data with GUID on this Slot.
	Returns empty record if nothing was found. */
	UFUNCTION(Category="Savior|Data OPS", BlueprintPure, meta=(DisplayName="[SAVIOR] Find Record (by guid)"))
	FSaviorRecord FindRecordByGUID(const FGuid &ID) const;
	//
	/** Adds foreign data record to this Slot. */
	UFUNCTION(Category="Savior|Data OPS", BlueprintCallable, meta=(DisplayName="[SAVIOR] Append Records", ExpandEnumAsExecs="Result"))
	void AppendDataRecords(const TMap<FName,FSaviorRecord>&Records, ESaviorResult &Result);
	//
	/** Adds foreign data record to this Slot. */
	UFUNCTION(Category="Savior|Data OPS", BlueprintCallable, meta=(DisplayName="[SAVIOR] Insert Record", ExpandEnumAsExecs="Result"))
	void InsertDataRecord(const FName &ID, const FSaviorRecord &Record, ESaviorResult &Result);
	//
	/** Removes data record from this Slot by name. */
	UFUNCTION(Category="Savior|Data OPS", BlueprintCallable, meta=(DisplayName="[SAVIOR] Remove Record (by name)", ExpandEnumAsExecs="Result"))
	void RemoveDataRecordByName(const FName &ID, ESaviorResult &Result);
	//
	/** Removes data record from this Slot by ID. */
	UFUNCTION(Category="Savior|Data OPS", BlueprintCallable, meta=(DisplayName="[SAVIOR] Remove Record (by guid)", ExpandEnumAsExecs="Result"))
	void RemoveDataRecordByGUID(const FGuid &ID, ESaviorResult &Result);
	//
	/** Removes data record from this Slot by reference. */
	UFUNCTION(Category="Savior|Data OPS", BlueprintCallable, meta=(DisplayName="[SAVIOR] Remove Record (by ref)", ExpandEnumAsExecs="Result"))
	void RemoveDataRecordByRef(const FSaviorRecord &Record, ESaviorResult &Result);
	///
	/// :: UTILITY ::
	///
	/** Checks if a given record is empty data. */
	UFUNCTION(Category="Savior", BlueprintPure, meta=(DisplayName="[SAVIOR] Make Actor ID"))
	static FName MakeActorID(AActor* Actor, bool FullPathID);
	//
	/** Checks if a given record is empty data. */
	UFUNCTION(Category="Savior", BlueprintPure, meta=(DisplayName="[SAVIOR] Make Component ID"))
	static FName MakeComponentID(UActorComponent* Component, bool FullPathID);
	//
	/** Checks if a given record is empty data. */
	UFUNCTION(Category="Savior", BlueprintPure, meta=(DisplayName="[SAVIOR] Make Object ID"))
	static FName MakeObjectID(UObject* Object, bool FullPathID);
	//
	/** Checks if a given record is empty data. */
	UFUNCTION(Category="Savior|Data OPS", BlueprintPure, meta=(DisplayName="[SAVIOR] Is Record Empty"))
	static bool IsRecordEmpty(const FSaviorRecord &Record);
	//
	/** Checks if a given record is marked to self-destroy. */
	UFUNCTION(Category="Savior|Data OPS", BlueprintPure, meta=(DisplayName="[SAVIOR] Is Marked Destroyed"))
	static bool IsRecordMarkedDestroy(const FSaviorRecord &Record);
	//
	//
	/** Marks object to self-destroy on Level Load;
	Target Object must contain a Boolean Property, named 'Destroyed', which is accessed via Code Reflection in Runtime.
	@Object: Target Object to Auto-Destroy. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Mark Object (Auto-Destroy)", Keywords="Mark,Destroy"))
	static ESaviorResult MarkObjectAutoDestroyed(UObject* Object);
	//
	/** Marks Component to self-destroy on Level Load;
	Target Component must contain a Boolean Property, named 'Destroyed', which is accessed via Code Reflection in Runtime.
	@Component: Target Component to Auto-Destroy. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Mark Component (Auto-Destroy)", Keywords="Mark,Destroy"))
	static ESaviorResult MarkComponentAutoDestroyed(UActorComponent* Component);
	//
	/** Marks Actor to self-destroy on Level Load;
	Target Actor must contain a Boolean Property, named 'Destroyed', which is accessed via Code Reflection in Runtime.
	@Actor: Target Actor to Auto-Destroy. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Mark Actor (Auto-Destroy)", Keywords="Mark,Destroy"))
	static ESaviorResult MarkActorAutoDestroyed(AActor* Actor);
	//
	/** Checks if an Object is Marked to be Auto-Destroyed. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Is Object Marked (Auto-Destroy)", Keywords="Mark,Destroy"))
	static bool IsObjectMarkedAutoDestroy(UObject* Object);
	//
	/** Checks if a Component is Marked to be Auto-Destroyed. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Is Component Marked (Auto-Destroy)", Keywords="Mark,Destroy"))
	static bool IsComponentMarkedAutoDestroy(UActorComponent* Component);
	//
	/** Checks if a Actor is Marked to be Auto-Destroyed. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Is Actor Marked (Auto-Destroy)", Keywords="Mark,Destroy"))
	static bool IsActorMarkedAutoDestroy(AActor* Actor);
	//
	//
	/** Sets Value of 'Default Player ID' in Savior 3 Settings. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Set Default Player ID"))
	static void SetDefaultPlayerID(const int32 NewID);
	//
	/** Sets Value of 'Default Player Name' in Savior 3 Settings. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Set Default Player Name"))
	static void SetDefaultPlayerName(const FString NewName);
	//
	/** Sets Value of 'Default Player Level' in Savior 3 Settings. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Set Default Player Level"))
	static void SetDefaultPlayerLevel(const int32 NewLevel);
	//
	/** Sets Value of 'Default Chapter' in Savior 3 Settings. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Set Default Chapter"))
	static void SetDefaultChapter(const FString NewChapter);
	//
	/** Sets Value of 'Default Location' in Savior 3 Settings. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Set Default Save Location"))
	static void SetDefaultLocation(const FString NewLocation);
	//
	//
	/** Creates a new GUID only when Object's 'SGUID' is invalid.
	If existing SGUID is valid then no new values are generated for this Object.
	This is important for Runtime-Generated Objects that require a 'SGUID' to respawn from Save Data. */
	UFUNCTION(Category="Savior", BlueprintPure, meta=(DisplayName="[SAVIOR] Create Once : SGUID", WorldContext="Context"))
	static FGuid CreateSGUID(UObject* Context);
	//
	/** Checks if this Object's 'SGUID' (if any) matches compared Target Object's SGUID. */
	UFUNCTION(Category="Savior", BlueprintPure, meta=(DisplayName="[SAVIOR] Matches : SGUID", WorldContext="Context"))
	static bool MatchesGUID(UObject* Context, UObject* ComparedTo);
	//
	/** Look up current map (UWorld) for any Actor containing specified 'SGUID' property.
	Returns nothing if any Actor with GUID was not found in the level, check the returning value.
	@SGUID: Target Actor must contain a 'SGUID' Property of type FGuid marked 'SaveGame'. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Find Actor With GUID", WorldContext="Context"))
	static AActor* FindActorWithGUID(UObject* Context, const FGuid &SGUID);
	//
	//
	/** Creates a new GUID from Object's full name path.
	The Guid generated ignores instance numbers auto-generated from class ('_C_0', '_C_1'),
	So this is useful for dynamically spawned classes that should always generate the same SGUID.
	@Object: The target Object Reference a Guid will be generated from.
	@Mode: The Guid generation mode intended for this type of object:
			'Static Name to GUID' - Converts Object's name to a deterministic Guid value. (fast)
			'Resolved Name To GUID' - Same as Static, except automatically resolving Guid conflicts. (expensive)
			'Memory Address To GUID' - Converts Object to Guid, considers Object's address in memory. (expensive) */
	UFUNCTION(Category="Savior", BlueprintPure, meta=(DisplayName="[SAVIOR] Make SGUID (Object)"))
	static FGuid MakeObjectGUID(UObject* Object, const EGuidGeneratorMode Mode);
	//
	/** Creates a new GUID from Component's full name path.
	The Guid generated ignores instance numbers auto-generated from class ('_C_0', '_C_1'),
	So this is useful for dynamically spawned classes that should always generate the same SGUID.
	@Instance: The target Component Reference a Guid will be generated from.
	@Mode: The Guid generation mode intended for this type of object:
			'Static Name to GUID' - Converts Component's name to a deterministic Guid value. (fast)
			'Resolved Name To GUID' - Same as Static, except automatically resolving Guid conflicts. (expensive)
			'Memory Address To GUID' - Converts Component to Guid, considers Component's address in memory. (expensive) */
	UFUNCTION(Category="Savior", BlueprintPure, meta=(DisplayName="[SAVIOR] Make SGUID (Component)"))
	static FGuid MakeComponentGUID(UActorComponent* Instance, const EGuidGeneratorMode Mode);
	//
	/** Creates a new GUID from Actor's full name path.
	The Guid generated ignores instance numbers auto-generated from class ('_C_0', '_C_1'),
	So this is useful for dynamically spawned classes that should always generate the same SGUID.
	@Instance: The target Actor Reference a Guid will be generated from.
	@Mode: The Guid generation mode intended for this type of object:
			'Static Name to GUID' - Converts Actor's name to a deterministic Guid value. (fast)
			'Resolved Name To GUID' - Same as Static, except automatically resolving Guid conflicts. (expensive)
			'Memory Address To GUID' - Converts Actor to Guid, considers Actor's address in memory. (expensive) */
	UFUNCTION(Category="Savior", BlueprintPure, meta=(DisplayName="[SAVIOR] Make SGUID (Actor)"))
	static FGuid MakeActorGUID(AActor* Instance, const EGuidGeneratorMode Mode);
	//
	//
	/** Query SGUID Property from an Object without Casting.
	Returns an invalid Guid if SGUID Property not found. */
	UFUNCTION(Category="Savior", BlueprintPure, meta=(DisplayName="[SAVIOR] Get SGUID (Object)"))
	static FGuid GetObjectGUID(UObject* Instance);
	//
	/** Query SGUID Property from a Component without Casting.
	Returns an invalid Guid if SGUID Property not found. */
	UFUNCTION(Category="Savior", BlueprintPure, meta=(DisplayName="[SAVIOR] Get SGUID (Component)"))
	static FGuid GetComponentGUID(UActorComponent* Instance);
	//
	/** Query SGUID Property from an Actor without Casting.
	Returns an invalid Guid if SGUID Property not found. */
	UFUNCTION(Category="Savior", BlueprintPure, meta=(DisplayName="[SAVIOR] Get SGUID (Actor)"))
	static FGuid GetActorGUID(AActor* Instance);
	//
	//
	/** Try to Load Actor's Properties from a data record directly (if any).
	@Actor: Object Reference loaded Properties are written to. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Object : (Slot Data)", ExpandEnumAsExecs="Result"))
	static void LoadObjectData(UObject* Object, const FSaviorRecord &Data, ESaviorResult &Result);
	//
	/** Try to Load Actor's Properties from a data record directly (if any).
	@Actor: Object Reference loaded Properties are written to. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Component : (Slot Data)", ExpandEnumAsExecs="Result"))
	static void LoadComponentData(UActorComponent* Component, const FSaviorRecord &Data, ESaviorResult &Result);
	//
	/** Try to Load Actor's Properties from a data record directly (if any).
	@Actor: Object Reference loaded Properties are written to. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Actor : (Slot Data)", ExpandEnumAsExecs="Result"))
	static void LoadActorData(AActor* Actor, const FSaviorRecord &Data, ESaviorResult &Result);
	//
	//
	/** Try to Load Object's Properties from Slot in memory (if any).
	@Object: Object Reference loaded Properties are written to. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Object : (Static)", WorldContext="Context", ExpandEnumAsExecs="Result"))
	static void StaticLoadObject(UObject* Context, UObject* Object, ESaviorResult &Result);
	//
	/** Try to Load Component's Properties from Slot in memory (if any).
	@Component: Object Reference loaded Properties are written to. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Component : (Static)", WorldContext="Context", ExpandEnumAsExecs="Result"))
	static void StaticLoadComponent(UObject* Context, UActorComponent* Component, ESaviorResult &Result);
	//
	/** Try to Load Actor's Properties from Slot in memory (if any).
	@Actor: Object Reference loaded Properties are written to. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Actor : (Static)", WorldContext="Context", ExpandEnumAsExecs="Result"))
	static void StaticLoadActor(UObject* Context, AActor* Actor, ESaviorResult &Result);
	//
	//
	/** Creates a new Runtime Instance of a Savior Slot. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] New Slot Instance", WorldContext="Context", ExpandEnumAsExecs="Result"))
	static USavior3* NewSlotInstance(UObject* Context, USavior3* Slot, ESaviorResult &Result);
	//
	/** Loads the Default (Original, Root) Instance of an Object. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta = (DisplayName="[SAVIOR] Get Object of Class", Keywords="Get,Object"))
	static UObject* GetClassDefaultObject(UClass* Class);
	//
	/** Creates a Runtime New Instance of an Object. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta = (DisplayName="[SAVIOR] New Object Instance", WorldContext="Context", Keywords="New,Object,Instance"))
	static UObject* NewObjectInstance(UObject* Context, UClass* Class);
	//
	/** Creates a Runtime New Named Instance of an Object. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta = (DisplayName="[SAVIOR] New Object Instance", WorldContext="Context", Keywords="New,Object,Instance"))
	static UObject* NewNamedObjectInstance(UObject* Context, UClass* Class, FName Name);
	//
	//
	/** Generates a new valid GUID value. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta = (DisplayName="[SAVIOR] New Object GUID", Keywords="New,GUID"))
	static FGuid NewObjectGUID();
	//
	//
	/** Transfers in-memory data from one existing Slot instance to another. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Slot Shadow Copy", ExpandEnumAsExecs="Result"))
	static USavior3* ShadowCopySlot(USavior3* From, USavior3* To, ESaviorResult &Result);
	//
	//
	/** Retrieves Slot's Thumbnail Image Asset, converted to 2D Texture, making it usable by UI Widgets. */
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Get Slot Thumbnail"))
	UTexture2D* GetSlotThumbnail(FVector2D &ImageSize);
	//
	/** Retrieves Slot's Save Time (Hour : Minutes) as Formatted String for UI/UX. */
	UFUNCTION(Category = "Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Get Save-Time ISO"))
	FString GetSaveTimeISO();
	//
	/** Retrieves Slot's Save Date as Formatted String for UI/UX. */
	UFUNCTION(Category = "Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Get Save-Date ISO"))
	FString GetSaveDateISO();
	//
	//
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Get Thread Safety"))
	static EThreadSafety GetThreadSafety() {return USavior3::ThreadSafety;}
	//
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Get Save Progress"))
	static float GetSaveProgress() {return USavior3::SS_Progress;}
	//
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Get Load Progress"))
	static float GetLoadProgress() {return USavior3::SL_Progress;}
	//
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Get Saves Done"))
	static float GetSavesDone() {return USavior3::SS_Complete;}
	//
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Get Loads Done"))
	static float GetLoadsDone() {return USavior3::SL_Complete;}
	//
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Calculate Save Workload"))
	const float GetSaveWorkload() {return (USavior3::SS_Workload=CalculateWorkload());}
	//
	UFUNCTION(Category="Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Calculate Load Workload"))
	const float GetLoadWorkload() {return (USavior3::SL_Workload=CalculateWorkload());}
	//
	//
	UFUNCTION(Category="UI/UX", BlueprintCallable, meta=(CompactNodeTitle="Set Chapter")) void SetChapter(FString New);
	UFUNCTION(Category="UI/UX", BlueprintCallable, meta=(CompactNodeTitle="Set Progress")) void SetProgress(float New);
	UFUNCTION(Category="UI/UX", BlueprintCallable, meta=(CompactNodeTitle="Set Play Time")) void SetPlayTime(int32 New);
	UFUNCTION(Category="UI/UX", BlueprintCallable, meta=(CompactNodeTitle="Set Save Date")) void SetSaveDate(FDateTime New);
	UFUNCTION(Category="UI/UX", BlueprintCallable, meta=(CompactNodeTitle="Set Player Level")) void SetPlayerLevel(int32 New);
	UFUNCTION(Category="UI/UX", BlueprintCallable, meta=(CompactNodeTitle="Set Player Name")) void SetPlayerName(FString New);
	UFUNCTION(Category="UI/UX", BlueprintCallable, meta=(CompactNodeTitle="Set Save Location")) void SetSaveLocation(FString New);
	//
	UFUNCTION(Category="UI/UX", BlueprintCallable, meta=(CompactNodeTitle="Get Chapter")) FString GetChapter() const;
	UFUNCTION(Category="UI/UX", BlueprintCallable, meta=(CompactNodeTitle="Get Progress")) float GetProgress() const;
	UFUNCTION(Category="UI/UX", BlueprintCallable, meta=(CompactNodeTitle="Get Play Time")) int32 GetPlayTime() const;
	UFUNCTION(Category="UI/UX", BlueprintCallable, meta=(CompactNodeTitle="Get Save Date")) FDateTime GetSaveDate() const;
	UFUNCTION(Category="UI/UX", BlueprintCallable, meta=(CompactNodeTitle="Get Player Name")) FString GetPlayerName() const;
	UFUNCTION(Category="UI/UX", BlueprintCallable, meta=(CompactNodeTitle="Get Player Level")) int32 GetPlayerLevel() const;
	UFUNCTION(Category="UI/UX", BlueprintCallable, meta=(CompactNodeTitle="Get Save Location")) FString GetSaveLocation() const;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UCLASS(ClassGroup=Synaptech,Category="Synaptech")
class SAVIOR3_API USaviorSubsystem : public UGameInstanceSubsystem {
	GENERATED_BODY()
public:
	UPROPERTY() USavior3* StaticSlot;
public:
	virtual void Initialize(FSubsystemCollectionBase &Collection) override;
	virtual void Deinitialize() override;
	//
	void Reset();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior Utilities:

/** Return True of an Actor hosts an specific Tag in its Tag List. */
SAVIOR3_API bool ParseActorTAG(const AActor* Actor, const FName Tag);

/** Return True of a Component hosts an specific Tag in its Tag List. */
SAVIOR3_API bool ParseComponentTAG(const UActorComponent* Component, const FName Tag);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior Threading Utilities:

static void OnPreparedToSave() {
	for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
		if (!OBJ->IsValidLowLevelFast()) {continue;}
		if (OBJ->IsA(USavior3::StaticClass())) {continue;}
		if (OBJ->IsA(USaveGame::StaticClass())) {continue;}
		if (OBJ->HasAnyFlags(RF_ClassDefaultObject|RF_BeginDestroyed)) {continue;}
		//
		const auto &Interface = Cast<ISAVIOR_Serializable>(*OBJ);
		//
		if (Interface) {Interface->Execute_OnPrepareToSave(*OBJ);}
		else if ((*OBJ)->GetClass()->ImplementsInterface(USAVIOR_Serializable::StaticClass())) {
			ISAVIOR_Serializable::Execute_OnPrepareToSave(*OBJ);
		}///
	}///
}

static void OnPreparedToLoad() {
	for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
		if (!OBJ->IsValidLowLevelFast()) {continue;}
		if (OBJ->IsA(USavior3::StaticClass())) {continue;}
		if (OBJ->IsA(USaveGame::StaticClass())) {continue;}
		if (OBJ->HasAnyFlags(RF_ClassDefaultObject|RF_BeginDestroyed)) {continue;}
		//
		const auto &Interface = Cast<ISAVIOR_Serializable>(*OBJ);
		//
		if (Interface) {Interface->Execute_OnPrepareToLoad(*OBJ);}
		else if ((*OBJ)->GetClass()->ImplementsInterface(USAVIOR_Serializable::StaticClass())) {
			ISAVIOR_Serializable::Execute_OnPrepareToLoad(*OBJ);
		}///
	}///
}

static void OnObjectSaved(UObject* Object) {
	const auto &Interface = Cast<ISAVIOR_Serializable>(Object);
	//
	if (Interface) {Interface->Execute_OnSaved(Object);}
	else if (Object->GetClass()->ImplementsInterface(USAVIOR_Serializable::StaticClass())) {
		ISAVIOR_Serializable::Execute_OnSaved(Object);
	}///
}

static void OnAnimationSaved(UAnimInstance* Anim) {
	const auto &Interface = Cast<ISAVIOR_Serializable>(Anim);
	//
	if (Interface) {Interface->Execute_OnSaved(Anim);}
	else if (Anim->GetClass()->ImplementsInterface(USAVIOR_Serializable::StaticClass())) {
		ISAVIOR_Serializable::Execute_OnSaved(Anim);
	}///
}

static void OnActorSaved(AActor* Actor) {
	const auto &Interface = Cast<ISAVIOR_Serializable>(Actor);
	//
	if (Interface) {Interface->Execute_OnSaved(Actor);}
	else if (Actor->GetClass()->ImplementsInterface(USAVIOR_Serializable::StaticClass())) {
		ISAVIOR_Serializable::Execute_OnSaved(Actor);
	}///
}

static void OnComponentSaved(UActorComponent* Component) {
	const auto &Interface = Cast<ISAVIOR_Serializable>(Component);
	//
	if (Interface) {Interface->Execute_OnSaved(Component);}
	else if (Component->GetClass()->ImplementsInterface(USAVIOR_Serializable::StaticClass())) {
		ISAVIOR_Serializable::Execute_OnSaved(Component);
	}///
}

static void OnGameInstanceSaved(UGameInstance* Instance) {
	const auto &Interface = Cast<ISAVIOR_Serializable>(Instance);
	//
	if (Interface) {Interface->Execute_OnSaved(Instance);}
	else if (Instance->GetClass()->ImplementsInterface(USAVIOR_Serializable::StaticClass())) {
		ISAVIOR_Serializable::Execute_OnSaved(Instance);
	}///
}

static void OnObjectLoaded(const FSlotMeta Meta, UObject* Object) {
	const auto &Interface = Cast<ISAVIOR_Serializable>(Object);
	//
	if (Interface) {Interface->Execute_OnLoaded(Object,Meta);}
	else if (Object->GetClass()->ImplementsInterface(USAVIOR_Serializable::StaticClass())) {
		ISAVIOR_Serializable::Execute_OnLoaded(Object,Meta);
	}///
}

static void OnAnimationLoaded(const FSlotMeta Meta, UAnimInstance* Anim) {
	const auto &Interface = Cast<ISAVIOR_Serializable>(Anim);
	//
	if (Interface) {Interface->Execute_OnLoaded(Anim,Meta);}
	else if (Anim->GetClass()->ImplementsInterface(USAVIOR_Serializable::StaticClass())) {
		ISAVIOR_Serializable::Execute_OnLoaded(Anim,Meta);
	}///
}

static void OnActorLoaded(const FSlotMeta Meta, AActor* Actor) {
	const auto &Interface = Cast<ISAVIOR_Serializable>(Actor);
	//
	if (Interface) {Interface->Execute_OnLoaded(Actor,Meta);}
	else if (Actor->GetClass()->ImplementsInterface(USAVIOR_Serializable::StaticClass())) {
		ISAVIOR_Serializable::Execute_OnLoaded(Actor,Meta);
	}///
}

static void OnComponentLoaded(const FSlotMeta Meta, UActorComponent* Component) {
	const auto &Interface = Cast<ISAVIOR_Serializable>(Component);
	//
	if (Interface) {Interface->Execute_OnLoaded(Component,Meta);}
	else if (Component->GetClass()->ImplementsInterface(USAVIOR_Serializable::StaticClass())) {
		ISAVIOR_Serializable::Execute_OnLoaded(Component,Meta);
	}///
}

static void OnGameInstanceLoaded(const FSlotMeta Meta, UGameInstance* Instance) {
	const auto &Interface = Cast<ISAVIOR_Serializable>(Instance);
	//
	if (Interface) {Interface->Execute_OnLoaded(Instance,Meta);}
	else if (Instance->GetClass()->ImplementsInterface(USAVIOR_Serializable::StaticClass())) {
		ISAVIOR_Serializable::Execute_OnLoaded(Instance,Meta);
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior Threading Interface [Game World]:

static void OnFinishSerializeGameWorld(USavior3* Savior, ESaviorResult Result) {
	const auto &Settings = GetDefault<USavior3Settings>();
	//
	Savior->ClearWorkload();
	Savior->WriteSlotToFile(Settings->DefaultPlayerID,Result);
	Savior->EVENT_OnFinishDataSAVE.Broadcast(Result==ESaviorResult::Success);
	//
	if (Savior->PauseGameOnLoad) {
		if (auto PC=Savior->GetWorld()->GetFirstPlayerController()){PC->SetPause(false);}
	}///
}

///	(C) 2021 - Bruno Xavier Leite
static void SerializeGameWorld(USavior3* Savior) {
	if (Savior==nullptr||!Savior->IsValidLowLevelFast()||Savior->GetWorld()==nullptr) {return;}
	//
	const auto &Settings = GetDefault<USavior3Settings>();
	ESaviorResult HierarchyResult;
	ESaviorResult Result;
	//
	//
	for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
		if (!OBJ->IsValidLowLevelFast()) {continue;}
		if (OBJ->IsA(USavior3::StaticClass())) {continue;}
		if (OBJ->IsA(USaveGame::StaticClass())) {continue;}
		//
		if (OBJ->IsA(UGameInstance::StaticClass())) {
			Savior->SaveGameInstanceSingleTon(CastChecked<UGameInstance>(*OBJ),Result);
		continue;}
		//
		if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
		if (OBJ->GetWorld()!=Savior->GetWorld()) {continue;}
		if (OBJ->HasAnyFlags(RF_ClassDefaultObject|RF_BeginDestroyed)) {continue;}
		//
		bool IsTarget = false;
		for (auto ACT : Savior->ActorScope) { if (ACT.Get()&&(OBJ->IsA(ACT))) {IsTarget=true; break;} }
		for (auto OBS : Savior->ObjectScope) { if (OBS.Get()&&(OBJ->IsA(OBS))) {IsTarget=true; break;} }
		for (auto COM : Savior->ComponentScope) { if (COM.Get()&&(OBJ->IsA(COM))) {IsTarget=true; break;} }
		//
		if (IsTarget) {
			if (OBJ->IsA(UActorComponent::StaticClass())) {Savior->SaveComponent(Savior->GetWorld(),CastChecked<UActorComponent>(*OBJ),Result);} else
			if (OBJ->IsA(AActor::StaticClass())) {Savior->SaveActor(Savior->GetWorld(),CastChecked<AActor>(*OBJ),HierarchyResult);}
			else {Savior->SaveObjectHierarchy((*OBJ),Result,HierarchyResult);}
		}///
	}///
	//
	//
	Result = ESaviorResult::Success;
	if (IsInGameThread()) {OnFinishSerializeGameWorld(Savior,Result);} else {
		FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			FSimpleDelegateGraphTask::FDelegate::CreateStatic(&OnFinishSerializeGameWorld,Savior,Result),
			GET_STATID(STAT_FSimpleDelegateGraphTask_SerializeGameWorld),
			nullptr, ENamedThreads::GameThread
		);//
	}///
}

static void OnFinishDeserializeGameWorld(USavior3* Savior, ESaviorResult Result) {
	if (Savior==nullptr||Savior->GetWorld()==nullptr) {return;}
	const auto &Settings = GetDefault<USavior3Settings>();
	//
	Savior->ClearWorkload();
	Savior->EVENT_OnFinishDataLOAD.Broadcast(Result==ESaviorResult::Success);
	//
	const auto &PlayerController = UGameplayStatics::GetPlayerController(Savior,Settings->DefaultPlayerID);
	const auto &Pawn = UGameplayStatics::GetPlayerPawn(Savior,Settings->DefaultPlayerID);
	//
	if ((PlayerController!=nullptr)&&(Pawn!=nullptr)) {
		if (PlayerController->GetRootComponent()) {PlayerController->GetRootComponent()->SetUsingAbsoluteRotation(true);}
		PlayerController->SetControlRotation(Pawn->ActorToWorld().GetRotation().Rotator());
		if (PlayerController->GetRootComponent()) {PlayerController->GetRootComponent()->SetUsingAbsoluteRotation(false);}
	}///
	//
	if (Savior->PauseGameOnLoad) {
		if (auto PC=Savior->GetWorld()->GetFirstPlayerController()){PC->SetPause(false);}
	}///
	//
	USaviorSubsystem* System = nullptr;
	if (UGameInstance*const&GI=Savior->GetWorld()->GetGameInstance()) {
		System = GI->GetSubsystem<USaviorSubsystem>();
	} if (System) {System->StaticSlot->Reset();}
}

///	(C) 2021 - Bruno Xavier Leite
static void DeserializeGameWorld(USavior3* Savior) {
	if (Savior==nullptr||Savior->GetWorld()==nullptr) {return;}
	const auto &Settings = GetDefault<USavior3Settings>();
	//
	ESaviorResult Result;
	USaviorSubsystem* System = nullptr;
	//
	if (UGameInstance*const&GI=Savior->GetWorld()->GetGameInstance()) {
		System = GI->GetSubsystem<USaviorSubsystem>();
	} if (System) {USavior3::ShadowCopySlot(Savior,System->StaticSlot,Result);}
	//
	TMap<FName,FSaviorRecord>SlotData = Savior->GetSlotDataCopy();
	TMap<FName,FSaviorRecord>DES_Data = SlotData;
	TMap<FName,FSaviorRecord>ACT_Data;
	TMap<FName,FSaviorRecord>CMP_Data;
	//
	if (Savior->Compression==ERecordType::Complex) {
		for (auto Pair : DES_Data) {
			if (Pair.Value.Destroyed) {SlotData.Remove(Pair.Key); continue;}
		} DES_Data.Empty();
	}///
	//
	if ((Settings->RespawnDynamicActors)&&(Savior->Compression==ERecordType::Complex)) {
		for (auto Pair : SlotData) {
			if (FPackageName::IsShortPackageName(Pair.Value.FullName)) {continue;}
			//
			const FSoftObjectPath ObjectPath(Pair.Value.FullName);
			const auto &OBJ = ObjectPath.ResolveObject();
			if (OBJ!=nullptr) {continue;}
			//
			const FSoftObjectPath ClassPath(Pair.Value.ClassPath);
			const auto CDO = ClassPath.ResolveObject();
			//
			if ((CDO!=nullptr)&&(CDO->IsValidLowLevelFast())&&(CDO->IsA(AActor::StaticClass()))) {
				const auto &IT = Cast<ISAVIOR_Procedural>(CDO);
				if (IT||CDO->GetClass()->ImplementsInterface(USAVIOR_Procedural::StaticClass())) {
					ACT_Data.Emplace(Pair.Key,Pair.Value);
				}///
			}///
		}///
		//
		for (auto Pair : ACT_Data) {
			if (IsInGameThread()) {Reflector::RespawnActorFromData(Savior->GetWorld(),Settings->RespawnScope,Pair.Value);} else {
				FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
					FSimpleDelegateGraphTask::FDelegate::CreateStatic(&Reflector::RespawnActorFromData,Savior->GetWorld(),Settings->RespawnScope,Pair.Value),
					GET_STATID(STAT_FSimpleDelegateGraphTask_RespawnActorFromData),
					nullptr, ENamedThreads::GameThread
				);//
			}///
		}///
	}///
	//
	if ((Settings->RespawnDynamicComponents)&&(Savior->Compression==ERecordType::Complex)) {
		for (auto Pair : SlotData) {
			if (FPackageName::IsShortPackageName(Pair.Value.FullName)) {continue;}
			//
			const FSoftObjectPath ObjectPath(Pair.Value.FullName);
			const auto &OBJ = ObjectPath.ResolveObject();
			if (OBJ!=nullptr) {continue;}
			//
			const FSoftObjectPath ClassPath(Pair.Value.ClassPath);
			const auto CDO = ClassPath.ResolveObject();
			//
			if ((CDO!=nullptr)&&(CDO->IsA(UActorComponent::StaticClass()))) {
				const auto &IT = Cast<ISAVIOR_Procedural>(CDO);
				if (IT||CDO->GetClass()->ImplementsInterface(USAVIOR_Procedural::StaticClass())) {
					CMP_Data.Emplace(Pair.Key,Pair.Value);
				}///
			}///
		}///
		//
		for (auto Pair : CMP_Data) {
			if (IsInGameThread()) {Reflector::RespawnComponentFromData(Savior->GetWorld(),Settings->RespawnScope,Pair.Value);} else {
				FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
					FSimpleDelegateGraphTask::FDelegate::CreateStatic(&Reflector::RespawnComponentFromData,Savior->GetWorld(),Settings->RespawnScope,Pair.Value),
					GET_STATID(STAT_FSimpleDelegateGraphTask_RespawnComponentFromData),
					nullptr, ENamedThreads::GameThread
				);//
			}///
		}///
	}///
	//
	//
	for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
		if (!OBJ->IsValidLowLevelFast()) {continue;}
		if (OBJ->IsA(USavior3::StaticClass())) {continue;}
		//
		if (OBJ->IsA(UGameInstance::StaticClass())) {
			Savior->LoadGameInstanceSingleTon(CastChecked<UGameInstance>(*OBJ),Result);
		continue;}
		//
		if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
		if (OBJ->GetWorld()!=Savior->GetWorld()) {continue;}
		if (OBJ->HasAnyFlags(RF_ClassDefaultObject|RF_BeginDestroyed)) {continue;}
		//
		bool IsTarget = false;
		for (auto ACT : Savior->ActorScope) { if (ACT.Get()&&(OBJ->IsA(ACT))) {IsTarget=true; break;} }
		for (auto OBS : Savior->ObjectScope) { if (OBS.Get()&&(OBJ->IsA(OBS))) {IsTarget=true; break;} }
		for (auto COM : Savior->ComponentScope) { if (COM.Get()&&(OBJ->IsA(COM))) {IsTarget=true; break;} }
		//
		if (IsTarget) {
			if (OBJ->IsA(UActorComponent::StaticClass())) {
				Savior->LoadComponent(Savior,CastChecked<UActorComponent>(*OBJ),Result);
			} else if (OBJ->IsA(AActor::StaticClass())) {
				Savior->LoadActor(Savior,CastChecked<AActor>(*OBJ),Result);
			} else {Savior->LoadObject((*OBJ),Result);}
		}///
	}///
	//
	//
	Result = ESaviorResult::Success;
	if (IsInGameThread()) {OnFinishDeserializeGameWorld(Savior,Result);} else {
		FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			FSimpleDelegateGraphTask::FDelegate::CreateStatic(&OnFinishDeserializeGameWorld,Savior,Result),
			GET_STATID(STAT_FSimpleDelegateGraphTask_DeserializeGameWorld),
			nullptr, ENamedThreads::GameThread
		);//
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Property Threading Interface:

static void GT_NewObjectInstance(USavior3* Savior, FObjectProperty* Property, UObject* Outer, UObject* CDO, const FString FullName) {
	if (!IsInGameThread()||(Outer==nullptr)||(CDO==nullptr)||(Property==nullptr)) {return;}
	//
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Outer);
	FString SGUID; FGuid GUID; GUID.Invalidate();
	ESaviorResult Result;
	//
	if (FullName.Split(TEXT("_"),nullptr,&SGUID,ESearchCase::IgnoreCase,ESearchDir::FromEnd)) {
		FGuid::Parse(SGUID,GUID);
		//
		if (GUID.IsValid()) {
			for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
				if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
				if (OBJ->HasAnyFlags(RF_BeginDestroyed)) {continue;}
				//
				if (Reflector::FindGUID(*OBJ)==GUID) {
					Savior->LoadObject(*OBJ,Result);
					Property->SetPropertyValue(ValuePtr,(*OBJ));
				return;}
			}///
		}///
	} else {
		for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
			if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
			if (OBJ->HasAnyFlags(RF_BeginDestroyed)) {continue;}
			//
			if ((*OBJ)->IsA(AActor::StaticClass())) {
				if (Reflector::MakeActorID(CastChecked<AActor>(*OBJ),true).ToString()==FullName){Property->SetPropertyValue(ValuePtr,*OBJ); return;}
			} else if ((*OBJ)->IsA(UActorComponent::StaticClass())) {
				if (Reflector::MakeComponentID(CastChecked<UActorComponent>(*OBJ),true).ToString()==FullName) {Property->SetPropertyValue(ValuePtr,*OBJ); return;}
			} else {
				if (Reflector::MakeObjectID(*OBJ).ToString()==FullName) {
					Savior->LoadObject(*OBJ,Result);
					Property->SetPropertyValue(ValuePtr,*OBJ);
				return;}
			}///
		}///
	}///
	//
	//
	FString Name = (SGUID.IsEmpty()) ? CDO->GetName() : (CDO->GetName()+TEXT("_0")).Replace(TEXT("Default__"),TEXT(""));
	auto OBJ = NewObject<UObject>(Outer,CDO->GetClass(),*Name,RF_NoFlags,CDO);
	Reflector::SetGUID(GUID,OBJ);
	//
	if (Savior) {Savior->LoadObject(OBJ,Result);}
	//
	Property->SetPropertyValue(ValuePtr,OBJ);
}

static void GT_NewObjectIntoArray(USavior3* Savior, FArrayProperty* Property, UObject* Outer, UObject* CDO, const FString FullName, const int32 Index) {
	if (!IsInGameThread()||(Outer==nullptr)||(CDO==nullptr)||(Property==nullptr)) {return;}
	//
	TArray<UObject*>&Array = *Property->ContainerPtrToValuePtr<TArray<UObject*>>(Outer);
	if (!Array.IsValidIndex(Index)) {return;}
	//
	ESaviorResult Result;
	FString SGUID; FGuid GUID; GUID.Invalidate();
	if (FullName.Split(TEXT("_"),nullptr,&SGUID,ESearchCase::IgnoreCase,ESearchDir::FromEnd)) {
		FGuid::Parse(SGUID,GUID);
		//
		if (GUID.IsValid()) {
			for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
				if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
				if (OBJ->HasAnyFlags(RF_BeginDestroyed)) {continue;}
				//
				if (Reflector::FindGUID(*OBJ)==GUID) {
					Savior->LoadObject(*OBJ,Result);
					Array[Index]=(*OBJ);
				return;}
			}///
		}///
	} else {
		for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
			if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
			if (OBJ->HasAnyFlags(RF_BeginDestroyed)) {continue;}
			//
			if ((*OBJ)->IsA(AActor::StaticClass())) {
				if (Reflector::MakeActorID(CastChecked<AActor>(*OBJ),true).ToString()==FullName){Array[Index]=(*OBJ); return;}
			} else if ((*OBJ)->IsA(UActorComponent::StaticClass())) {
				if (Reflector::MakeComponentID(CastChecked<UActorComponent>(*OBJ),true).ToString()==FullName) {Array[Index]=(*OBJ); return;}
			} else {
				if (Reflector::MakeObjectID(*OBJ).ToString()==FullName) {
					Savior->LoadObject(*OBJ,Result);
					Array[Index]=(*OBJ);
				return;}
			}///
		}///
	}///
	//
	//
	FString Name = (SGUID.IsEmpty()) ? CDO->GetName() : (CDO->GetName()+TEXT("_")+FString::FromInt(Index)).Replace(TEXT("Default__"),TEXT(""));
	auto OBJ = NewObject<UObject>(Outer,CDO->GetClass(),*Name,RF_NoFlags,CDO);
	Reflector::SetGUID(GUID,OBJ);
	//
	if (Savior) {Savior->LoadObject(OBJ,Result);}
	//
	Array[Index] = (OBJ);
}

static void GT_NewObjectIntoSet(USavior3* Savior, FSetProperty* Property, UObject* Outer, UObject* CDO, const FString FullName) {
	if (!IsInGameThread()||(Outer==nullptr)||(CDO==nullptr)||(Property==nullptr)) {return;}
	//
	TSet<UObject*>&Set = *Property->ContainerPtrToValuePtr<TSet<UObject*>>(Outer);
	//
	ESaviorResult Result;
	FString SGUID; FGuid GUID; GUID.Invalidate();
	if (FullName.Split(TEXT("_"),nullptr,&SGUID,ESearchCase::IgnoreCase,ESearchDir::FromEnd)) {
		FGuid::Parse(SGUID,GUID);
		//
		if (GUID.IsValid()) {
			for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
				if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
				if (OBJ->HasAnyFlags(RF_BeginDestroyed)) {continue;}
				//
				if (Reflector::FindGUID(*OBJ)==GUID) {
					Savior->LoadObject(*OBJ,Result);
					Set.Add(*OBJ);
				return;}
			}///
		}///
	} else {
		for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
			if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
			if (OBJ->HasAnyFlags(RF_BeginDestroyed)) {continue;}
			//
			if ((*OBJ)->IsA(AActor::StaticClass())) {
				if (Reflector::MakeActorID(CastChecked<AActor>(*OBJ),true).ToString()==FullName){Set.Add(*OBJ); return;}
			} else if ((*OBJ)->IsA(UActorComponent::StaticClass())) {
				if (Reflector::MakeComponentID(CastChecked<UActorComponent>(*OBJ),true).ToString()==FullName) {Set.Add(*OBJ); return;}
			} else {
				if (Reflector::MakeObjectID(*OBJ).ToString()==FullName) {
					Savior->LoadObject(*OBJ,Result); Set.Add(*OBJ);
				return;}
			}///
		}///
	}///
	//
	//
	FString Name = (SGUID.IsEmpty()) ? CDO->GetName() : CDO->GetName().Replace(TEXT("Default__"),TEXT(""));
	auto OBJ = NewObject<UObject>(Outer,CDO->GetClass(),*Name,RF_NoFlags,CDO);
	Reflector::SetGUID(GUID,OBJ);
	//
	if (Savior) {Savior->LoadObject(OBJ,Result);}
	//
	Set.Add(OBJ);
}

static void GT_NewObjectIntoMapKey(USavior3* Savior, FMapProperty* Property, UObject* Outer, UObject* CDO, const FString FullName) {
	if (!IsInGameThread()||(Outer==nullptr)||(CDO==nullptr)||(Property==nullptr)) {return;}
	//
	TMap<UObject*,void*>&Map = *Property->ContainerPtrToValuePtr<TMap<UObject*,void*>>(Outer);
	FString SGUID; FGuid GUID; GUID.Invalidate();
	//
	if (FullName.Split(TEXT("_"),nullptr,&SGUID,ESearchCase::IgnoreCase,ESearchDir::FromEnd)) {
		FGuid::Parse(SGUID,GUID);
		//
		if (GUID.IsValid()) {
			for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
				if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
				if (OBJ->HasAnyFlags(RF_BeginDestroyed)) {continue;}
				//
				if (Reflector::FindGUID(*OBJ)==GUID) {
					Map.Add(*OBJ); return;
				}///
			}///
		}///
	}///
	//
	FString Name = (SGUID.IsEmpty()) ? CDO->GetName() : CDO->GetName().Replace(TEXT("Default__"),TEXT(""));
	auto OBJ = NewObject<UObject>(Outer,CDO->GetClass(),*Name,RF_NoFlags,CDO);
	//
	Reflector::SetGUID(GUID,OBJ);
	//
	ESaviorResult Result;
	if (Savior){Savior->LoadObject(OBJ,Result);}
	//
	Map.Add(OBJ);
}

static void GT_NewObjectIntoMapValue(USavior3* Savior, FMapProperty* Property, UObject* Outer, UObject* Key, UObject* CDO, const FString FullName) {
	if (!IsInGameThread()||(Outer==nullptr)||(CDO==nullptr)||(Property==nullptr)) {return;}
	//
	TMap<UObject*,UObject*>&Map = *Property->ContainerPtrToValuePtr<TMap<UObject*,UObject*>>(Outer);
	FString SGUID; FGuid GUID; GUID.Invalidate();
	//
	if (FullName.Split(TEXT("_"),nullptr,&SGUID,ESearchCase::IgnoreCase,ESearchDir::FromEnd)) {
		FGuid::Parse(SGUID,GUID);
		//
		if (GUID.IsValid()) {
			for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
				if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
				if (OBJ->HasAnyFlags(RF_BeginDestroyed)) {continue;}
				//
				if (Reflector::FindGUID(*OBJ)==GUID) {
					Map.Emplace(Key,*OBJ); return;
				}///
			}///
		}///
	}///
	//
	FString Name = (SGUID.IsEmpty()) ? CDO->GetName() : CDO->GetName().Replace(TEXT("Default__"),TEXT(""));
	auto OBJ = NewObject<UObject>(Outer,CDO->GetClass(),*Name,RF_NoFlags,CDO);
	//
	Reflector::SetGUID(GUID,OBJ);
	//
	ESaviorResult Result;
	if (Savior){Savior->LoadObject(OBJ,Result);}
	//
	Map.Emplace(Key,OBJ);
}

static void GT_NewObjectIntoNamedMapValue(USavior3* Savior, FMapProperty* Property, UObject* Outer, FName Key, UObject* CDO, const FString FullName) {
	if (!IsInGameThread()||(Outer==nullptr)||(CDO==nullptr)||(Property==nullptr)) {return;}
	//
	TMap<FName,UObject*>&Map = *Property->ContainerPtrToValuePtr<TMap<FName,UObject*>>(Outer);
	FString SGUID; FGuid GUID; GUID.Invalidate();
	//
	if (FullName.Split(TEXT("_"),nullptr,&SGUID,ESearchCase::IgnoreCase,ESearchDir::FromEnd)) {
		FGuid::Parse(SGUID,GUID);
		//
		if (GUID.IsValid()) {
			for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
				if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
				if (OBJ->HasAnyFlags(RF_BeginDestroyed)) {continue;}
				//
				if (Reflector::FindGUID(*OBJ)==GUID) {
					Map.Emplace(Key,*OBJ); return;
				}///
			}///
		}///
	}///
	//
	FString Name = (SGUID.IsEmpty()) ? CDO->GetName() : CDO->GetName().Replace(TEXT("Default__"),TEXT(""));
	auto OBJ = NewObject<UObject>(Outer,CDO->GetClass(),*Name,RF_NoFlags,CDO);
	//
	Reflector::SetGUID(GUID,OBJ);
	//
	ESaviorResult Result;
	if (Savior){Savior->LoadObject(OBJ,Result);}
	//
	Map.Emplace(Key,OBJ);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior Threading Interface [Level]:

static void OnFinishSerializeLevel(USavior3* Savior, ESaviorResult Result, const bool WriteFile) {
	const auto &Settings = GetDefault<USavior3Settings>();
	check(IsInGameThread());
	//
	if (WriteFile) {
		Savior->WriteSlotToFile(Settings->DefaultPlayerID,Result);
	}///
	//
	Savior->ClearWorkload();
	Savior->EVENT_OnFinishDataSAVE.Broadcast(Result==ESaviorResult::Success);
	//
	if (Savior->PauseGameOnLoad) {
		if (auto PC=Savior->GetWorld()->GetFirstPlayerController()){PC->SetPause(false);}
	}///
}

///	(C) 2021 - Bruno Xavier Leite
static void SerializeLevel(USavior3* Savior, const FName LevelToSave, const bool WriteFile) {
	if (Savior==nullptr||Savior->GetWorld()==nullptr||LevelToSave.IsNone()) {return;}
	const auto &Settings = GetDefault<USavior3Settings>();
	ESaviorResult Result;
	//
	//
	for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
		if (!OBJ->IsValidLowLevelFast()) {continue;}
		if (OBJ->IsA(USavior3::StaticClass())) {continue;}
		if (OBJ->IsA(USaveGame::StaticClass())) {continue;}
		//
		if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
		if (OBJ->GetWorld()!=Savior->GetWorld()) {continue;}
		if (OBJ->HasAnyFlags(RF_ClassDefaultObject|RF_BeginDestroyed)) {continue;}
		if (!OBJ->GetOutermost()->GetName().Contains(LevelToSave.ToString(),ESearchCase::IgnoreCase)) {continue;}
		//
		bool IsTarget = false;
		for (auto ACT : Savior->ActorScope) { if (ACT.Get()&&(OBJ->IsA(ACT))) {IsTarget=true; break;} }
		for (auto OBS : Savior->ObjectScope) { if (OBS.Get()&&(OBJ->IsA(OBS))) {IsTarget=true; break;} }
		for (auto COM : Savior->ComponentScope) { if (COM.Get()&&(OBJ->IsA(COM))) {IsTarget=true; break;} }
		//
		if (IsTarget) {
			if (OBJ->IsA(UActorComponent::StaticClass())) {Savior->SaveComponent(Savior->GetWorld(),CastChecked<UActorComponent>(*OBJ),Result);} else
			if (OBJ->IsA(AActor::StaticClass())) {Savior->SaveActor(Savior->GetWorld(),CastChecked<AActor>(*OBJ),Result);}
			else {Savior->SaveObject((*OBJ),Result);}
		}///
	}///
	//
	//
	Result = ESaviorResult::Success;
	if (IsInGameThread()) {OnFinishSerializeLevel(Savior,Result,WriteFile);} else {
		FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			FSimpleDelegateGraphTask::FDelegate::CreateStatic(&OnFinishSerializeLevel,Savior,Result,WriteFile),
			GET_STATID(STAT_FSimpleDelegateGraphTask_SerializeLevel),
			nullptr, ENamedThreads::GameThread
		);//
	}///
}

static void OnFinishDeserializeLevel(USavior3* Savior, ESaviorResult Result) {
	check(IsInGameThread());
	//
	Savior->ClearWorkload();
	Savior->EVENT_OnFinishDataLOAD.Broadcast(Result==ESaviorResult::Success);
	//
	if (Savior->PauseGameOnLoad) {
		if (auto PC=Savior->GetWorld()->GetFirstPlayerController()){PC->SetPause(false);}
	}///
}

///	(C) 2021 - Bruno Xavier Leite
static void DeserializeLevel(USavior3* Savior, const FName LevelToLoad) {
	if (Savior==nullptr||Savior->GetWorld()==nullptr||LevelToLoad.IsNone()) {return;}
	const auto &Settings = GetDefault<USavior3Settings>();
	ESaviorResult Result;
	//
	//
	for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
		if (!OBJ->IsValidLowLevelFast()) {continue;}
		if (OBJ->IsA(USavior3::StaticClass())) {continue;}
		if (OBJ->IsA(USaveGame::StaticClass())) {continue;}
		//
		if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
		if (OBJ->GetWorld()!=Savior->GetWorld()) {continue;}
		if (OBJ->HasAnyFlags(RF_ClassDefaultObject|RF_BeginDestroyed)) {continue;}
		if (!OBJ->GetOutermost()->GetName().Contains(LevelToLoad.ToString(),ESearchCase::IgnoreCase)) {continue;}
		//
		bool IsTarget = false;
		for (auto ACT : Savior->ActorScope) { if (ACT.Get()&&(OBJ->IsA(ACT))) {IsTarget=true; break;} }
		for (auto OBS : Savior->ObjectScope) { if (OBS.Get()&&(OBJ->IsA(OBS))) {IsTarget=true; break;} }
		for (auto COM : Savior->ComponentScope) { if (COM.Get()&&(OBJ->IsA(COM))) {IsTarget=true; break;} }
		//
		if (IsTarget) {
			if (OBJ->IsA(UActorComponent::StaticClass())) {Savior->LoadComponent(Savior->GetWorld(),CastChecked<UActorComponent>(*OBJ),Result);} else
			if (OBJ->IsA(AActor::StaticClass())) {Savior->LoadActor(Savior->GetWorld(),CastChecked<AActor>(*OBJ),Result);}
			else {Savior->LoadObject((*OBJ),Result);}
		}///
	}///
	//
	//
	Result = ESaviorResult::Success;
	if (IsInGameThread()) {OnFinishDeserializeLevel(Savior,Result);} else {
		FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			FSimpleDelegateGraphTask::FDelegate::CreateStatic(&OnFinishDeserializeLevel,Savior,Result),
			GET_STATID(STAT_FSimpleDelegateGraphTask_DeserializeLevel),
			nullptr, ENamedThreads::GameThread
		);//
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior Threading Interface [Game-Mode]:

static void OnFinishSerializeGameMode(USavior3* Savior, ESaviorResult Result) {
	const auto &Settings = GetDefault<USavior3Settings>();
	//
	Savior->WriteSlotToFile(Settings->DefaultPlayerID,Result);
	//
	Savior->ClearWorkload();
	Savior->EVENT_OnFinishDataSAVE.Broadcast(Result==ESaviorResult::Success);
	//
	if (Savior->PauseGameOnLoad) {
		if (auto PC=Savior->GetWorld()->GetFirstPlayerController()){PC->SetPause(false);}
	}///
}

///	(C) 2021 - Bruno Xavier Leite
static void SerializeGameMode(USavior3* Savior, const bool WithGameInstance) {
	if (Savior==nullptr||Savior->GetWorld()==nullptr) {return;}
	const auto &Settings = GetDefault<USavior3Settings>();
	ESaviorResult Result;
	//
	//
	if (WithGameInstance) {
		const auto &GameInstance = Savior->GetWorld()->GetGameInstance();
		if ((GameInstance!=nullptr)&&GameInstance->IsValidLowLevelFast()) {
			Savior->SaveGameInstanceSingleTon(GameInstance,Result);
		}///
	}///
	//
	const auto &GameMode = Savior->GetWorld()->GetAuthGameMode();
	if ((GameMode!=nullptr)&&(GameMode->IsValidLowLevelFast())) {
		Savior->SaveActor(Savior->GetWorld(),GameMode,Result);
	}///
	//
	const auto &GameState = Savior->GetWorld()->GetGameState();
	if ((GameState!=nullptr)&&GameState->IsValidLowLevelFast()) {
		Savior->SaveActor(Savior->GetWorld(),GameState,Result);
	}///
	//
	const auto &PlayerController = UGameplayStatics::GetPlayerController(Savior,Settings->DefaultPlayerID);
	if ((PlayerController!=nullptr)&&(PlayerController->IsValidLowLevelFast())) {
		const auto &Pawn = PlayerController->GetPawn();
		const auto &PlayerHUD = PlayerController->MyHUD;
		const auto &PlayerState = PlayerController->PlayerState;
		//
		Savior->SaveActor(Savior->GetWorld(),PlayerController,Result);
		//
		if ((Pawn!=nullptr)&&Pawn->IsValidLowLevelFast()) {Savior->SaveActor(Savior->GetWorld(),Pawn,Result);}
		if ((PlayerHUD!=nullptr)&&PlayerHUD->IsValidLowLevelFast()) {Savior->SaveActor(Savior->GetWorld(),PlayerHUD,Result);}
		if ((PlayerState!=nullptr)&&PlayerState->IsValidLowLevelFast()) {Savior->SaveActor(Savior->GetWorld(),PlayerState,Result);}
	}///
	//
	//
	Result = ESaviorResult::Success;
	if (IsInGameThread()) {OnFinishSerializeGameMode(Savior,Result);} else {
		FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			FSimpleDelegateGraphTask::FDelegate::CreateStatic(&OnFinishSerializeGameMode,Savior,Result),
			GET_STATID(STAT_FSimpleDelegateGraphTask_SerializeGameMode), nullptr, ENamedThreads::GameThread
		);//
	}///
}

static void OnFinishDeserializeGameMode(USavior3* Savior, ESaviorResult Result) {
	const auto &Settings = GetDefault<USavior3Settings>();
	//
	Savior->ClearWorkload();
	Savior->EVENT_OnFinishDataLOAD.Broadcast(Result==ESaviorResult::Success);
	//
	const auto &PlayerController = UGameplayStatics::GetPlayerController(Savior,Settings->DefaultPlayerID);
	const auto &Pawn = UGameplayStatics::GetPlayerPawn(Savior,Settings->DefaultPlayerID);
	//
	if ((PlayerController!=nullptr)&&(Pawn!=nullptr)) {
		if (PlayerController->GetRootComponent()) {PlayerController->GetRootComponent()->SetUsingAbsoluteRotation(true);}
		PlayerController->SetControlRotation(Pawn->ActorToWorld().GetRotation().Rotator());
		if (PlayerController->GetRootComponent()) {PlayerController->GetRootComponent()->SetUsingAbsoluteRotation(false);}
	}///
	//
	if (Savior->PauseGameOnLoad) {
		if (auto PC=Savior->GetWorld()->GetFirstPlayerController()){PC->SetPause(false);}
	}///
}

///	(C) 2021 - Bruno Xavier Leite
static void DeserializeGameMode(USavior3* Savior, const bool WithGameInstance) {
	if (Savior==nullptr||Savior->GetWorld()==nullptr) {return;}
	const auto &Settings = GetDefault<USavior3Settings>();
	ESaviorResult Result;
	//
	//
	if (WithGameInstance) {
		const auto &GameInstance = Savior->GetWorld()->GetGameInstance();
		if ((GameInstance!=nullptr)&&GameInstance->IsValidLowLevelFast()) {
			Savior->LoadGameInstanceSingleTon(GameInstance,Result);
		}///
	}///
	//
	const auto &GameMode = Savior->GetWorld()->GetAuthGameMode();
	if ((GameMode!=nullptr)&&(GameMode->IsValidLowLevelFast())) {
		Savior->LoadActor(Savior->GetWorld(),GameMode,Result);
	}///
	//
	const auto &GameState = Savior->GetWorld()->GetGameState();
	if ((GameState!=nullptr)&&GameState->IsValidLowLevelFast()) {
		Savior->LoadActor(Savior->GetWorld(),GameState,Result);
	}///
	//
	const auto &PlayerController = UGameplayStatics::GetPlayerController(Savior,Settings->DefaultPlayerID);
	if ((PlayerController!=nullptr)&&PlayerController->IsValidLowLevelFast()) {
		const auto &Pawn = PlayerController->GetPawn();
		const auto &PlayerHUD = PlayerController->MyHUD;
		const auto &PlayerState = PlayerController->PlayerState;
		//
		Savior->LoadActor(Savior->GetWorld(),PlayerController,Result);
		//
		if (Pawn!=nullptr) {Savior->LoadActor(Savior->GetWorld(),Pawn,Result);}
		if ((PlayerHUD!=nullptr)&&PlayerHUD->IsValidLowLevelFast()) {Savior->LoadActor(Savior->GetWorld(),PlayerHUD,Result);}
		if ((PlayerState!=nullptr)&&PlayerState->IsValidLowLevelFast()) {Savior->LoadActor(Savior->GetWorld(),PlayerState,Result);}
	}///
	//
	//
	Result = ESaviorResult::Success;
	if (IsInGameThread()) {OnFinishDeserializeGameMode(Savior,Result);} else {
		FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			FSimpleDelegateGraphTask::FDelegate::CreateStatic(&OnFinishDeserializeGameMode,Savior,Result),
			GET_STATID(STAT_FSimpleDelegateGraphTask_DeserializeGameMode), nullptr, ENamedThreads::GameThread
		);//
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior Threading Interface [Game-Instance]:

static void OnFinishSerializeGameInstance(USavior3* Savior, ESaviorResult Result) {
	check(IsInGameThread());
	//
	const auto &Settings = GetDefault<USavior3Settings>();
	Savior->WriteSlotToFile(Settings->DefaultPlayerID,Result);
	//
	Savior->ClearWorkload();
	Savior->EVENT_OnFinishDataSAVE.Broadcast(Result==ESaviorResult::Success);
	//
	if (Savior->PauseGameOnLoad) {
		if (auto PC=Savior->GetWorld()->GetFirstPlayerController()){PC->SetPause(false);}
	}///
}

///	(C) 2021 - Bruno Xavier Leite
static void SerializeGameInstance(USavior3* Savior) {
	if (Savior==nullptr||Savior->GetWorld()==nullptr) {return;}
	const auto &Settings = GetDefault<USavior3Settings>();
	ESaviorResult Result = ESaviorResult::Failed;
	//
	//
	const auto &GameInstance = Savior->GetWorld()->GetGameInstance();
	if ((GameInstance!=nullptr)&&GameInstance->IsValidLowLevelFast()) {
		Savior->SaveGameInstanceSingleTon(GameInstance,Result);
	}///
	//
	//
	if (IsInGameThread()) {OnFinishSerializeGameInstance(Savior,Result);} else {
		FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			FSimpleDelegateGraphTask::FDelegate::CreateStatic(&OnFinishSerializeGameInstance,Savior,Result),
			GET_STATID(STAT_FSimpleDelegateGraphTask_SerializeGameInstance),
			nullptr, ENamedThreads::GameThread
		);//
	}///
}

static void OnFinishDeserializeGameInstance(USavior3* Savior, ESaviorResult Result) {
	check(IsInGameThread());
	//
	Savior->ClearWorkload();
	Savior->EVENT_OnFinishDataLOAD.Broadcast(Result==ESaviorResult::Success);
	//
	if (Savior->PauseGameOnLoad) {
		if (auto PC=Savior->GetWorld()->GetFirstPlayerController()){PC->SetPause(false);}
	}///
}

///	(C) 2021 - Bruno Xavier Leite
static void DeserializeGameInstance(USavior3* Savior) {
	if (Savior==nullptr||Savior->GetWorld()==nullptr) {return;}
	const auto &Settings = GetDefault<USavior3Settings>();
	//
	//
	ESaviorResult Result = ESaviorResult::Failed;
	const auto &GameInstance = Savior->GetWorld()->GetGameInstance();
	if ((GameInstance!=nullptr)&&GameInstance->IsValidLowLevelFast()) {
		Savior->LoadGameInstanceSingleTon(GameInstance,Result);
	}///
	//
	//
	if (IsInGameThread()) {OnFinishDeserializeGameInstance(Savior,Result);} else {
		FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			FSimpleDelegateGraphTask::FDelegate::CreateStatic(&OnFinishDeserializeGameInstance,Savior,Result),
			GET_STATID(STAT_FSimpleDelegateGraphTask_DeserializeGameInstance),
			nullptr, ENamedThreads::GameThread
		);//
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior Threading Interface [Entities]:

///	(C) 2021 - Bruno Xavier Leite
static void DeserializeActor(const FSaviorRecord Record, AActor* Actor) {
	const auto &Scene = Cast<USceneComponent>(Actor->GetRootComponent());
	const auto &Capsule = Cast<UCapsuleComponent>(Actor->GetRootComponent());
	//
	FSoftObjectPath MeshPth = FSoftObjectPath(Record.ActorMesh);
	//
	if (const auto &Avatar = Cast<ACharacter>(Actor)) {
		if (const auto &Skel = Avatar->GetMesh()) {
			auto MeshPtr = TSoftObjectPtr<USkeletalMesh>(MeshPth);
			if (auto*OBJ=MeshPtr.Get()) {Skel->SetSkeletalMesh(OBJ);}
		}///
	} else if (const auto &Mesh = Cast<UStaticMeshComponent>(Actor->GetRootComponent())) {
		auto MeshPtr = TSoftObjectPtr<UStaticMesh>(MeshPth);
		if (auto*OBJ=MeshPtr.Get()) {Mesh->SetStaticMesh(OBJ);}
	}///
	//
	if (!TAG_ANOTRAN(Actor) && !Record.IgnoreTransform && ((Scene && Scene->Mobility==EComponentMobility::Movable)||(Capsule && Capsule->Mobility==EComponentMobility::Movable))) {
		UWorld* World = Actor->GetWorld();
		AActor* Owner = nullptr;
		//
		for (TObjectIterator<UChildActorComponent>CACT; CACT; ++CACT) {
			if (CACT->GetChildActor()==Actor) {return;}
		}///
		//
		for (TActorIterator<AActor>PACT(World); PACT; ++PACT) {
			if (Reflector::MakeActorID(*PACT).ToString()==Record.OuterName) {Owner=(*PACT); break;}
		}///
		//
		if (auto*Root=Actor->GetRootComponent()) {
			if (Owner) {Root->SetupAttachment(Owner->GetRootComponent());}
		} Actor->SetActorRelativeLocation(Record.Location);
		//
		Actor->SetActorRelativeScale3D(Record.Scale);
		Actor->SetActorRelativeRotation(Record.Rotation);
	}///
	//
	if (!TAG_ANOPHYS(Actor) && Scene && (Scene->Mobility==EComponentMobility::Movable)) {
		if (auto Primitive=Cast<UPrimitiveComponent>(Actor->GetRootComponent())) {
			Primitive->SetPhysicsAngularVelocityInDegrees(Record.AngularVelocity);
		} Scene->ComponentVelocity = Record.LinearVelocity;
	}///
	//
	if (!TAG_ANOHIDE(Actor)) {Actor->SetActorHiddenInGame(Record.HiddenInGame);}
}

///	(C) 2021 - Bruno Xavier Leite
static void DeserializeComponent(const FSaviorRecord Record, UActorComponent* Component) {
	if (Component->IsA(UChildActorComponent::StaticClass())) {return;}
	//
	const auto &Scene = Cast<USceneComponent>(Component);
	if (!TAG_CNOTRAN(Component) && (Scene && Scene->Mobility==EComponentMobility::Movable)) {
		Scene->SetRelativeRotation(Record.Rotation,false,nullptr,ETeleportType::TeleportPhysics);
		Scene->SetRelativeLocation(Record.Location,false,nullptr,ETeleportType::TeleportPhysics);
		Scene->SetRelativeScale3D(Record.Scale);
	}///
	//
	if (Component->IsA(UParticleSystemComponent::StaticClass())) {
		const auto &Particle = CastChecked<UParticleSystemComponent>(Component);
		if (Record.Active) {Particle->ActivateSystem();}
		else {Particle->DeactivateSystem();}
	}///
	//
	if (Component->IsA(ULightComponent::StaticClass())) {
		auto Light = CastChecked<ULightComponent>(Component);
		if (Light->Mobility!=EComponentMobility::Movable) {return;}
		//
		UWorld* World=Light->GetWorld();
		if (World && (World->Scene)) {
			World->Scene->UpdateLightTransform(Light);
			World->Scene->UpdateLightColorAndBrightness(Light);
		}///
	}///
	//
	Component->SetActive(Record.Active);
}

///	(C) 2021 - Bruno Xavier Leite
static void DeserializeActor(const FSaviorMinimal Record, AActor* Actor) {
	const auto &Scene = Cast<USceneComponent>(Actor->GetRootComponent());
	const auto &Capsule = Cast<UCapsuleComponent>(Actor->GetRootComponent());
	//
	if (!TAG_ANOTRAN(Actor) && ((Scene && Scene->Mobility==EComponentMobility::Movable)||(Capsule && Capsule->Mobility==EComponentMobility::Movable))) {
		Actor->SetActorRelativeLocation(Record.Location,false,nullptr,ETeleportType::TeleportPhysics);
		Actor->SetActorRelativeRotation(Record.Rotation,false,nullptr,ETeleportType::TeleportPhysics);
	}///
}

///	(C) 2021 - Bruno Xavier Leite
static void DeserializeComponent(const FSaviorMinimal Record, UActorComponent* Component) {
	const auto &Scene = Cast<USceneComponent>(Component);
	//
	if (!TAG_CNOTRAN(Component) && (Scene && Scene->Mobility==EComponentMobility::Movable)) {
		Scene->SetRelativeRotation(Record.Rotation,false,nullptr,ETeleportType::TeleportPhysics);
		Scene->SetRelativeLocation(Record.Location,false,nullptr,ETeleportType::TeleportPhysics);
	}///
}

///	(C) 2021 - Bruno Xavier Leite
static void SerializeActorMaterials(USavior3* Savior, AActor* Actor) {
	ESaviorResult Result; Savior->SaveActorMaterials(Actor->GetWorld(),Actor,Result);
}

///	(C) 2021 - Bruno Xavier Leite
static void DeserializeActorMaterials(USavior3* Savior, AActor* Actor) {
	ESaviorResult Result; Savior->LoadActorMaterials(Actor->GetWorld(),Actor,Result);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior Threading Task Interface:

class TASK_SerializeGameWorld : public FNonAbandonableTask {
private:
	USavior3* Savior;
public:
	TASK_SerializeGameWorld(USavior3* Savior) {this->Savior=Savior;}
	//
	FORCEINLINE TStatId GetStatId() const {
		RETURN_QUICK_DECLARE_CYCLE_STAT(TASK_SerializeGameWorld,STATGROUP_ThreadPoolAsyncTasks);
	}///
	//
	void DoWork() {SerializeGameWorld(Savior);}
};

class TASK_DeserializeGameWorld : public FNonAbandonableTask {
private:
	USavior3* Savior;
public:
	TASK_DeserializeGameWorld(USavior3* Savior) {this->Savior=Savior;}
	//
	FORCEINLINE TStatId GetStatId() const {
		RETURN_QUICK_DECLARE_CYCLE_STAT(TASK_DeserializeGameWorld,STATGROUP_ThreadPoolAsyncTasks);
	}///
	//
	void DoWork() {DeserializeGameWorld(Savior);}
};

class TASK_SerializeLevel : public FNonAbandonableTask {
private:
	bool WriteSlotFile;
	USavior3* Savior;
	FName Level;
public:
	TASK_SerializeLevel(USavior3* Savior, const FName LevelToSave, const bool WriteFile) {
		this->WriteSlotFile = WriteFile;
		this->Level = LevelToSave;
		this->Savior = Savior;
	}///
	//
	FORCEINLINE TStatId GetStatId() const {
		RETURN_QUICK_DECLARE_CYCLE_STAT(TASK_SerializeLevel,STATGROUP_ThreadPoolAsyncTasks);
	}///
	//
	void DoWork() {SerializeLevel(Savior,Level,WriteSlotFile);}
};

class TASK_DeserializeLevel : public FNonAbandonableTask {
private:
	USavior3* Savior;
	FName Level;
public:
	TASK_DeserializeLevel(USavior3* Savior, const FName LevelToLoad) {
		this->Level = LevelToLoad;
		this->Savior = Savior;
	}///
	//
	FORCEINLINE TStatId GetStatId() const {
		RETURN_QUICK_DECLARE_CYCLE_STAT(TASK_DeserializeLevel,STATGROUP_ThreadPoolAsyncTasks);
	}///
	//
	void DoWork() {DeserializeLevel(Savior,Level);}
};

class TASK_SerializeGameMode : public FNonAbandonableTask {
private:
	USavior3* Savior;
	bool WithGameInstance;
public:
	TASK_SerializeGameMode(USavior3* Savior, bool WithGameInstance) {
		this->WithGameInstance = WithGameInstance;
		this->Savior = Savior;
	}///
	//
	FORCEINLINE TStatId GetStatId() const {
		RETURN_QUICK_DECLARE_CYCLE_STAT(TASK_SerializeGameMode,STATGROUP_ThreadPoolAsyncTasks);
	}///
	//
	void DoWork() {SerializeGameMode(Savior,WithGameInstance);}
};

class TASK_DeserializeGameMode : public FNonAbandonableTask {
private:
	USavior3* Savior;
	bool WithGameInstance;
public:
	TASK_DeserializeGameMode(USavior3* Savior, bool WithGameInstance) {
		this->WithGameInstance = WithGameInstance;
		this->Savior = Savior;
	}///
	//
	FORCEINLINE TStatId GetStatId() const {
		RETURN_QUICK_DECLARE_CYCLE_STAT(TASK_DeserializeGameMode,STATGROUP_ThreadPoolAsyncTasks);
	}///
	//
	void DoWork() {DeserializeGameMode(Savior,WithGameInstance);}
};

class TASK_SerializeGameInstance : public FNonAbandonableTask {
private:
	USavior3* Savior;
public:
	TASK_SerializeGameInstance(USavior3* Savior) {this->Savior=Savior;}
	//
	FORCEINLINE TStatId GetStatId() const {
		RETURN_QUICK_DECLARE_CYCLE_STAT(TASK_SerializeGameInstance,STATGROUP_ThreadPoolAsyncTasks);
	}///
	//
	void DoWork() {SerializeGameInstance(Savior);}
};

class TASK_DeserializeGameInstance : public FNonAbandonableTask {
private:
	USavior3* Savior;
public:
	TASK_DeserializeGameInstance(USavior3* Savior) {this->Savior=Savior;}
	//
	FORCEINLINE TStatId GetStatId() const {
		RETURN_QUICK_DECLARE_CYCLE_STAT(TASK_DeserializeGameInstance,STATGROUP_ThreadPoolAsyncTasks);
	}///
	//
	void DoWork() {DeserializeGameInstance(Savior);}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////