//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//			Copyright 2020 (C) Bruno Xavier Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SaviorTypes.h"
#include "Runtime/Engine/Classes/GameFramework/SaveGame.h"

#include "SaviorMetaData.generated.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UCLASS(ClassGroup=Synaptech, Category="Synaptech", Blueprintable, BlueprintType, meta=(DisplayName="[SAVIOR] Slot Meta"))
class SAVIOR3_API UMetaSlot : public USaveGame {
	GENERATED_BODY()
public:
	UPROPERTY(Category="Meta Data", EditDefaultsOnly, BlueprintReadWrite) float Progress;
	UPROPERTY(Category="Meta Data", EditDefaultsOnly, BlueprintReadWrite) int32 PlayTime;
	UPROPERTY(Category="Meta Data", EditDefaultsOnly, BlueprintReadWrite) FString Chapter;
	UPROPERTY(Category="Meta Data", EditDefaultsOnly, BlueprintReadWrite) int32 PlayerLevel;
	UPROPERTY(Category="Meta Data", EditDefaultsOnly, BlueprintReadWrite) FString PlayerName;
	UPROPERTY(Category="Meta Data", EditDefaultsOnly, BlueprintReadWrite) FString SaveLocation;
	UPROPERTY(Category="Meta Data", VisibleDefaultsOnly, BlueprintReadWrite) FDateTime SaveDate;
public:
	UMetaSlot()
		: Progress(0.f)
		, PlayTime(0)
		, Chapter(TEXT(""))
		, PlayerLevel(0)
		, PlayerName(TEXT(""))
		, SaveLocation(TEXT(""))
		, SaveDate(FDateTime::Now())
	{}///
public:
	friend inline FArchive &operator << (FArchive &AR, UMetaSlot &SD) {
		AR << SD.Chapter;
		AR << SD.Progress;
		AR << SD.PlayTime;
		AR << SD.SaveDate;
		AR << SD.PlayerName;
		AR << SD.PlayerLevel;
		AR << SD.SaveLocation;
	return AR;}
};

UCLASS(ClassGroup=Synaptech, Category="Synaptech", Blueprintable, BlueprintType, meta=(DisplayName="[SAVIOR] Slot Data"))
class SAVIOR3_API UDataSlot : public USaveGame {
	GENERATED_BODY()
public:
	UPROPERTY(Category="Data",VisibleAnywhere) TMap<FName,FSaviorMinimal>Minimal;
	UPROPERTY(Category="Data",VisibleAnywhere) TMap<FName,FSaviorRecord>Complex;
public:
	UDataSlot()
		: Minimal(TMap<FName,FSaviorMinimal>{})
		, Complex(TMap<FName,FSaviorRecord>{})
	{}///
public:
	friend inline FArchive &operator << (FArchive &AR, UDataSlot &SD) {
		AR << SD.Minimal;
		AR << SD.Complex;
	return AR;}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType,meta=(DisplayName="Slot Meta"))
struct SAVIOR3_API FSlotMeta {
	GENERATED_USTRUCT_BODY()
	//
	UPROPERTY(Category="Meta Data", BlueprintReadOnly) float Progress;
	UPROPERTY(Category="Meta Data", BlueprintReadOnly) int32 PlayTime;
	UPROPERTY(Category="Meta Data", BlueprintReadOnly) FString Chapter;
	UPROPERTY(Category="Meta Data", BlueprintReadOnly) int32 PlayerLevel;
	UPROPERTY(Category="Meta Data", BlueprintReadOnly) FString PlayerName;
	UPROPERTY(Category="Meta Data", BlueprintReadOnly) FDateTime SaveDate;
	UPROPERTY(Category="Meta Data", BlueprintReadOnly) FString SaveLocation;
	//
	//
	FSlotMeta()
		: Progress(0.f)
		, PlayTime(0)
		, Chapter(TEXT(""))
		, PlayerLevel(0)
		, PlayerName(TEXT(""))
		, SaveDate(FDateTime::Now())
		, SaveLocation(TEXT(""))
	{}///
	//
	FSlotMeta(UMetaSlot* MS)
		: Progress(0.f)
		, PlayTime(0)
		, Chapter(TEXT(""))
		, PlayerLevel(0)
		, PlayerName(TEXT(""))
		, SaveDate(FDateTime::Now())
		, SaveLocation(TEXT(""))
	{///
		if (MS->IsValidLowLevelFast()) {
			Chapter = MS->Chapter;
			Progress = MS->Progress;
			PlayTime = MS->PlayTime;
			SaveDate = MS->SaveDate;
			PlayerName = MS->PlayerName;
			PlayerLevel = MS->PlayerLevel;
			SaveLocation = MS->SaveLocation;
		}///
	}///
	//
	FSlotMeta(const UMetaSlot* MS)
		: Progress(0.f)
		, PlayTime(0)
		, Chapter(TEXT(""))
		, PlayerLevel(0)
		, PlayerName(TEXT(""))
		, SaveDate(FDateTime::Now())
		, SaveLocation(TEXT(""))
	{///
		if (MS->IsValidLowLevelFast()) {
			Chapter = MS->Chapter;
			Progress = MS->Progress;
			PlayTime = MS->PlayTime;
			SaveDate = MS->SaveDate;
			PlayerName = MS->PlayerName;
			PlayerLevel = MS->PlayerLevel;
			SaveLocation = MS->SaveLocation;
		}///
	}///
	//
	//
	FORCEINLINE FSlotMeta &operator = (const FSlotMeta &MS) {
		Chapter = MS.Chapter;
		Progress = MS.Progress;
		PlayTime = MS.PlayTime;
		SaveDate = MS.SaveDate;
		PlayerName = MS.PlayerName;
		PlayerLevel = MS.PlayerLevel;
		SaveLocation = MS.SaveLocation;
	return *this;}
	//
	FORCEINLINE FSlotMeta &operator = (UMetaSlot* MS) {
		Chapter = MS->Chapter;
		Progress = MS->Progress;
		PlayTime = MS->PlayTime;
		SaveDate = MS->SaveDate;
		PlayerName = MS->PlayerName;
		PlayerLevel = MS->PlayerLevel;
		SaveLocation = MS->SaveLocation;
	return *this;}
	//
	FORCEINLINE FSlotMeta &operator = (const UMetaSlot* MS) {
		Chapter = MS->Chapter;
		Progress = MS->Progress;
		PlayTime = MS->PlayTime;
		SaveDate = MS->SaveDate;
		PlayerName = MS->PlayerName;
		PlayerLevel = MS->PlayerLevel;
		SaveLocation = MS->SaveLocation;
	return *this;}
	//
	//
	friend FORCEINLINE uint32 GetTypeHash(const FSlotMeta &MS) {
		return FCrc::MemCrc32(&MS,sizeof(FSlotMeta));
	}///
};

USTRUCT(BlueprintType,meta=(DisplayName="Slot Data"))
struct SAVIOR3_API FSlotData {
	GENERATED_USTRUCT_BODY()
	//
	UPROPERTY(Category="Data",VisibleAnywhere) TMap<FName,FSaviorMinimal>Minimal;
	UPROPERTY(Category="Data",VisibleAnywhere) TMap<FName,FSaviorRecord>Complex;
	//
	//
	FSlotData()
		: Minimal(TMap<FName,FSaviorMinimal>{})
		, Complex(TMap<FName,FSaviorRecord>{})
	{}///
	//
	FSlotData(const UDataSlot* DS)
		: Minimal(TMap<FName,FSaviorMinimal>{})
		, Complex(TMap<FName,FSaviorRecord>{})
	{///
		if (DS->IsValidLowLevelFast()) {
			Minimal = DS->Minimal;
			Complex = DS->Complex;
		}///
	}///
	//
	//
	FORCEINLINE FSlotData &operator = (const FSlotData &SD) {
		Minimal = SD.Minimal;
		Complex = SD.Complex;
	return *this;}
	//
	FORCEINLINE FSlotData &operator = (UDataSlot* DS) {
		Minimal = DS->Minimal;
		Complex = DS->Complex;
	return *this;}
	//
	FORCEINLINE FSlotData &operator = (const UDataSlot* DS) {
		Minimal = DS->Minimal;
		Complex = DS->Complex;
	return *this;}
	//
	//
	friend FORCEINLINE uint32 GetTypeHash(const FSlotData &SD) {
		return FCrc::MemCrc32(&SD,sizeof(FSlotData));
	}///
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////