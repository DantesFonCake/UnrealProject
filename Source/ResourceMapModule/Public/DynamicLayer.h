#pragma once

#include "StaticLayer.h"
#include "DynamicLayer.generated.h"

UCLASS(Blueprintable)
class UDynamicLayer : public UStaticLayer {
	GENERATED_BODY()
	protected:
		UPROPERTY()
		float Diffuse;
		UPROPERTY()
		TArray<float> Future;
		UPROPERTY()
		FName AssociatedGroundName;
		UPROPERTY()
		FName AssociatedDiffuseMapName;
		UPROPERTY()
		FName AssociatedVelocityFieldLayer;

		void Initialize(FName name, int size, float diffuse, FName associatedGroundName, FName associatedDiffuseMapName, FName associatedVelocityFieldLayer){
			UStaticLayer::Initialize(name, size);
			Diffuse = diffuse;
			Future.Init(0, FLAT_SIZE(size));
			AssociatedGroundName = associatedGroundName;
			AssociatedDiffuseMapName = associatedDiffuseMapName;
			AssociatedVelocityFieldLayer = associatedVelocityFieldLayer;
		}
		
		virtual void ReInitialize(int size) override {
			UStaticLayer::ReInitialize(size);
			Future.Init(0,FLAT_SIZE(size));
		}

		friend class UResourceMapManager;
	public:
		UDynamicLayer():Diffuse(),Future(),AssociatedGroundName(),AssociatedDiffuseMapName(),AssociatedVelocityFieldLayer() {}
		UDynamicLayer(FName name, int size, float diffuse, FName associatedGroundName, FName associatedDiffuseMapName, FName associatedVelocityFieldLayer) :
			UStaticLayer(name, size), Diffuse(diffuse), 
			AssociatedGroundName(associatedGroundName), 
			AssociatedDiffuseMapName(associatedDiffuseMapName), 
			AssociatedVelocityFieldLayer(associatedVelocityFieldLayer) {
			Future.Init(0, FLAT_SIZE(size));
		}

		 UFUNCTION(BlueprintCallable)
			 float GetDiffuse() const {
			return Diffuse;
		}
		 UFUNCTION(BlueprintCallable)
			 void SetDiffuse(float value) {
			Diffuse=value;
		}
		 UFUNCTION(BlueprintCallable)
			 FName GetAssociatedGroundName() const {
			return AssociatedGroundName;
		}
		 UFUNCTION(BlueprintCallable)
			 FName GetAssociatedDiffuseMapName() const {
			return AssociatedDiffuseMapName;
		}
		 UFUNCTION(BlueprintCallable)
			 FName GetAssociatedVelocityFieldLayer() const {
			return AssociatedVelocityFieldLayer;
		}
		 UFUNCTION(BlueprintCallable)
			 TArray<float>& GetFutureArray() {
			return Future;
		}
	};