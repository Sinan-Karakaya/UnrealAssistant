// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealAssistantCommands.h"

#define LOCTEXT_NAMESPACE "FUnrealAssistantModule"

void FUnrealAssistantCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "UnrealAssistant", "Execute UnrealAssistant action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
