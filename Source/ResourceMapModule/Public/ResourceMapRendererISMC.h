// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ResourceMapModule.h"
#include "ResourceMapManager.h"
#include "PreRenderLayersProccesor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "ResourceMapRendererISMC.generated.h"


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
	UPROPERTY(VisibleAnywhere)
	bool isDirty;
	UPROPERTY(VisibleAnywhere, Instanced)
	UInstancedStaticMeshComponent* MeshComponent;
	UPROPERTY()
	TArray<FTransform> transformsBuffer;

public:
	UDrawableLayer(){}

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
		bool IsDirty() const {
		return isDirty;
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
		MeshComponent->SetVisibility(drawable);
	}

	UFUNCTION(BlueprintCallable)
		void MarkDirty() {
		isDirty = true;
	}

	UFUNCTION(BlueprintCallable)
	void MarkClean() {
		isDirty = false;
	}

	UFUNCTION(BlueprintCallable)
		void ReInitialize(int s) {
		Size = s;
	}

	UFUNCTION(BlueprintCallable)
	void AddInstance(const FTransform& t) {
		transformsBuffer.Add(t);
		MeshComponent->AddInstance(t);
	}

	UFUNCTION(BlueprintCallable)
	void SetInstanceTransform(int index, const FTransform& t) {
		auto& oldT=transformsBuffer[index];
		if (!t.Equals(oldT,1e-4)) {
			transformsBuffer[index] = t;
			MarkDirty();
		}
	}
	UFUNCTION(BlueprintCallable)
	void MeshUpdate() {
		if (isDirty) {
			MeshComponent->BatchUpdateInstancesTransforms(0, transformsBuffer, false, true, true);
			MarkClean();
		}
	}
	UFUNCTION(BlueprintCallable)
	void Clear() {
		MeshComponent->DestroyComponent(true);
		transformsBuffer.Empty();
	}

};

UCLASS()
class RESOURCEMAPMODULE_API AResourceMapRendererISMC : public AActor
{
	GENERATED_BODY()

public:
	AResourceMapRendererISMC();

protected:
	UPROPERTY(BlueprintReadOnly,VisibleAnywhere,Instanced)
	UResourceMapManager* manager;
	UPROPERTY()
	USceneComponent* root;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int Size;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CellSize;
	UPROPERTY(Instanced)
	TMap<FName, UDrawableLayer*> drawableLayers;
	UPROPERTY()
	TMap<FName, TScriptInterface<IPreRenderLayersProccesor>> preRenderPasses;
	static const float GroundOffset;
	static const float ZeroEpsilon;

	virtual void CreateMeshesForLayer(UDrawableLayer* layer);

public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UStaticMesh* Mesh;

	UFUNCTION(BlueprintCallable)
	virtual void SetSize(int NewSize);
	UFUNCTION(BlueprintCallable)
	virtual void AddLayerDrawable(const FName layerName, bool isDrawable, UMaterialInterface* material=nullptr, int NumCustomDataFloats=0);
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
	virtual void AddToPreRenderPass(const FName layerName, const TScriptInterface<IPreRenderLayersProccesor>& pass);
	UFUNCTION(BlueprintCallable)
	virtual UDrawableLayer* GetLayer(const FName name);
	UFUNCTION(BlueprintCallable)
	virtual void LogStats();
	UFUNCTION(BlueprintCallable)
	virtual void Clear();
};
