// Copyright Epic Games, Inc. All Rights Reserved.

#include "ResourceMapModule.h"

#define LOCTEXT_NAMESPACE "FResourceMapModule"

//DEFINE_LOG_CATEGORY(ResourceMapModuleLogs);

void FResourceMapModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FResourceMapModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FResourceMapModule, ResourceMapModule)