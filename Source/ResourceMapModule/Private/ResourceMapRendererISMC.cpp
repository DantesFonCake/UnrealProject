// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceMapRendererISMC.h"

DECLARE_CYCLE_STAT(TEXT("Resource_Map_Renderer_Create_Mesh"), STAT_RMR_CREATE_MESH, STATGROUP_RMM)
DECLARE_CYCLE_STAT(TEXT("Resource_Map_Renderer_Add_Layer"), STAT_RMR_ADD_LAYER, STATGROUP_RMM)
DECLARE_CYCLE_STAT(TEXT("Resource_Map_Renderer_Create_Coords"), STAT_RMR_CREATE_COORDS, STATGROUP_RMM)
DECLARE_CYCLE_STAT(TEXT("Resource_Map_Renderer_Update"), STAT_RMR_UPDATE, STATGROUP_RMM)
DECLARE_CYCLE_STAT(TEXT("Resource_Map_Renderer_Clear"), STAT_RMR_CLEAR, STATGROUP_RMM)

const float AResourceMapRendererISMC::GroundOffset = 1;
const float AResourceMapRendererISMC::ZeroEpsilon = 5e-3;

// Sets default values
AResourceMapRendererISMC::AResourceMapRendererISMC():Size(0),CellSize(0),drawableLayers(),bInitialized(false)
{
	UE_LOG(LogTemp, Warning, TEXT("Constructor Called"));
	root = CreateDefaultSubobject<USceneComponent>(FName(TEXT("RootScene_")+GetNameSafe(this)));
	SetRootComponent(root);
	root->RegisterComponent();
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	Mesh = CubeMesh.Object;
}

void AResourceMapRendererISMC::CreateMeshesForLayer(UDrawableLayer* layer)
{
	SCOPE_CYCLE_COUNTER(STAT_RMR_CREATE_MESH);
	check(bInitialized);
	for (int j = 0; j < Size; j++)
	{
		for (int i = 0; i < Size; i++)
		{
			layer->AddInstance(CreateCoordTransform(i,j,0));
		}
	}
}

void AResourceMapRendererISMC::Initialize(UResourceMapManager* m, int s, float cS)
{
	Size = s;
	CellSize = cS;
	manager = m;

	bInitialized = true;
}

void AResourceMapRendererISMC::AddLayerDrawable(const FName layerName,bool isDrawable,UMaterialInterface* material,int NumCustomDataFloats)
{
	SCOPE_CYCLE_COUNTER(STAT_RMR_ADD_LAYER);
	check(bInitialized);
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
	auto mesh = NewObject<UInstancedStaticMeshComponent>(this);
	mesh->SetWorldTransform(this->GetTransform());
	mesh->SetStaticMesh(Mesh);
	mesh->bCastDynamicShadow = false;
	if(material)
		mesh->SetMaterial(0,material);

	//mesh->AttachTo(RootComponent);
	mesh->NumCustomDataFloats = NumCustomDataFloats;
	mesh->SetupAttachment(RootComponent);
	mesh->RegisterComponent();
	AddInstanceComponent(mesh);
	auto layer = NewObject<UDrawableLayer>(this);
	layer->Initialize(Size, mesh,isDrawable,GetManager()->IsDynamic(layerName));
	CreateMeshesForLayer(layer);
	drawableLayers.Add(layerName, layer);
}

void AResourceMapRendererISMC::SetLayerDrawable(const FName layerName, bool visible)
{
	check(bInitialized);
	if (!drawableLayers.Contains(layerName)) {
		UE_LOG(LogTemp, Error, TEXT("Could not find layer named '%s'"), *layerName.ToString());
		return;
	}
	drawableLayers[layerName]->SetDrawable(visible);
}

FTransform AResourceMapRendererISMC::CreateCoordTransform(int x, int y, float height, float ground) {
	SCOPE_CYCLE_COUNTER(STAT_RMR_CREATE_COORDS);
	check(bInitialized);
	const auto meshSize = Mesh->GetBounds().GetBox().GetSize();
	const auto cellSizeFraction = CellSize / meshSize.X;
	const auto topLeft = -CellSize * (Size - 1) / 2.0f;
	const float cX = topLeft + x * CellSize;
	const float cY = topLeft + y * CellSize;
	float sZ = height/ meshSize.X;
	if(FMath::IsNearlyEqual(sZ,0,ZeroEpsilon))
		return FTransform(FRotator(0), FVector(cX, cY, height/2+ground-GroundOffset), FVector(cellSizeFraction, cellSizeFraction, 0));
	return FTransform(FRotator(0), FVector(cX, cY, height/2+ground), FVector(cellSizeFraction, cellSizeFraction, sZ));
}


#define IX_INTERNAL(x,y) ((x)+(y)*Size)
void AResourceMapRendererISMC::UpdateFromManager()
{
	SCOPE_CYCLE_COUNTER(STAT_RMR_UPDATE);
	check(bInitialized);
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

		auto preRenderPass = preRenderPasses.Find(layerName);
		if (preRenderPass)
			(*preRenderPass)->Execute_Proccess(preRenderPass->GetObject(), this, dLayer);

		for (int y = 0; y < Size; y++)
		{
			for (int x = 0; x < Size; x++)
			{
				auto t = CreateCoordTransform(x, y, layer->GetAmount(x + 1, y + 1), ground->GetAmount(x + 1, y + 1));
				dLayer->SetInstanceTransform(IX_INTERNAL(x, y), t);
			}
		}

		dLayer->MeshUpdate();
		layer->MarkClean();
	}
}

void AResourceMapRendererISMC::AddToPreRenderPass(const FName layerName, const TScriptInterface<IPreRenderLayersProccesor>& pass)
{
	check(bInitialized);
	preRenderPasses.Add(layerName, pass);
}

UDrawableLayer* AResourceMapRendererISMC::GetLayer(const FName name)
{
	check(bInitialized);
	return drawableLayers[name];
}

void AResourceMapRendererISMC::LogStats()
{
	check(bInitialized);
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

void AResourceMapRendererISMC::Clear()
{
	SCOPE_CYCLE_COUNTER(STAT_RMR_CLEAR);
	check(bInitialized);
	/*if (manager) {
		manager->Clear();
	}*/
	for (auto& layer : drawableLayers) {
		layer.Value->Clear();
	}
	drawableLayers.Reset();
	preRenderPasses.Reset();
}
