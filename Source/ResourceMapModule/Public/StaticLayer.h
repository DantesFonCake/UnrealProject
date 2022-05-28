#pragma once

#include "CoreMinimal.h"
#include "StaticLayer.generated.h"

#define IX(x,y) ((y) * (Size+2) + (x))
#define FLAT_SIZE(s) ((s)+2)*((s)+2)
UCLASS(Blueprintable)
class UStaticLayer : public UObject  {
		GENERATED_BODY()
	protected:
		UPROPERTY()
		bool dirty=true;
		UPROPERTY()
		FName Name;
		UPROPERTY()
		int Size;
		UPROPERTY()
		TArray<float> Amount;

		void Initialize(FName name, int size) {
			Name = name;
			Size = size;
			Amount.Init(0, FLAT_SIZE(size));
		}

		virtual void ReInitialize(int size) {
			Size = size;
			Amount.Init(0,FLAT_SIZE(size));
			UE_LOG(LogTemp, Warning, TEXT("Reinitialize Called"));
		}

		friend class UResourceMapManager;
	public:
		UStaticLayer():Name(),Size(),Amount() {}
		UStaticLayer(FName name, int size):Name(name), Size(size) {
			Amount.Init(0, FLAT_SIZE(size));
		}

		virtual ~UStaticLayer() = default;
		 UFUNCTION(BlueprintCallable)
			 FName GetName() const {
			return Name;
		}
		 UFUNCTION(BlueprintCallable)
			int GetSize() const {
			return Size;
		}
		UFUNCTION(BlueprintCallable)
		float GetAmount(const int x, const int y) const{
			return Amount[IX(x, y)];
		}
		UFUNCTION(BlueprintCallable)
		void MarkDirty(){
			dirty = true;
		}
		UFUNCTION(BlueprintCallable)
		void MarkClean(){
			dirty = false;
		}
		UFUNCTION(BlueprintCallable)
		bool IsDirty() const{
			return dirty;
		}

		UFUNCTION(BlueprintCallable)
	    TArray<float>& GetAmountArray() {
			return Amount;
		}
		 UFUNCTION(BlueprintCallable)
		void SetAmount(const int x, const int y, const float value) {
			Amount[IX(x, y)] = value;
			MarkDirty();
		}
		 UFUNCTION(BlueprintCallable)
			 float FlatGetAmount(const int i) const{
			return Amount[i];
		}
		 UFUNCTION(BlueprintCallable)
		void FlatSetAmount(const int i, const float value) {
			Amount[i] = value;
			MarkDirty();
		}
	};