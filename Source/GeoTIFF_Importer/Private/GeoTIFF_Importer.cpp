// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "GeoTIFF_Importer.h"
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#include "LevelEditor.h"
#include "LandscapeEditorModule.h"
#include "MultiBoxBuilder.h"
#include "SlateApplication.h"
#include "SButton.h"
#include "IMainFrameModule.h"
#include "SWindow.h"
#include "DesktopPlatformModule.h"
#include "LandscapeFileFormatGeoTIFF.h"

#define LOCTEXT_NAMESPACE "crssnkyPlugin"


class FGeoTIFF_Importer: public IGeoTIFF_Importer{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	void OnWindowMenuExtension(FMenuBuilder& MenuBuilder);
	void OnMyToolMenu();
	void OnChecked();
	void OnUnChecked();
	TSharedPtr<FExtender> Extender;
	TSharedPtr<FLandscapeHeightmapFileFormat_GeoTIFF> GeoTIFF_Importer;
	bool b = false;
};

IMPLEMENT_MODULE(FGeoTIFF_Importer, GeoTIFF_Importer)

void FGeoTIFF_Importer::StartupModule(){
	// This code will execute after your module is loaded into memory (but after global variables are initialized, of course.)
	UE_LOG(LogTemp, Display, TEXT("Loaded GeoTIFF_Importer"));
	if(IsRunningCommandlet()){ return; }

	GeoTIFF_Importer = MakeShareable(new FLandscapeHeightmapFileFormat_GeoTIFF());
	Extender = MakeShareable(new FExtender);
	Extender->AddMenuExtension(
		"LevelEditor",
		EExtensionHook::After,
		NULL,
		FMenuExtensionDelegate::CreateRaw(this, &FGeoTIFF_Importer::OnWindowMenuExtension)
	);
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(Extender);
}

void FGeoTIFF_Importer::ShutdownModule(){
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UE_LOG(LogTemp, Display, TEXT("Unloaded GeoTIFF_Importer"));
	if(Extender.IsValid() && FModuleManager::Get().IsModuleLoaded("LevelEditor")){
		FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
		LevelEditorModule.GetMenuExtensibilityManager()->RemoveExtender(Extender);
	}

	if(FModuleManager::Get().IsModuleLoaded("MainFrame")){
		IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
		MainFrameModule.OnMainFrameCreationFinished().RemoveAll(this);
	}
}

void FGeoTIFF_Importer::OnWindowMenuExtension(FMenuBuilder & MenuBuilder){
	MenuBuilder.BeginSection("crssnky's Menu", FText::FromString("crssnky's Menu"));
	MenuBuilder.AddMenuEntry(
		FText::FromString("GeoTIFF Importer"),
		FText::FromString("If true, landscape support GeoTIFF."),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateRaw(this, &FGeoTIFF_Importer::OnMyToolMenu),
			FCanExecuteAction(),
			FIsActionChecked::CreateLambda([=]{return b; })
		),
		NAME_None,
		EUserInterfaceActionType::ToggleButton,
		NAME_None
	);
	MenuBuilder.EndSection();
}

void FGeoTIFF_Importer::OnMyToolMenu(){
	b = !b;
	if(b){
		UE_LOG(LogTemp, Display, TEXT("GeoTIFF_Importer true."));
		OnChecked();
	} else{
		UE_LOG(LogTemp, Display, TEXT("GeoTIFF_Importer false."));
		OnUnChecked();
	}
}

void FGeoTIFF_Importer::OnChecked(){
	ILandscapeEditorModule& LEM = FModuleManager::GetModuleChecked<ILandscapeEditorModule>("LandscapeEditor");
	FString extensions = LEM.GetHeightmapImportDialogTypeString();
	if(!extensions.Contains(".tif")){
		LEM.RegisterHeightmapFileFormat(GeoTIFF_Importer.ToSharedRef());
	}
}

void FGeoTIFF_Importer::OnUnChecked(){
	ILandscapeEditorModule& LEM = FModuleManager::GetModuleChecked<ILandscapeEditorModule>("LandscapeEditor");
	FString extensions = LEM.GetHeightmapImportDialogTypeString();
	if(extensions.Contains(".tif")){
		LEM.UnregisterHeightmapFileFormat(GeoTIFF_Importer.ToSharedRef());
	}
}