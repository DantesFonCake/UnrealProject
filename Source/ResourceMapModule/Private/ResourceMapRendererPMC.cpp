// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceMapRendererPMC.h"

const float AResourceMapRendererPMC::GroundOffset = 1;
const float AResourceMapRendererPMC::ZeroEpsilon = 1e-4;

// Sets default values
AResourceMapRendererPMC::AResourceMapRendererPMC():Size(128),CellSize(32),drawableLayers()
{
	UE_LOG(LogTemp, Warning, TEXT("Constructor Called"));
	root = /*NewObject<USceneComponent>(this)*/CreateDefaultSubobject<USceneComponent>(FName(TEXT("RootScene_")+GetNameSafe(this)));
	SetRootComponent(root);
	root->RegisterComponent();
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	Mesh = CubeMesh.Object;
	//UE_LOG(LogTemp, Warning, TEXT("Mesh Size %f"), Mesh->GetBounds().GetBox().GetSize().X)
	manager = /*NewObject<UResourceMapManager>(this)*/CreateDefaultSubobject<UResourceMapManager>(FName(TEXT("RM_") + GetNameSafe(this)));
	manager->ReInitialize(Size);
}

void AResourceMapRendererPMC::CreateMeshesForLayer(UDrawableLayer* layer)
{
	for (int j = 0; j < Size; j++)
	{
		for (int i = 0; i < Size; i++)
		{
			layer->GetMesh()->AddInstance(CreateCoordTransform(j,i,0));
		}
	}
}

void AResourceMapRendererPMC::SetSize(int NewSize)
{
	if (NewSize == Size)
		return;
	Size = NewSize;
	for (auto& pair : drawableLayers) {
		pair.Value->ReInitialize(NewSize);
	}
	GetManager()->ReInitialize(NewSize);
}

void AResourceMapRendererPMC::AddLayerDrawable(const FName layerName,bool isDrawable,UMaterialInterface* material)
{
	if (!GetManager()) {
		UE_LOG(LogTemp, Error, TEXT("Manager was null"));
		return;
	}

	if (!GetManager()->LayerExists(layerName)) {
		UE_LOG(LogTemp, Error, TEXT("Could not find layer named '%s' in the associeted manager"), *layerName.ToString());
		return;
	}

	if (drawableLayers.Contains(layerName)) {
		UE_LOG(LogTemp, Warning, TEXT("'%s' layer already exist"), *layerName.ToString());
		return;
	}
	auto mesh = NewObject<UInstancedStaticMeshComponent>(this/*, FName(TEXT("ISMC_")+layerName.ToString() + GetNameSafe(this))*/);
	mesh->SetWorldTransform(this->GetTransform());
	mesh->SetStaticMesh(Mesh);
	if(material)
		mesh->SetMaterial(0,material->GetMaterial());

	//mesh->AttachTo(RootComponent);
	AddInstanceComponent(mesh);
	mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	mesh->RegisterComponent();
	auto layer = NewObject<UDrawableLayer>(this/*, FName(TEXT("DL_") + layerName.ToString() + GetNameSafe(this))*/);
	layer->Initialize(Size, mesh,isDrawable,GetManager()->IsDynamic(layerName));
	//UE_LOG(LogTemp, Warning, TEXT("Layer created %d"), layer->GetSize());
	//UE_LOG(LogTemp, Warning, TEXT("ground offset %f"), GroundOffset);
	CreateMeshesForLayer(layer);
	drawableLayers.Add(layerName, layer);
}

void AResourceMapRendererPMC::SetLayerDrawable(const FName layerName, bool visible)
{
	if (!drawableLayers.Contains(layerName)) {
		UE_LOG(LogTemp, Error, TEXT("Could not find layer named '%s'"), *layerName.ToString());
		return;
	}
	drawableLayers[layerName]->SetDrawable(visible);
}

FTransform AResourceMapRendererPMC::CreateCoordTransform(int x, int y, float height, float ground) {
	const auto meshSize = Mesh->GetBounds().GetBox().GetSize();
	const auto cellSizeFraction = CellSize / meshSize.X;
	const auto topLeft = -CellSize * (Size - 1) / 2.0f;
	const float cX = topLeft + x * CellSize;
	const float cY = topLeft + y * CellSize;
	float sZ = height/ meshSize.X;
	if(FMath::IsNearlyEqual(sZ,0,ZeroEpsilon))
		return FTransform(FRotator(0), FVector(cX, cY, sZ*meshSize.X/2+ground-GroundOffset), FVector(cellSizeFraction, cellSizeFraction, 0));
	return FTransform(FRotator(0), FVector(cX, cY, sZ*meshSize.X/2+ground), FVector(cellSizeFraction, cellSizeFraction, sZ));
}


#define IX_INTERNAL(x,y) ((x)+(y)*Size)
void AResourceMapRendererPMC::UpdateFromManager()
{
	for (auto& pair : drawableLayers) {
		auto layerName = pair.Key;
		if (!GetManager()->LayerExists(layerName)) {
			UE_LOG(LogTemp, Error, TEXT("Could not find layer named '%s' in the associeted manager"), *layerName.ToString());
			continue;
		}
		if (!drawableLayers.Contains(layerName)) {
			UE_LOG(LogTemp, Error, TEXT("Could not find layer named '%s'"), *layerName.ToString());
			continue;
		}
		auto dLayer = pair.Value;
		if (!dLayer->IsDrawable())
			continue;
		auto layerMesh = dLayer->GetMesh();
		auto layer = GetManager()->GetNamedStaticLayer(layerName);
		if (!layer->IsDirty())
			continue;
		UStaticLayer* ground;
		if (dLayer->IsDynamic()) {
			ground = GetManager()->GetNamedStaticLayer(static_cast<UDynamicLayer*>(layer)->GetAssociatedGroundName());
		}
		else {
			ground = GetManager()->GetNamedStaticLayer(GetManager()->GetZeroLayerName());
		}
		

		//UE_LOG(LogTemp, Warning, TEXT("Updating layer %s"), *layerName.ToString());
		//UE_LOG(LogTemp, Warning, TEXT("Updating from layer with size %d"), layer->GetSize());
		//UE_LOG(LogTemp, Warning, TEXT("Updating from ground with name %s with size %d"), *ground->GetName().ToString(), ground->GetSize());
		//UE_LOG(LogTemp, Warning, TEXT("Updating mesh with size %d"), layerMesh->GetInstanceCount());

		for (int x = 0; x < Size; x++)
		{
			for (int y = 0; y < Size; y++)
			{
				layerMesh->UpdateInstanceTransform(IX_INTERNAL(x, y), CreateCoordTransform(x, y, layer->GetAmount(x+1, y+1),ground->GetAmount(x+1,y+1)), false,false, true);
			}
		}
		layerMesh->MarkRenderStateDirty();
		layer->MarkClean();
	}
}

void AResourceMapRendererPMC::LogStats()
{
	UE_LOG(LogTemp, Warning, TEXT("Mesh Size %f"), Mesh->GetBounds().GetBox().GetSize().X);
	UE_LOG(LogTemp, Warning, TEXT("Size %d"), Size);
	UE_LOG(LogTemp, Warning, TEXT("Drawable layers count: %d"), drawableLayers.Num());
	for (auto& pair:drawableLayers)
	{
		auto name = pair.Key;
		UE_LOG(LogTemp, Warning, TEXT("Layer: name - %s, mesh count - %d, internal layer count - %d, layer count - %d"), *name.ToString(), pair.Value->GetMesh()->GetInstanceCount(), pair.Value->GetSize(), GetManager()->GetNamedStaticLayer(name)->GetSize());
	}
	manager->LogStats();
}

void AResourceMapRendererPMC::Clear()
{
	if (manager) {
		manager->Clear();
	}
	for (auto& layer : drawableLayers) {
		layer.Value->GetMesh()->DestroyComponent(true);
	}
	drawableLayers.Reset();
}
