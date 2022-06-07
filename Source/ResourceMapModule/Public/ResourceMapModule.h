// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Stats/Stats.h"
#include "Modules/ModuleManager.h"
#include "ResourceMapManager.h"

DECLARE_STATS_GROUP(TEXT("RMM_Game"), STATGROUP_RMM, STATCAT_Advanced);

class RESOURCEMAPMODULE_API FResourceMapModule : public IModuleInterface
{
public:

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	UResourceMapManager* CreateResourceMapManager(UObject* owner, int size) {
		auto ref = NewObject<UResourceMapManager>(owner,FName("MapManager"+owner->GetName()));
		ref->ReInitialize(size);
		return ref;
	}
};