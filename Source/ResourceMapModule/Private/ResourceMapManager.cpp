#include "ResourceMapManager.h"
#include "ResourceMapModule.h"

DECLARE_CYCLE_STAT(TEXT("Resource_Map_Manager_Add_Static"), STAT_RMM_ADD_STATIC, STATGROUP_RMM)
DECLARE_CYCLE_STAT(TEXT("Resource_Map_Manager_Add_Dynamic"), STAT_RMM_ADD_DYNAMIC, STATGROUP_RMM)
DECLARE_CYCLE_STAT(TEXT("Resource_Map_Manager_Add_Static_Velocity"), STAT_RMM_ADD_STATIC_VELOCITY, STATGROUP_RMM)
DECLARE_CYCLE_STAT(TEXT("Resource_Map_Manager_Proccess_Pass"), STAT_RMM_PROCESS_PASS, STATGROUP_RMM)
DECLARE_CYCLE_STAT(TEXT("Resource_Map_Manager_Clear"), STAT_RMM_CLEAR, STATGROUP_RMM)
DECLARE_CYCLE_STAT(TEXT("Resource_Map_Manager_ReInitialize"), STAT_RMM_REINIT, STATGROUP_RMM)

const FName UResourceMapManager::ZeroLayerName = FName(TEXT("ZeroLayer"));
const FName UResourceMapManager::NoneVelocityFieldName = FName(TEXT("NoneField"));

void UResourceMapManager::AddStaticLayer(const FName layerName)
{
	SCOPE_CYCLE_COUNTER(STAT_RMM_ADD_STATIC);
	auto layer = NewObject<UStaticLayer>(this/*, FName(TEXT("SL_") + layerName.ToString() + GetNameSafe(this))*/);
	layer->Initialize(layerName, Size);
	NamedStaticLayers.Add(layerName, layer);
}

void UResourceMapManager::AddDynamicLayer(const FName layerName, const float diffuse,const FName associatedGroundLayer=ZeroLayerName, const FName associatedDiffuseMapLayer=ZeroLayerName, const FName associatedVelocityFieldLayer = NoneVelocityFieldName)
{
	SCOPE_CYCLE_COUNTER(STAT_RMM_ADD_DYNAMIC);

	if (!LayerExists(associatedGroundLayer)) {
		UE_LOG(LogTemp, Error, TEXT("Could not find layer named '%s'"), *associatedGroundLayer.ToString());
		return;
	}
	if (!LayerExists(associatedDiffuseMapLayer)) {
		UE_LOG(LogTemp, Error, TEXT("Could not find layer named '%s'"), *associatedDiffuseMapLayer.ToString());
		return;
	}
	if(associatedVelocityFieldLayer!=NoneVelocityFieldName && !VelocityFieldExist(associatedVelocityFieldLayer)){
		UE_LOG(LogTemp, Error, TEXT("Could not find layer named '%s'"), *associatedVelocityFieldLayer.ToString());
		return;
	};

	auto layer = NewObject<UDynamicLayer>(this/*, FName(TEXT("DYNL_") + layerName.ToString() + GetNameSafe(this))*/);
	layer->Initialize(layerName, Size, diffuse, associatedGroundLayer, associatedDiffuseMapLayer, associatedVelocityFieldLayer);
	NamedDynamicLayers.Add(layerName,layer);
	NamedStaticLayers.Add(layerName, layer);
}

void UResourceMapManager::AddStaticVelocityField(const FName layerName) {
	SCOPE_CYCLE_COUNTER(STAT_RMM_ADD_STATIC_VELOCITY);

	auto field = NewObject<UStaticVelocityField>(this/*, FName(TEXT("SVF_") + layerName.ToString() + GetNameSafe(this))*/);
	field->Initialize(layerName, Size);
	NamedStaticVelocityFields.Add(layerName, field);
}

bool UResourceMapManager::LayerExists(const FName LayerName) const
{
	return NamedStaticLayers.Contains(LayerName)||NamedDynamicLayers.Contains(LayerName);
}

bool UResourceMapManager::IsDynamic(const FName LayerName) const
{
	return NamedDynamicLayers.Contains(LayerName);
}

bool UResourceMapManager::VelocityFieldExist(const FName LayerName) const 
{
	return NamedStaticVelocityFields.Contains(LayerName) /*|| NamedDynamicVelocityFields.Contains(LayerName)*/;
}

void UResourceMapManager::ProccessPass(float DeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_RMM_PROCESS_PASS);

	//UE_LOG(LogTemp,Warning,TEXT("Starting process pass"))
	for (auto &proccesor : ProccesPass)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Start Procces"));
		proccesor->Execute_Proccess(proccesor.GetObject(), this, DeltaTime);
		//UE_LOG(LogTemp, Warning, TEXT("End Procces"));
	}
}

void UResourceMapManager::Clear()
{
	SCOPE_CYCLE_COUNTER(STAT_RMM_CLEAR);

	auto zeroLayerCache = NamedStaticLayers[ZeroLayerName];
	NamedStaticLayers.Reset();
	NamedDynamicLayers.Reset();
	NamedStaticVelocityFields.Reset();
	ProccesPass.Empty();
	NamedStaticLayers.Add(ZeroLayerName, zeroLayerCache);
}

void UResourceMapManager::AddProccesorToProccesPass(TScriptInterface<ITimedLayersProccesor>& proccesor)
{
	ProccesPass.Add(proccesor);
}

void UResourceMapManager::LogStats()
{
	UE_LOG(LogTemp, Warning, TEXT("Named Static Layers count - %d"), NamedStaticLayers.Num());
	for (auto& layer : NamedStaticLayers) {
		UE_LOG(LogTemp, Warning, TEXT("Named Static Layer - %s, with size - %d"), *layer.Key.ToString(), layer.Value->Size);
	}
	UE_LOG(LogTemp, Warning, TEXT("Named Dynamic Layers count - %d"), NamedDynamicLayers.Num());
	for (auto& layer : NamedDynamicLayers) {
		UE_LOG(LogTemp, Warning, TEXT("Named Dynamic Layer - %s, with size - %d"), *layer.Key.ToString(), layer.Value->Size);
	}
	UE_LOG(LogTemp, Warning, TEXT("Named Static Velocity Fields count - %d"), NamedStaticVelocityFields.Num());
	for (auto& layer : NamedStaticVelocityFields) {
		UE_LOG(LogTemp, Warning, TEXT("Named Static Velocity Field - %s, with size - %d"), *layer.Key.ToString(), layer.Value->Size);
	}
	UE_LOG(LogTemp, Warning, TEXT("Procces Passes count - %d"), ProccesPass.Num());
	
}

void UResourceMapManager::ReInitialize(int size)
{
	SCOPE_CYCLE_COUNTER(STAT_RMM_REINIT);

	Size = size;
	for (auto &layer : NamedStaticLayers) {
		layer.Value->ReInitialize(size);
	}
	for (auto &layer : NamedDynamicLayers) {
		layer.Value->ReInitialize(size);
	}
	for (auto &layer : NamedStaticVelocityFields) {
		layer.Value->ReInitialize(size);
	}
}

int UResourceMapManager::GetSize() const {
	return Size;
}

