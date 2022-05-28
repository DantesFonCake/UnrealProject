// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ResourceMapModule.h"
#include "ResourceMapManager.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "ResourceMapRendererPMC.generated.h"


UCLASS()
class UDrawableLayer : public UObject {
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere)
	FName LayerName;
	UPROPERTY(VisibleAnywhere)
	int Size;
	UPROPERTY(VisibleAnywhere)
	bool isDrawable;
	UPROPERTY(VisibleAnywhere)
	bool isDynamic;
	UPROPERTY(VisibleAnywhere, Instanced)
	UInstancedStaticMeshComponent* MeshComponent;

public:
	UDrawableLayer(){}
	UDrawableLayer(FName name, int size, bool defaultDrawable, UInstancedStaticMeshComponent* mesh) :LayerName(name), Size(size), isDrawable(defaultDrawable), MeshComponent(mesh)
	{}
	UFUNCTION(BlueprintCallable)
		FName GetName() const {
		return LayerName;
	}

	UFUNCTION(BlueprintCallable)
		int GetSize() const {
		return Size;
	}

	UFUNCTION(BlueprintCallable)
		bool IsDrawable() const {
		return isDrawable;
	}

	UFUNCTION(BlueprintCallable)
		bool IsDynamic() const {
		return isDynamic;
	}

	UFUNCTION(BlueprintCallable)
		UInstancedStaticMeshComponent* GetMesh() const {
		return MeshComponent;
	}

	UFUNCTION(BlueprintCallable)
	void Initialize(int s, UInstancedStaticMeshComponent* mesh, bool drawable, bool dynamic) {
		Size = s;
		MeshComponent = mesh;
		this->isDrawable = drawable;
		isDynamic = dynamic;
		mesh->SetVisibility(drawable);
	}

	UFUNCTION(BlueprintCallable)
		void SetDrawable(bool drawable) {
		isDrawable = drawable;
	}

	UFUNCTION(BlueprintCallable)
		void ReInitialize(int s) {
		Size = s;
	}
};

UCLASS()
class RESOURCEMAPMODULE_API AResourceMapRendererPMC : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AResourceMapRendererPMC();

protected:
	UPROPERTY(BlueprintReadOnly,VisibleAnywhere,Instanced)
	UResourceMapManager* manager;
	UPROPERTY()
	USceneComponent* root;
	UPROPERTY(BlueprintReadOnly)
	int Size;
	UPROPERTY(BlueprintReadOnly)
	float CellSize;
	UPROPERTY(Instanced)
	TMap<FName, UDrawableLayer*> drawableLayers;
	static const float GroundOffset;
	static const float ZeroEpsilon;

	virtual void CreateMeshesForLayer(UDrawableLayer* layer);

public:	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UStaticMesh* Mesh;

	UFUNCTION(BlueprintCallable)
	virtual void SetSize(int NewSize);
	UFUNCTION(BlueprintCallable)
	virtual void AddLayerDrawable(const FName layerName, bool isDrawable, UMaterialInterface* material);
	UFUNCTION(BlueprintCallable)
	virtual void SetLayerDrawable(const FName layerName, bool visible);
	virtual FTransform CreateCoordTransform(int x, int y, float height, float ground=0);
	UFUNCTION(BlueprintCallable)
	inline UResourceMapManager *GetManager()
	{
		return manager;
	}
	UFUNCTION(BlueprintCallable)
	virtual void UpdateFromManager();
	UFUNCTION(BlueprintCallable)
	virtual void LogStats();
	UFUNCTION(BlueprintCallable)
	virtual void Clear();
};
