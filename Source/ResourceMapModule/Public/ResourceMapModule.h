// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "ResourceMapManager.h"

class RESOURCEMAPMODULE_API FResourceMapModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	UResourceMapManager* CreateResourceMapManager(UObject* owner, int size) {
		auto ref = NewObject<UResourceMapManager>(owner,FName("MapManager"+owner->GetName()));
		ref->ReInitialize(size);
		return ref;
	}
};

//DECLARE_LOG_CATEGORY_EXTERN(ResourceMapModuleLogs, Error, All);
