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

	AskText = LOCTEXT("AskText", "");
	AskRequest->SetStringField(TEXT("model"), TEXT("mistral"));
	AskRequest->SetStringField(TEXT("format"), TEXT("json"));
	AskRequest->SetBoolField(TEXT("stream"), false);
	AskRequest->SetStringField(TEXT("system"), TEXT("You are a helpful Unreal Engine 5 assistant. The user will ask you to do something."
		"You must answer in JSON. Your answer must be made of a list of primitives (cube, sphere...), that contains basic parameters, such as Transform or Material."
		"If the request is too complex, answer by sending an error that is set to true."
	));
	pRequest->SetVerb(TEXT("POST"));
	pRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	pRequest->SetURL(TEXT("http://localhost:11434/api/generate"));
	pRequest->SetTimeout(50.f);
	pRequest->OnProcessRequestComplete().BindRaw(this, &FUnrealAssistantModule::OnAskCallback);
	
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
						.Text(AskText)
						.HintText(LOCTEXT("PromptHintText", "Ask what you want your Assistant to do..."))
						.Padding(FMargin(12.f, 6.f, 12.f, 6.f))
						.IsEnabled(!bIsWaitingForResponse)
						.OnTextChanged_Lambda([this](const FText &InText) { AskRequest->SetStringField(TEXT("prompt"), InText.ToString()); })
				]
			+SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.f, 4.f, 0.f, 0.f))
				[
					SNew(SButton)
						.Text(LOCTEXT("AskButton", "Ask"))
						.OnClicked_Raw(this, &FUnrealAssistantModule::OnAskClicked)
						.IsEnabled(!bIsWaitingForResponse)
						.HAlign(HAlign_Center)
				]
		];
}

FReply FUnrealAssistantModule::OnAskClicked()
{
	bIsWaitingForResponse = true;

	FString RequestString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&RequestString);
	FJsonSerializer::Serialize(AskRequest.ToSharedRef(), JsonWriter);
	pRequest->SetContentAsString(RequestString);
	UE_LOG(LogInit, Warning, TEXT("Sending: %s"), *RequestString);
	pRequest->ProcessRequest();

	return FReply::Handled();
}

// FIXME: unexpected EOF
void FUnrealAssistantModule::OnAskCallback(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response->GetContentType() == "application/json") {
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(Response->GetContentAsString());

		FJsonSerializer::Deserialize(JsonReader, JsonObject);
		UE_LOG(LogInit, Log, TEXT("Response successful: %s"), *(Response->GetContentAsString()));
	} else {
		UE_LOG(LogInit, Error, TEXT("Response unsuccessful: %s"), *(Response->GetContentAsString()));
	}
	bIsWaitingForResponse = false;
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