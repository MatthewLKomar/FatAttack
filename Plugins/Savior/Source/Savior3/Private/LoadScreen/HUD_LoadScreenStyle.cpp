//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2020 (C) Bruno Xavier Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "HUD_LoadScreenStyle.h"

#include "Runtime/SlateCore/Public/SlateGlobals.h"
#include "Runtime/SlateCore/Public/Styling/SlateStyle.h"
#include "Runtime/SlateCore/Public/Styling/SlateTypes.h"
#include "Runtime/SlateCore/Public/Brushes/SlateBoxBrush.h"
#include "Runtime/SlateCore/Public/Brushes/SlateColorBrush.h"
#include "Runtime/SlateCore/Public/Brushes/SlateImageBrush.h"
#include "Runtime/SlateCore/Public/Brushes/SlateBorderBrush.h"
#include "Runtime/SlateCore/Public/Styling/SlateStyleRegistry.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define BORDER_BRUSH(RelativePath,...) FSlateBorderBrush(FSaviorLoadScreenStyle::InContent(RelativePath,".png"),__VA_ARGS__)
#define IMAGE_BRUSH(RelativePath,...) FSlateImageBrush(FSaviorLoadScreenStyle::InContent(RelativePath,".png"),__VA_ARGS__)
#define BOX_BRUSH(RelativePath,...) FSlateBoxBrush(FSaviorLoadScreenStyle::InContent(RelativePath,".png"),__VA_ARGS__)
#define TTF_FONT(RelativePath,...) FSlateFontInfo(FSaviorLoadScreenStyle::InContent(RelativePath,".ttf"),__VA_ARGS__)
#define OTF_FONT(RelativePath,...) FSlateFontInfo(FSaviorLoadScreenStyle::InContent(RelativePath,".otf"),__VA_ARGS__)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const ISlateStyle &FSaviorLoadScreenStyle::IGet() {return StyleSet.ToSharedRef().Get();}
TSharedPtr<ISlateStyle> FSaviorLoadScreenStyle::Get() {return StyleSet;}
TSharedPtr<FSlateStyleSet> FSaviorLoadScreenStyle::StyleSet = nullptr;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FString FSaviorLoadScreenStyle::InContent(const FString &RelativePath, const ANSICHAR* Extension) {
	static FString Content = FString(FPaths::EngineContentDir()/TEXT("Slate"));
	return (Content/RelativePath)+Extension;
}

FName FSaviorLoadScreenStyle::GetStyleSetName() {
	static FName StyleName(TEXT("SaviorLoadScreenStyle"));
	return StyleName;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FSaviorLoadScreenStyle::Initialize() {
	if (StyleSet.IsValid()) {return;}
	//
	StyleSet = MakeShareable(new FSlateStyleSet(GetStyleSetName()));
	StyleSet->SetContentRoot(FPaths::EngineContentDir()/TEXT("Slate"));
	//
	{
		StyleSet->Set("SaviorProgressBar",FProgressBarStyle()
			.SetBackgroundImage(BOX_BRUSH("Common/GroupBorder",FMargin(5.f/12.f)))
			.SetFillImage(BOX_BRUSH("Common/ProgressBar_Fill",FMargin(5.f/12.f),FLinearColor::White))
			.SetMarqueeImage(IMAGE_BRUSH("Common/ProgressBar_Marquee",FVector2D(20,12),FLinearColor::White,ESlateBrushTileType::Horizontal))
		);//
	}
	//
	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
};

void FSaviorLoadScreenStyle::Shutdown() {
	if (StyleSet.IsValid()) {
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
		ensure(StyleSet.IsUnique());
		StyleSet.Reset();
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#undef BORDER_BRUSH
#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef TTF_FONT
#undef OTF_FONT

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////