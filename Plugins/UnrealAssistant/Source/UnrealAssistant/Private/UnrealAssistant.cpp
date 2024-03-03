// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealAssistant.h"
#include "UnrealAssistantStyle.h"
#include "UnrealAssistantCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"

static const FName UnrealAssistantTabName("UnrealAssistant");

#define LOCTEXT_NAMESPACE "FUnrealAssistantModule"

void FUnrealAssistantModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FUnrealAssistantStyle::Initialize();
	FUnrealAssistantStyle::ReloadTextures();

	FUnrealAssistantCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FUnrealAssistantCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FUnrealAssistantModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FUnrealAssistantModule::RegisterMenus));
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(UnrealAssistantTabName, FOnSpawnTab::CreateRaw(this, &FUnrealAssistantModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FUnrealAssistantTabTitle", "Unreal Assistant"))
		.SetMenuType(ETabSpawnerMenuType::Enabled);
}

void FUnrealAssistantModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FUnrealAssistantStyle::Shutdown();

	FUnrealAssistantCommands::Unregister();
}

void FUnrealAssistantModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(UnrealAssistantTabName);
}

TSharedRef<SDockTab> FUnrealAssistantModule::OnSpawnPluginTab(const FSpawnTabArgs &SpawnTabArgs)
{
	FText WidgetText = LOCTEXT("WindowWidgetText", "");

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
				.Padding(FMargin(6.f))
				.FillHeight(250.f)
				[
					SNew(SMultiLineEditableTextBox)
						.Text(WidgetText)
						.HintText(LOCTEXT("PromptHintText", "Ask what you want your Assistant to do..."))
						.Padding(FMargin(12.f, 6.f, 12.f, 6.f))
				]
			+SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.f, 4.f, 0.f, 0.f))
				[
					SNew(SBox)
					.WidthOverride(200)
						[
							SNew(SButton)
								.Text(LOCTEXT("AskButton", "Ask"))
								.OnClicked_Raw(this, &FUnrealAssistantModule::OnAskClicked)
								.IsEnabled(!bIsAsking)
								.HAlign(HAlign_Center)
						]
				]
		];
}

FReply FUnrealAssistantModule::OnAskClicked()
{
	return FReply::Handled();
}

void FUnrealAssistantModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FUnrealAssistantCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FUnrealAssistantCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUnrealAssistantModule, UnrealAssistant)