#pragma once

#include "CoreMinimal.h"
#include "VelocityField.generated.h"

#define IX(x,y) ((y) * (Size+2) + (x))
#define FLAT_SIZE(s) ((s)+2)*((s)+2)
UCLASS(Blueprintable)
class UVelocityField : public UObject {
	GENERATED_BODY()
protected:
	UPROPERTY()
	FName Name;
	UPROPERTY()
	int Size;
	UPROPERTY()
	float Viscosity;
	UPROPERTY()
	TArray<float> U;
	UPROPERTY()
	TArray<float> V;
	UPROPERTY()
	TArray<float> U0;
	UPROPERTY()
	TArray<float> V0;
	void Initialize(FName name, int size,float viscoity) {
		Name = name;
		Size = size;
		Viscosity = viscoity;
		U.Init(0, FLAT_SIZE(size));
		U0.Init(0, FLAT_SIZE(size));
		V.Init(0, FLAT_SIZE(size));
		V0.Init(0, FLAT_SIZE(size));
	}

	virtual void ReInitialize(int size) {
		Size = size;
		U.Init(0, FLAT_SIZE(size));
		U0.Init(0, FLAT_SIZE(size));
		V.Init(0, FLAT_SIZE(size));
		V0.Init(0, FLAT_SIZE(size));
	}

	friend class UResourceMapManager;
public:
	UVelocityField():Name(),Size(),U(),V() {}
	UVelocityField(FName name, int size) :Name(name), Size(size) {
		U.Init(0, FLAT_SIZE(size));
		V.Init(0, FLAT_SIZE(size));
	}

	 UFUNCTION(BlueprintCallable)
		 float GetU(const int x, const int y) const {
		return U[IX(x, y)];
	}
	 UFUNCTION(BlueprintCallable)
		 TArray<float>& GetUArray() {
		return U;
	}
	 UFUNCTION(BlueprintCallable)
		 TArray<float>& GetU0Array() {
		return U0;
	}
	 UFUNCTION(BlueprintCallable)
		 float GetV(const int x, const int y) const {
		return V[IX(x, y)];
	}
	 UFUNCTION(BlueprintCallable)
		 TArray<float>& GetVArray() {
		return V;
	}
	 UFUNCTION(BlueprintCallable)
		 TArray<float>& GetV0Array() {
		return V0;
	}
	 UFUNCTION(BlueprintCallable)
		 void SetU(const int x, const int y, const float value) {
		U[IX(x, y)] = value;
	}
	 UFUNCTION(BlueprintCallable)
		 void SetV(const int x, const int y, const float value) {
		V[IX(x, y)] = value;
	}
	 UFUNCTION(BlueprintCallable)
		 float FlatGetU(const int i) const {
		return U[i];
	}
	 UFUNCTION(BlueprintCallable)
		 float FlatGetV(const int i) const {
		return V[i];
	}
	 UFUNCTION(BlueprintCallable)
		 void FlatSetU(const int i, const float value) {
		U[i] = value;
	}
	 UFUNCTION(BlueprintCallable)
		 void FlatSetV(const int i, const float value) {
		V[i] = value;
	}
	 UFUNCTION(BlueprintCallable)
	float GetViscosity() {
		return Viscosity;
	}
	 UFUNCTION(BlueprintCallable)
		 void SetViscosity(const float value) {
		 Viscosity = value;
	}

	 UFUNCTION(BlueprintCallable)
		 FVector2D Get(const int x, const int y) const {
		 return { U[IX(x, y)], V[IX(x,y)] };
	 }
	 UFUNCTION(BlueprintCallable)
		 void Set(const int x, const int y, const FVector2D& value) {
		const auto flatI = IX(x, y);
		U[flatI] = value.X;
		V[flatI] = value.Y;
	}
	 UFUNCTION(BlueprintCallable)
		 FVector2D FlatGet(const int i) const {
		return { U[i], V[i] };
	}
	 UFUNCTION(BlueprintCallable)
		 void FlatSet(const int i, const FVector2D& value) {
		U[i] = value.X;
		V[i] = value.Y;
	}

	virtual ~UVelocityField() = default;
};