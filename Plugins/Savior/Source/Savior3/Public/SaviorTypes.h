//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//			Copyright 2020 (C) Bruno Xavier Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Savior3_Shared.h"

#include "Runtime/Core/Public/Serialization/ArchiveSaveCompressedProxy.h"
#include "Runtime/Core/Public/Serialization/ArchiveLoadCompressedProxy.h"

#include "Runtime/CoreUObject/Public/UObject/SoftObjectPtr.h"

#include "Runtime/Engine/Classes/Materials/Material.h"
#include "Runtime/Engine/Classes/Materials/MaterialExpression.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"

#include "SaviorTypes.generated.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class USavior3;
class UMetaSlot;
class SaviorReflector;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior Internal Types:

enum class ESeverity : uint8 {
	Fatal			= 0,
	Error			= 1,
	Warning			= 2,
	Info			= 3
};

UENUM(BlueprintType, meta=(DisplayName="[SAVIOR] Runtime Mode",BlueprintInternalUseOnly=true))
enum class ERuntimeMode : uint8 {
	/// Executes Serialization within Game Thread.
	SynchronousTask,
	/// Executes Serialization from its own Asynchronous Threaded.
	BackgroundTask
};

UENUM(BlueprintType,meta=(DisplayName="[SAVIOR] Thread Safety",BlueprintInternalUseOnly=true))
enum class EThreadSafety : uint8 {
	/// Is Safe to Bind any Interface Calls.
	IsCurrentlyThreadSafe,
	/// Is about to perform runtime expensive operations.
	IsPreparingToSaveOrLoad,
	/// Is Unsafe Loading. Any interaction with Main Game's while loading is dangerous!
	AsynchronousLoading,
	/// Is Unsafe Saving. Any interaction with Main Game's while saving is dangerous!
	AsynchronousSaving,
	/// Is Safely Loading. Any interaction with Main Game's Thread is safe!
	SynchronousLoading,
	/// Is Safely Saving. Any interaction with Main Game's Thread is safe!
	SynchronousSaving
};

UENUM(BlueprintType,meta=(DisplayName="[SAVIOR] Serializer Type"))
enum class ERecordType : uint8 {
	Complex,
	Minimal
};

UENUM(BlueprintType,meta=(DisplayName="[SAVIOR] Serializer Result"))
enum class ESaviorResult : uint8 {
	Success,
	Failed
};

UENUM(meta=(BlueprintInternalUseOnly=true))
enum class EStructType : uint8 {
	Struct,
	Transform,
	Vector2D,
	Vector3D,
	Rotator,
	LColor,
	FColor,
	Curve
};

UENUM(BlueprintType,meta=(DisplayName="[SAVIOR] Load-Screen Mode",BlueprintInternalUseOnly=true))
enum class ELoadScreenMode : uint8 {
	BackgroundBlur,
	SplashScreen,
	MoviePlayer,
	NoLoadScreen
};

UENUM(BlueprintType,meta=(DisplayName="[SAVIOR] Load-Screen Trigger",BlueprintInternalUseOnly=true))
enum class ELoadScreenTrigger : uint8 {
	AllScreens,
	WhileSaving,
	WhileLoading
};

UENUM(BlueprintType,meta=(DisplayName="[SAVIOR] SGUID Generation Mode",BlueprintInternalUseOnly=true))
enum class EGuidGeneratorMode : uint8 {
	StaticNameToGUID,
	ResolvedNameToGUID,
	MemoryAddressToGUID
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

USTRUCT()
struct FMatScalarRecord {
	GENERATED_USTRUCT_BODY()
	//
	UPROPERTY() FName ParamName;
	UPROPERTY() float ParamValue;
	//
	//
	FMatScalarRecord()
		: ParamName(NAME_None)
		, ParamValue(0)
	{}///
};

USTRUCT()
struct FMatVectorRecord {
	GENERATED_USTRUCT_BODY()
	//
	UPROPERTY() FName ParamName;
	UPROPERTY() FLinearColor ParamValue;
	//
	//
	FMatVectorRecord()
		: ParamName(NAME_None)
		, ParamValue(FLinearColor::Black)
	{}///
};

USTRUCT()
struct FMatTextureRecord {
	GENERATED_USTRUCT_BODY()
	//
	//
	UPROPERTY() FName ParamName;
	UPROPERTY() FSoftObjectPath ParamValue;
	//
	//
	FMatTextureRecord()
		: ParamName(NAME_None)
		, ParamValue(FSoftObjectPath{})
	{}///
};

USTRUCT()
struct FMaterialRecord {
	GENERATED_USTRUCT_BODY()
	//
	UPROPERTY() TMap<FGuid,FMatScalarRecord>ScalarParams;
	UPROPERTY() TMap<FGuid,FMatVectorRecord>VectorParams;
	UPROPERTY() TMap<FGuid,FMatTextureRecord>TextureParams;
	//
	//
	FMaterialRecord()
		: ScalarParams(TMap<FGuid,FMatScalarRecord>{})
		, VectorParams(TMap<FGuid,FMatVectorRecord>{})
		, TextureParams(TMap<FGuid,FMatTextureRecord>{})
	{}///
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType,meta=(BlueprintInternalUseOnly=true))
struct SAVIOR3_API FSaviorRecord {
	GENERATED_USTRUCT_BODY()
	//
	//
	UPROPERTY() FGuid GUID;
	//
	UPROPERTY() FString Buffer;
	//
	UPROPERTY() FString FullName;
	UPROPERTY() FString OuterName;
	UPROPERTY() FString ClassPath;
	//
	UPROPERTY() FString ActorMesh;
	UPROPERTY() TMap<FName,FMaterialRecord>Materials;
	//
	UPROPERTY() FVector Scale;
	UPROPERTY() FVector Location;
	UPROPERTY() FRotator Rotation;
	//
	UPROPERTY() FVector LinearVelocity;
	UPROPERTY() FVector AngularVelocity;
	//
	UPROPERTY() bool Active;
	UPROPERTY() bool Destroyed;
	UPROPERTY() bool HiddenInGame;
	UPROPERTY(Transient) bool IgnoreTransform;
	//
	//
	FSaviorRecord()
		: GUID(FGuid{})
		//
		, Buffer(TEXT(""))
		//
		, FullName(TEXT(""))
		, OuterName(TEXT(""))
		, ClassPath(TEXT(""))
		//
		, ActorMesh(TEXT(""))
		, Materials(TMap<FName,FMaterialRecord>{})
		//
		, Scale(FVector::ZeroVector)
		, Location(FVector::ZeroVector)
		, Rotation(FRotator::ZeroRotator)
		//
		, LinearVelocity(FVector::ZeroVector)
		, AngularVelocity(FVector::ZeroVector)
		//
		, Active(true)
		, Destroyed(false)
		, HiddenInGame(false)
		, IgnoreTransform(false)
	{}///
	//
	//
	inline FSaviorRecord &operator = (const FSaviorRecord &REC) {
		GUID = REC.GUID;
		Scale = REC.Scale;
		Buffer = REC.Buffer;
		FullName = REC.FullName;
		Location = REC.Location;
		Rotation = REC.Rotation;
		ActorMesh = REC.ActorMesh;
		Materials = REC.Materials;
		ClassPath = REC.ClassPath;
		Destroyed = REC.Destroyed;
		HiddenInGame = REC.HiddenInGame;
		LinearVelocity = REC.LinearVelocity;
		AngularVelocity = REC.AngularVelocity;
	return *this;}
	//
	inline bool operator == (const FSaviorRecord &REC) const {
		return (
			(GUID==REC.GUID) &&
			(Scale==REC.Scale) &&
			(Buffer==REC.Buffer) &&
			(FullName==REC.FullName) &&
			(Location==REC.Location) &&
			(Rotation==REC.Rotation) &&
			(ActorMesh==REC.ActorMesh) &&
			(ClassPath==REC.ClassPath) &&
			(Destroyed==REC.Destroyed) &&
			(HiddenInGame==REC.HiddenInGame) &&
			(LinearVelocity==REC.LinearVelocity) &&
			(AngularVelocity==REC.AngularVelocity) &&
			(Materials.Num()==REC.Materials.Num())
		);///
	}///
	//
	//
	friend inline uint32 GetTypeHash(const FSaviorRecord &REC) {
		return FCrc::MemCrc32(&REC,sizeof(FSaviorRecord));
	}///
};

USTRUCT(BlueprintType,meta=(BlueprintInternalUseOnly=true))
struct SAVIOR3_API FSaviorMinimal {
	GENERATED_USTRUCT_BODY()
	//
	//
	UPROPERTY() FString Buffer;
	UPROPERTY() FVector Location;
	UPROPERTY() FRotator Rotation;
	//
	UPROPERTY() bool Destroyed;
	//
	//
	inline FSaviorMinimal &operator = (const FSaviorMinimal &REC) {
		Buffer = REC.Buffer;
		Location = REC.Location;
		Rotation = REC.Rotation;
		Destroyed = REC.Destroyed;
	return *this;}
	//
	friend inline uint32 GetTypeHash(const FSaviorMinimal &REC) {
		return FCrc::MemCrc32(&REC,sizeof(FSaviorMinimal));
	}///
	//
	//
	FSaviorMinimal()
		: Buffer(TEXT(""))
		, Location(FVector::ZeroVector)
		, Rotation(FRotator::ZeroRotator)
		, Destroyed(false)
	{}///
};

USTRUCT(meta=(BlueprintInternalUseOnly=true))
struct SAVIOR3_API FSaviorRedirector {
	GENERATED_USTRUCT_BODY()
	//
	UPROPERTY(Category="Versioning", EditAnywhere)
	TMap<FName,FName>PropertyRedirect;
	//
	//
	inline FSaviorRedirector &operator = (const FSaviorRedirector &RED) {
		PropertyRedirect = RED.PropertyRedirect;
	return *this;}
	//
	friend inline uint32 GetTypeHash(const FSaviorRedirector &RED) {
		return FCrc::MemCrc32(&RED,sizeof(FSaviorRedirector));
	}///
	//
	//
	FSaviorRedirector()
		: PropertyRedirect(TMap<FName,FName>{})
	{}///
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline FArchive &operator << (FArchive &AR, FSaviorRecord &SR) {
	AR << SR.GUID;
	AR << SR.Scale;
	AR << SR.Buffer;
	AR << SR.FullName;
	AR << SR.Location;
	AR << SR.Rotation;
	AR << SR.ActorMesh;
	AR << SR.Materials;
	AR << SR.ClassPath;
	AR << SR.Destroyed;
	AR << SR.HiddenInGame;
	AR << SR.LinearVelocity;
	AR << SR.AngularVelocity;
	//
	return AR;
}

inline FArchive &operator << (FArchive &AR, FSaviorMinimal &SR) {
	AR << SR.Buffer;
	AR << SR.Location;
	AR << SR.Rotation;
	AR << SR.Destroyed;
	//
	return AR;
}

inline FArchive &operator << (FArchive &AR, FSaviorRedirector &SR) {
	AR << SR.PropertyRedirect;
	//
	return AR;
}

inline FArchive &operator << (FArchive &AR, FMaterialRecord &MR) {
	AR << MR.ScalarParams;
	AR << MR.VectorParams;
	AR << MR.TextureParams;
	//
	return AR;
}

inline FArchive &operator << (FArchive &AR, FMatScalarRecord &MR) {
	AR << MR.ParamName;
	AR << MR.ParamValue;
	//
	return AR;
}

inline FArchive &operator << (FArchive &AR, FMatVectorRecord &MR) {
	AR << MR.ParamName;
	AR << MR.ParamValue;
	//
	return AR;
}

inline FArchive &operator << (FArchive &AR, FMatTextureRecord &MR) {
	AR << MR.ParamName;
	AR << MR.ParamValue;
	//
	return AR;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////