// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"

class FToolBarBuilder;
class FMenuBuilder;

class FUnrealAssistantModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command. */
	void PluginButtonClicked();
	
	
private:

	void RegisterMenus();
	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs &SpawnTabArgs);

	FReply OnAskClicked();
	void OnAskCallback(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

private:
	TSharedPtr<class FUICommandList> PluginCommands;
	FText AskText;
	bool bIsWaitingForResponse = false;

	FHttpModule &httpModule = FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> pRequest = httpModule.CreateRequest();
	TSharedPtr<FJsonObject> AskRequest = MakeShareable(new FJsonObject);
};
