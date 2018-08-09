// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "ImyPlugin.h"
#include "LevelEditor.h"
#include "Runtime/Slate/Public/Framework/MultiBox/MultiBoxBuilder.h"
#include "Runtime/Slate/Public/Framework/Application/SlateApplication.h"
#include "Runtime/Slate/Public/Widgets/Input/SButton.h"
#include "Editor//MainFrame/Public/Interfaces/IMainFrameModule.h"
#include "SWindow.h"

#define LOCTEXT_NAMESPACE "MyPlugin"


class FmyPlugin : public ImyPlugin
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

IMPLEMENT_MODULE(FmyPlugin, myPlugin)



void FmyPlugin::StartupModule()
{
	// This code will execute after your module is loaded into memory (but after global variables are initialized, of course.)
	UE_LOG(LogTemp, Warning, TEXT("かいしだよ～"));
	if (IsRunningCommandlet()) { return; }

	Extender = MakeShareable(new FExtender);
	Extender->AddMenuExtension(
		"LevelEditor",
		EExtensionHook::After,
		NULL,
		FMenuExtensionDelegate::CreateRaw(this, &FmyPlugin::OnWindowMenuExtension)
	);
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(Extender);

	IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
	MainFrameModule.OnMainFrameCreationFinished().AddRaw(
		this, &FmyPlugin::OnMainFrameLoad
	);
}

void FmyPlugin::ShutdownModule()
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

void FmyPlugin::OnWindowMenuExtension(FMenuBuilder & MenuBuilder) {
	MenuBuilder.BeginSection("MyMenuHook", LOCTEXT("MyMenu", "MyMenu"));
	MenuBuilder.AddMenuEntry(
		LOCTEXT("MyMenuTitle", "MyMenuTitle"),
		LOCTEXT("MyMenuToolTip", "hello..."),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FmyPlugin::OnMyToolMenu))
	);
	MenuBuilder.EndSection();
}

void FmyPlugin::OnMyToolMenu() {
	UE_LOG(LogTemp, Warning, TEXT("いべんとだよ～"));

	if (!MyWindow.IsValid()) {
		TSharedPtr<SWindow> Window = SNew(SWindow)
			.Title(LOCTEXT("FileImporter", "FileImporter"))
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
				SNew(STextBlock).Text(LOCTEXT("Hello", "Hello"))
			] + SVerticalBox::Slot().VAlign(VAlign_Center).FillHeight(1.f).Padding(2.f)[
				SNew(SButton).Text(LOCTEXT("Button", "Button")).OnClicked_Raw(this, &FmyPlugin::OnButtonClicked)
			]
					);
	}
	MyWindow.Pin()->BringToFront();
}

void FmyPlugin::OnMainFrameLoad(TSharedPtr<SWindow> InRootWindow, bool bIsNewProjectWindow) {
	if ((!bIsNewProjectWindow) && (InRootWindow.IsValid())) {
		RootWindow = InRootWindow;
	}
}

FReply FmyPlugin::OnButtonClicked() {
	UE_LOG(LogTemp, Warning, TEXT("ぼたんだよ～"));

	

	return FReply::Handled();
}