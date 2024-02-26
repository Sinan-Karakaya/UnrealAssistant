// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "UnrealAssistantStyle.h"

class FUnrealAssistantCommands : public TCommands<FUnrealAssistantCommands>
{
public:

	FUnrealAssistantCommands()
		: TCommands<FUnrealAssistantCommands>(TEXT("UnrealAssistant"), NSLOCTEXT("Contexts", "UnrealAssistant", "UnrealAssistant Plugin"), NAME_None, FUnrealAssistantStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
