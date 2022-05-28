// Fill out your copyright notice in the Description page of Project Settings.

#include "DiffuseLayerProccesor.h"
#include "ResourceMapManager.h"

#define FLAT_SIZE(s) ((s)+2)*((s)+2)
void UDiffuseLayerProccesor::Proccess_Implementation(UResourceMapManager* Manager, float DeltaTime)
{
	//UE_LOG(LogTemp, Warning, TEXT("Start Procces internal"));

	auto layer = Manager->GetNamedDynamicLayer(LayerToProcces); 
	int Size = Manager->GetSize();
	auto ground = Manager->GetNamedStaticLayer(layer->GetAssociatedGroundName());
	auto diffuseMap = Manager->GetNamedStaticLayer(layer->GetAssociatedDiffuseMapName());
	/*for (auto& elem : layer->GetAmountArray()) {
		UE_LOG(LogTemp, Warning, TEXT("%f"), elem);
	}*/
	SetBound<0>(Size, layer->GetAmountArray());
	SetBound<0>(Size, ground->GetAmountArray());
	SetBound<0>(Size, diffuseMap->GetAmountArray());
	Diffuse(Size, DeltaTime, layer->GetDiffuse(), layer->GetAmountArray(), layer->GetFutureArray(), ground->GetAmountArray(), diffuseMap->GetAmountArray());
	if (layer->GetAssociatedVelocityFieldLayer() != Manager->GetNoneVelocityFieldName()) {
		auto velocityField = Manager->GetNamedStaticVelocityField(layer->GetAssociatedVelocityFieldLayer());
		Advect(Size, DeltaTime, layer->GetFutureArray(), layer->GetAmountArray(), ground->GetAmountArray(), velocityField->GetUArray(), velocityField->GetVArray());
		return;
	}
	FMemory::Memcpy(layer->GetAmountArray().GetData(), layer->GetFutureArray().GetData(), FLAT_SIZE(Size)*sizeof(float));
	layer->MarkDirty();
	//UE_LOG(LogTemp, Warning, TEXT("End Procces internal"));

}


#define IX(x,y) ((y) * (Size+2) + (x))
#define FOR_EACH_CELL for ( i=1 ; i<=Size ; i++ ) { for ( j=1 ; j<=Size ; j++ ) {
#define END_FOR }}
#define GAUSS_CLAMP(s,x1,y1) if((s)>=0) { if((s)<=X[IX(i,j)]){ totalNet-=GroundLevel[IX(i,j)]-GroundLevel[IX((x1),(y1))];} c+=a;}
#define GAUSS_CLAMP_D(s,x1,y1,d) if((s)>=0) { if((s)<=X[IX(i,j)]){ totalNet-=(GroundLevel[IX(i,j)]-GroundLevel[IX((x1),(y1))])*d;} c+=a*d;}

void UDiffuseLayerProccesor::Diffuse(int Size,float DeltaTime, float Diffuse,TArray<float>& X0, TArray<float>& X,
	const TArray<float>& GroundLevel, const TArray<float>& DiffuseMap)
{
	int i, j;
	const float a = DeltaTime * Diffuse * Size * Size;
	for (int k = 0; k < 20; k++)
	{
#pragma omp parallel for private(i)
		for (i = 1; i <= Size; i++) {
#pragma omp parallel for private(j)
			for (j = 1; j <= Size; j++) {
				float c = 1;
				float totalNet = FMath::Clamp<float>(PushDiff(Size, i - 1, j, i, j, X, GroundLevel), 0, X[IX(i - 1, j)])
					* (1 - DiffuseMap[IX(i - 1, j)])
					+ FMath::Clamp<float>(PushDiff(Size, i + 1, j, i, j, X, GroundLevel), 0, X[IX(i + 1, j)])
					* (1 - DiffuseMap[IX(i + 1, j)])
					+ FMath::Clamp<float>(PushDiff(Size, i, j - 1, i, j, X, GroundLevel), 0, X[IX(i, j - 1)])
					* (1 - DiffuseMap[IX(i, j - 1)])
					+ FMath::Clamp<float>(PushDiff(Size, i, j + 1, i, j, X, GroundLevel), 0, X[IX(i, j + 1)])
					 * (1 - DiffuseMap[IX(i, j + 1)]);

				const float d = 1 -DiffuseMap[IX(i, j)];
				const float lFrom = PushDiff(Size, i, j, i - 1, j, X, GroundLevel);
				GAUSS_CLAMP_D(lFrom, i - 1, j, d);
				const float rFrom = PushDiff(Size, i, j, i + 1, j, X, GroundLevel);
				GAUSS_CLAMP_D(rFrom, i + 1, j, d);
				const float uFrom = PushDiff(Size, i, j, i, j - 1, X, GroundLevel);
				GAUSS_CLAMP_D(uFrom, i, j - 1, d);
				const float dFrom = PushDiff(Size, i, j, i, j + 1, X, GroundLevel);
				GAUSS_CLAMP_D(dFrom, i, j + 1, d);

				X[IX(i, j)] = (X0[IX(i, j)] + a * totalNet) / c;
		END_FOR
		SetBound<0>(Size, X);
	}
}
		void UDiffuseLayerProccesor::Advect(int Size, float DeltaTime, TArray<float>& X0, TArray<float>& X, const TArray<float>& GroundLevel, const TArray<float>& U, const TArray<float>& V) {

		}

		inline float UDiffuseLayerProccesor::PushDiff(int Size, const int x, const int y, const int x1, const int y1,
			const TArray<float>& Source,
			const TArray<float>& GroundLevel)
		{
			const float xSource = Source[IX(x, y)];
			const float diff = (xSource + GroundLevel[IX(x, y)]) - GroundLevel[IX(x1, y1)];
			return diff;
		}

		template<int BoundType>
		inline void UDiffuseLayerProccesor::SetBound(int Size, TArray<float>& Field)
		{
			for (int i = 1; i <= Size; i++)
			{
				Field[IX(0, i)] = BoundType == 1 ? -Field[IX(1, i)] : Field[IX(1, i)];
				Field[IX(Size + 1, i)] = BoundType == 1
					? -Field[IX(Size, i)]
					: Field[IX(Size, i)];
				Field[IX(i, 0)] = BoundType == 2 ? -Field[IX(i, 1)] : Field[IX(i, 1)];
				Field[IX(i, Size + 1)] = BoundType == 2
					? -Field[IX(i, Size)]
					: Field[IX(i, Size)];
			}
			Field[IX(0, 0)] = 0.5f * (Field[IX(1, 0)] + Field[IX(0, 1)]);
			Field[IX(0, Size + 1)] = 0.5f * (Field[IX(1, Size + 1)] + Field[
				IX(0, Size)]);
			Field[IX(Size + 1, 0)] = 0.5f * (Field[IX(Size, 0)] + Field[IX(
				Size + 1, 1)]);
			Field[IX(Size + 1, Size + 1)] = 0.5f * (Field[IX(Size, Size + 1)] + Field[
				IX(Size + 1, Size)]);
		}