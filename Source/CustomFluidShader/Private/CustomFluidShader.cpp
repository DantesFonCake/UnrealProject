// Copyright Epic Games, Inc. All Rights Reserved.

#include "CustomFluidShader.h"

#define LOCTEXT_NAMESPACE "FCustomFluidShaderModule"

void FCustomFluidShader::StartupModule()
{
	FString ShaderDirectory = FPaths::Combine(FPaths::ProjectDir(), TEXT("Shaders/Private"));
	AddShaderSourceDirectoryMapping("/CustomShaders", ShaderDirectory);
}

void FCustomFluidShader::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCustomFluidShader, CustomFluidShader)