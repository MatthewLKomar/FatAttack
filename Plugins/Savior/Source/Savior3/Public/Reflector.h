//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//			Copyright 2020 (C) Bruno Xavier Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SaviorTypes.h"
#include "Savior3_Shared.h"

#include "SaviorMetaData.h"
#include "ISAVIOR_Serializable.h"

#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Core/Public/Async/AsyncWork.h"
#include "Runtime/Engine/Classes/Engine/Player.h"
#include "Runtime/Engine/Classes/Curves/CurveFloat.h"
#include "Runtime/GameplayTags/Public/GameplayTags.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "Runtime/CoreUObject/Public/UObject/UnrealType.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerState.h"
#include "Runtime/Engine/Classes/Components/ActorComponent.h"
#include "Runtime/CoreUObject/Public/UObject/SoftObjectPtr.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"

#include "Runtime/Core/Public/UObject/NameTypes.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DECLARE_LOG_CATEGORY_EXTERN(SaviorLog,Log,All);

SAVIOR3_API void LOG_SV(const bool Debug, const ESeverity Severity, const FName Message);
SAVIOR3_API void LOG_SV(const bool Debug, const ESeverity Severity, const TCHAR* Message);
SAVIOR3_API void LOG_SV(const bool Debug, const ESeverity Severity, const FString Message);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Reflector Utilities:

static inline const FString SanitizePropertyID(const FString PID) {
	auto IX = PID.Find(TEXT("_"),ESearchCase::IgnoreCase,ESearchDir::FromEnd);
	//
	return PID.Left(IX);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Unreal Property Reflection System:

class SAVIOR3_API Reflector {
public:
	static TSharedPtr<FJsonValue>UPropertyToJSON(FStrProperty* Property, const UObject* Container);
	static TSharedPtr<FJsonValue>UPropertyToJSON(FIntProperty* Property, const UObject* Container);
	static TSharedPtr<FJsonValue>UPropertyToJSON(FByteProperty* Property, const UObject* Container);
	static TSharedPtr<FJsonValue>UPropertyToJSON(FEnumProperty* Property, const UObject* Container);
	static TSharedPtr<FJsonValue>UPropertyToJSON(FNameProperty* Property, const UObject* Container);
	static TSharedPtr<FJsonValue>UPropertyToJSON(FTextProperty* Property, const UObject* Container);
	static TSharedPtr<FJsonValue>UPropertyToJSON(FBoolProperty* Property, const UObject* Container);
	static TSharedPtr<FJsonObject>UPropertyToJSON(FMapProperty* Property, const UObject* Container);
	static TSharedPtr<FJsonValue>UPropertyToJSON(FFloatProperty* Property, const UObject* Container);
	static TSharedPtr<FJsonValue>UPropertyToJSON(FInt64Property* Property, const UObject* Container);
	static TSharedPtr<FJsonObject>UPropertyToJSON(FClassProperty* Property, const UObject* Container);
	static TSharedPtr<FJsonObject>UPropertyToJSON(FObjectProperty* Property, const UObject* Container);
	static TArray<TSharedPtr<FJsonValue>>UPropertyToJSON(FSetProperty* Property, const UObject* Container);
	static TArray<TSharedPtr<FJsonValue>>UPropertyToJSON(FArrayProperty* Property, const UObject* Container);
	static TSharedPtr<FJsonObject>UPropertyToJSON(FStructProperty* Property, const UObject* Container, const EStructType StructType);
public:
	static void JSONToFProperty(TSharedPtr<FJsonObject>&JSON, const FString &PID, FIntProperty* Property, UObject* Container);
	static void JSONToFProperty(TSharedPtr<FJsonObject>&JSON, const FString &PID, FStrProperty* Property, UObject* Container);
	static void JSONToFProperty(TSharedPtr<FJsonObject>&JSON, const FString &PID, FByteProperty* Property, UObject* Container);
	static void JSONToFProperty(TSharedPtr<FJsonObject>&JSON, const FString &PID, FEnumProperty* Property, UObject* Container);
	static void JSONToFProperty(TSharedPtr<FJsonObject>&JSON, const FString &PID, FNameProperty* Property, UObject* Container);
	static void JSONToFProperty(TSharedPtr<FJsonObject>&JSON, const FString &PID, FTextProperty* Property, UObject* Container);
	static void JSONToFProperty(TSharedPtr<FJsonObject>&JSON, const FString &PID, FBoolProperty* Property, UObject* Container);
	static void JSONToFProperty(TSharedPtr<FJsonObject>&JSON, const FString &PID, FFloatProperty* Property, UObject* Container);
	static void JSONToFProperty(TSharedPtr<FJsonObject>&JSON, const FString &PID, FInt64Property* Property, UObject* Container);
	static void JSONToFProperty(TSharedPtr<FJsonObject>&JSON, const FString &PID, FStructProperty* Property, UObject* Container, const EStructType StructType);
	//
	static void JSONToFProperty(USavior3* Savior, TSharedPtr<FJsonObject>&JSON, const FString &PID, FMapProperty* Property, UObject* Container);
	static void JSONToFProperty(USavior3* Savior, TSharedPtr<FJsonObject>&JSON, const FString &PID, FSetProperty* Property, UObject* Container);
	static void JSONToFProperty(USavior3* Savior, TSharedPtr<FJsonObject>&JSON, const FString &PID, FArrayProperty* Property, UObject* Container);
	static void JSONToFProperty(USavior3* Savior, TSharedPtr<FJsonObject>&JSON, const FString &PID, FClassProperty* Property, UObject* Container);
	static void JSONToFProperty(USavior3* Savior, TSharedPtr<FJsonObject>&JSON, const FString &PID, FObjectProperty* Property, UObject* Container);
public:
	static TSharedPtr<FJsonObject> ParseFStructToJSON(FStructProperty* Property, const void* StructPtr, const UObject* Container);
	static void ParseFStructToJSON(TSharedPtr<FJsonObject> &JSON, FStructProperty* Property, const void* StructPtr, const UObject* Container);
	static void ParseFStructPropertyToJSON(TSharedPtr<FJsonObject> &JSON, FProperty* Property, const void* ValuePtr, const UObject* Container, int64 CheckFlags, int64 SkipFlags, const bool IsNative);
	//
	static void ParseJSONtoFStruct(TSharedPtr<FJsonObject> &JSON, FStructProperty* Property, void* StructPtr, UObject* Container);
	static void ParseJSONtoFStructProperty(TSharedPtr<FJsonObject> &JSON, FProperty* Property, void* ValuePtr, UObject* Container, int64 CheckFlags, int64 SkipFlags, const bool IsNative);
public:
	static void RespawnActorFromData(UWorld* World, const TSet<TSubclassOf<UObject>>RespawnScope, const FSaviorRecord Data);
	static void RespawnComponentFromData(UWorld* World, const TSet<TSubclassOf<UObject>>RespawnScope, const FSaviorRecord Data);
public:
	static FMaterialRecord CaptureMaterialSnapshot(UMaterialInstanceDynamic* Instance);
public:
	static const FName MakeActorID(AActor* Actor, bool FullPath=false);
	static const FName MakeObjectID(UObject* OBJ, bool FullPath=false);
	static const FName MakeComponentID(UActorComponent* CMP, bool FullPath=false);
	//
	static const FGuid FindGUID(UObject* Container);
	static const FGuid FindGUID(const UObject* Container);
	//
	static const FString FindSGUID(UObject* Container);
	static const FString FindSGUID(const UObject* Container);
	static const FGuid ParseGUID(FStructProperty* Property, const UObject* Container);
	static const FString ParseSGUID(FStructProperty* Property, const UObject* Container);
	//
	static void SetGUID(const FGuid &GUID, UObject* Container);
public:
	static bool HasGUID(UObject* Container);
	static bool IsMatchingSGUID(const FGuid &SGUID, UObject* Target);
	static bool IsMatchingSGUID(UObject* Target, UObject* ComparedTo);
	//
	static bool IsSafeToRespawnInstance(const UObject* CDO);
	static bool IsSupportedProperty(const FProperty* Property);
	static bool RestoreMaterialFromSnapshot(UMaterialInstanceDynamic* Instance, const FSaviorRecord &Data);
	static bool GetPlayerNetworkID(const APlayerController* PlayerController, FString &ID, const bool AppendPort);
public:
	static inline const FString SanitizeObjectPath(const FString &InPath);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////