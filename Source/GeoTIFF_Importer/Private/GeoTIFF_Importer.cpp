// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "IGeoTIFF_Importer.h"

#include "LevelEditor.h"
#include "MultiBoxBuilder.h"
#include "SlateApplication.h"
#include "SButton.h"
#include "IMainFrameModule.h"
#include "SWindow.h"
#include "DesktopPlatformModule.h"

#define LOCTEXT_NAMESPACE "crssnkyPlugin"


class FGeoTIFF_Importer : public IGeoTIFF_Importer
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	void OnWindowMenuExtension(FMenuBuilder& MenuBuilder);
	void OnMyToolMenu();
	void OnMainFrameLoad(TSharedPtr<SWindow> InRootWindow, bool bIsNewProjectWindow);
	FReply OnButtonClicked();
	TSharedPtr<FExtender> Extender;
	TWeakPtr<SWindow> RootWindow;
	TWeakPtr<SWindow> MyWindow;
};

IMPLEMENT_MODULE(FGeoTIFF_Importer, myPlugin)



void FGeoTIFF_Importer::StartupModule()
{
	// This code will execute after your module is loaded into memory (but after global variables are initialized, of course.)
	UE_LOG(LogTemp, Warning, TEXT("かいしだよ～"));
	if (IsRunningCommandlet()) { return; }

	Extender = MakeShareable(new FExtender);
	Extender->AddMenuExtension(
		"LevelEditor",
		EExtensionHook::After,
		NULL,
		FMenuExtensionDelegate::CreateRaw(this, &FGeoTIFF_Importer::OnWindowMenuExtension)
	);
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(Extender);

	IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
	MainFrameModule.OnMainFrameCreationFinished().AddRaw(
		this, &FGeoTIFF_Importer::OnMainFrameLoad
	);
}

void FGeoTIFF_Importer::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UE_LOG(LogTemp, Warning, TEXT("おわりだよ～"));
	if (Extender.IsValid() && FModuleManager::Get().IsModuleLoaded("LevelEditor")) {
		FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
		LevelEditorModule.GetMenuExtensibilityManager()->RemoveExtender(Extender);
	}

	if (FModuleManager::Get().IsModuleLoaded("MainFrame")) {
		IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
		MainFrameModule.OnMainFrameCreationFinished().RemoveAll(this);
	}
}

void FGeoTIFF_Importer::OnWindowMenuExtension(FMenuBuilder & MenuBuilder) {
	MenuBuilder.BeginSection("crssnkyMenuHook", FText::FromString("crssnkyMenu"));
	MenuBuilder.AddMenuEntry(
		FText::FromString("GeoTIFF Importer"),
		FText::FromString("GeoTIFF→Landscape"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FGeoTIFF_Importer::OnMyToolMenu))
	);
	MenuBuilder.EndSection();
}

void FGeoTIFF_Importer::OnMyToolMenu() {
	UE_LOG(LogTemp, Warning, TEXT("いべんとだよ～"));

	if (!MyWindow.IsValid()) {
		TSharedPtr<SWindow> Window = SNew(SWindow)
			.Title(FText::FromString("FileImporter"))
			.ClientSize(FVector2D(300.f, 300.f));
		MyWindow = TWeakPtr<SWindow>(Window);
		if (RootWindow.IsValid()) {
			FSlateApplication::Get().AddWindowAsNativeChild(
				Window.ToSharedRef(),
				RootWindow.Pin().ToSharedRef()
			);
		}
		Window->SetContent(
			SNew(SVerticalBox) + SVerticalBox::Slot().VAlign(VAlign_Center).FillHeight(1.f).Padding(2.f)[
				SNew(STextBlock).Text(FText::FromString("Hello"))
			] + SVerticalBox::Slot().VAlign(VAlign_Center).FillHeight(1.f).Padding(2.f)[
				SNew(SButton).Text(FText::FromString("Button")).OnClicked_Raw(this, &FGeoTIFF_Importer::OnButtonClicked)
			]
					);
	}
	MyWindow.Pin()->BringToFront();
}

void FGeoTIFF_Importer::OnMainFrameLoad(TSharedPtr<SWindow> InRootWindow, bool bIsNewProjectWindow) {
	if ((!bIsNewProjectWindow) && (InRootWindow.IsValid())) {
		RootWindow = InRootWindow;
	}
}

FReply FGeoTIFF_Importer::OnButtonClicked() {
	UE_LOG(LogTemp, Warning, TEXT("ぼたんだよ～"));



	return FReply::Handled();
}