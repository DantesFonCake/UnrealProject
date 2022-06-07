#pragma once

#include "CoreMinimal.h"
#include "StaticLayer.h"
#include "DynamicLayer.h"
#include "VelocityField.h"
#include "TimedLayersProccesor.h"
#include "ResourceMapManager.generated.h"

#define IX(x,y) ((y) * (Size+2) + (x))
#define FLAT_SIZE(s) ((s)+2)*((s)+2)
UCLASS(Blueprintable)
class UResourceMapManager : public UObject {
	GENERATED_BODY()

	static const FName ZeroLayerName;
	static const FName NoneVelocityFieldName;
	UPROPERTY(Instanced)
	TMap<FName, UStaticLayer*> NamedStaticLayers;
	UPROPERTY(Instanced)
	TMap<FName, UDynamicLayer*> NamedDynamicLayers;
	UPROPERTY(Instanced)
	TMap<FName, UVelocityField*> NamedVelocityFields;
	UPROPERTY()
	TArray<TScriptInterface<ITimedLayersProccesor>> ProccesPass;
	UPROPERTY(EditDefaultsOnly)
	int Size;

public:
	UResourceMapManager():NamedStaticLayers(),NamedDynamicLayers(),NamedVelocityFields(),ProccesPass(), Size(0)
	{
		auto layer = CreateDefaultSubobject<UStaticLayer>(ZeroLayerName);
		layer->Initialize(ZeroLayerName, Size);
		NamedStaticLayers.Add(ZeroLayerName, layer);
	};
	UResourceMapManager(int size): NamedStaticLayers(), NamedDynamicLayers(), NamedVelocityFields(), ProccesPass(),Size(size) {
		auto layer = CreateDefaultSubobject<UStaticLayer>(ZeroLayerName);
		layer->Initialize(ZeroLayerName, Size);
		NamedStaticLayers.Add(ZeroLayerName, layer);
	}

	UFUNCTION(BlueprintCallable)
	UDynamicLayer* GetNamedDynamicLayer(FName layer) {
		return NamedDynamicLayers[layer];
	}
	UFUNCTION(BlueprintCallable)
	UVelocityField* GetNamedVelocityField(FName field) {
		return NamedVelocityFields[field];
	}
	UFUNCTION(BlueprintCallable)
	UStaticLayer* GetNamedStaticLayer(const FName LayerName) {
		return NamedStaticLayers[LayerName];
	}
	UFUNCTION(BlueprintCallable)
	virtual void ReInitialize(int s);

	UFUNCTION(BlueprintCallable)
	virtual void AddStaticLayer(const FName LayerName);
	UFUNCTION(BlueprintCallable)
	inline FName GetZeroLayerName() {
		return ZeroLayerName;
	};
	UFUNCTION(BlueprintCallable)
	inline FName GetNoneVelocityFieldName() {
		return NoneVelocityFieldName;
	}
	UFUNCTION(BlueprintCallable)
	virtual void AddDynamicLayer(const FName LayerName, float diffuse, const FName AssociatedGroundLayer, const FName AssociatedDiffuseMapLayer, FName AssociatedVelocityFieldLayer);
	UFUNCTION(BlueprintCallable)
	virtual void AddVelocityField(const FName LayerName, const float Viscosity);
	UFUNCTION(BlueprintCallable)
	virtual bool LayerExists(const FName LayerName) const;
	UFUNCTION(BlueprintCallable)
	virtual bool IsDynamic(const FName LayerName) const;
	UFUNCTION(BlueprintCallable)
	virtual bool VelocityFieldExist(const FName LayerName) const;
	UFUNCTION(BlueprintCallable)
	virtual void ProccessPass(float DeltaTime);
	UFUNCTION(BlueprintCallable)
	virtual void Clear();

	UFUNCTION(BlueprintCallable)
	virtual void AddProccesorToProccesPass(UPARAM(ref)TScriptInterface<ITimedLayersProccesor>& proccesor);

	UFUNCTION(BlueprintCallable)
	virtual void LogStats();

	UFUNCTION(BlueprintCallable)
		int GetSize() const;
};