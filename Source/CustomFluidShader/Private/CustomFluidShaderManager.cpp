// Fill out your copyright notice in the Description page of Project Settings.
#define NUM_THREADS_PER_GROUP_DIMENSION 32

#include "CustomFluidShaderManager.h"

#include "GlobalShader.h"
#include "ShaderParameterStruct.h"
#include "RenderGraphUtils.h"
#include "RenderTargetPool.h"

class DiffuseShader : public FGlobalShader
{
public:
	//Declare this class as a global shader
	DECLARE_GLOBAL_SHADER(DiffuseShader);
	//Tells the engine that this shader uses a structure for its parameters
	SHADER_USE_PARAMETER_STRUCT(DiffuseShader, FGlobalShader);
	/// <summary>
	/// DECLARATION OF THE PARAMETER STRUCTURE
	/// The parameters must match the parameters in the HLSL code
	/// For each parameter, provide the C++ type, and the name (Same name used in HLSL code)
	/// </summary>
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<float>, X)
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<float>, X0)
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<float>, DiffuseMap)
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<float>, Ground)
		//SHADER_PARAMETER_RDG_UNIFORM_BUFFER(INT, Size)
		//SHADER_PARAMETER_RDG_UNIFORM_BUFFER(float, DiffuseStat)
		//SHADER_PARAMETER_RDG_BUFFER_SRV(AdditionalInfo, Info)
		SHADER_PARAMETER(int, Size)
		SHADER_PARAMETER(float, DiffuseStat)
		END_SHADER_PARAMETER_STRUCT()

public:
	//Called by the engine to determine which permutations to compile for this shader
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	//Modifies the compilations environment of the shader
	static inline void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);

		//We're using it here to add some preprocessor defines. That way we don't have to change both C++ and HLSL code when we change the value for NUM_THREADS_PER_GROUP_DIMENSION
		OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_X"), NUM_THREADS_PER_GROUP_DIMENSION);
		OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Y"), NUM_THREADS_PER_GROUP_DIMENSION);
		OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Z"), 1);
	}

};


// This will tell the engine to create the shader and where the shader entry point is.
//                        ShaderType              ShaderPath             Shader function name    Type
IMPLEMENT_GLOBAL_SHADER(DiffuseShader, "/CustomShaders/DiffuseShader.usf", "MainComputeShader", SF_Compute);

class BoundaryShader : public FGlobalShader
{
public:
	//Declare this class as a global shader
	DECLARE_GLOBAL_SHADER(BoundaryShader);
	//Tells the engine that this shader uses a structure for its parameters
	SHADER_USE_PARAMETER_STRUCT(BoundaryShader, FGlobalShader);
	/// <summary>
	/// DECLARATION OF THE PARAMETER STRUCTURE
	/// The parameters must match the parameters in the HLSL code
	/// For each parameter, provide the C++ type, and the name (Same name used in HLSL code)
	/// </summary>
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<float>, X)
		//SHADER_PARAMETER_RDG_UNIFORM_BUFFER(INT, Size)
		//SHADER_PARAMETER_RDG_UNIFORM_BUFFER(float, DiffuseStat)
		//SHADER_PARAMETER_RDG_BUFFER_SRV(AdditionalInfo, Info)
		SHADER_PARAMETER(int, Size)
		SHADER_PARAMETER(int, BoundType)
		END_SHADER_PARAMETER_STRUCT()

public:
	//Called by the engine to determine which permutations to compile for this shader
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	//Modifies the compilations environment of the shader
	static inline void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);

		//We're using it here to add some preprocessor defines. That way we don't have to change both C++ and HLSL code when we change the value for NUM_THREADS_PER_GROUP_DIMENSION
		OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_X"), NUM_THREADS_PER_GROUP_DIMENSION);
		OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Y"), NUM_THREADS_PER_GROUP_DIMENSION);
		OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Z"), 1);
	}

};


// This will tell the engine to create the shader and where the shader entry point is.
//                        ShaderType              ShaderPath             Shader function name    Type
IMPLEMENT_GLOBAL_SHADER(BoundaryShader, "/CustomShaders/BoundaryShader.usf", "MainComputeShader", SF_Compute);


//Static members
CustomFluidShaderManager* CustomFluidShaderManager::instance = nullptr;

#define MCreateSRV(x, to)\
	l = x.Num();\
\
	FRDGBufferRef buffer##x = CreateStructuredBuffer(graph, TEXT(#x), sizeof(float),l ,x.GetData(),l*sizeof(float));\
	to = graph.CreateSRV(buffer##x, EPixelFormat::PF_R32_UINT);

#define MCreateUAV(x,to,output)\
	l = x.Num();\
\
	output = CreateStructuredBuffer(graph, TEXT(#x), sizeof(float),l ,x.GetData(),l*sizeof(float),ERDGInitialDataFlags::NoCopy);\
	to = graph.CreateUAV(output, EPixelFormat::PF_R32_UINT);

void CustomFluidShaderManager::DispatchInternal(FRHICommandListImmediate& cmd, const TArray<float>& x, const TArray<float>& x0, const TArray<float>& ground, const TArray<float>& diffuseMap, float diffuseProps, int size)
{
#define IX(x,y) (x+y*(size+2))
	check(IsInRenderingThread());
	FRDGBuilder graph(cmd);
	//FRHICommandListImmediate& cmd = GRHICommandList.GetImmediateCommandList();
	DiffuseShader::FParameters* diffuseParams = graph.AllocParameters<DiffuseShader::FParameters>();
	BoundaryShader::FParameters* boundaryParams = graph.AllocParameters<BoundaryShader::FParameters>();
	int l;
	
	
	MCreateSRV(x0,diffuseParams->X0);
	FRDGBufferRef output;
	MCreateUAV(x, diffuseParams->X, output);
	boundaryParams->X = diffuseParams->X;
	MCreateSRV(ground, diffuseParams->Ground);
	MCreateSRV(diffuseMap, diffuseParams->DiffuseMap);
	diffuseParams->Size = size;
	boundaryParams->Size = size;
	boundaryParams->BoundType = 0;
	diffuseParams->DiffuseStat = diffuseProps;

	TShaderMapRef<DiffuseShader> diffuseShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	TShaderMapRef<BoundaryShader> boundaryShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	auto groups = FMath::DivideAndRoundUp(size, NUM_THREADS_PER_GROUP_DIMENSION);
	auto memsize = lastResults.Num()*sizeof(float);//buf->GetSize();
	for (int i = 0; i < 20; i++) {

		FComputeShaderUtils::AddPass(graph, RDG_EVENT_NAME("Diffuse Shader"), diffuseShader, diffuseParams, 
			{ groups,
				groups,
				1 });
		/*FComputeShaderUtils::AddPass(graph, RDG_EVENT_NAME("Boundary Shader"), boundaryShader, boundaryParams,
			{ groups,
				1,
				1 });*/
		
	}
	TRefCountPtr<FRDGPooledBuffer> pooled;
	graph.QueueBufferExtraction(output, &pooled);

	graph.Execute();
	FRHIStructuredBuffer* buf = pooled.GetReference()->GetStructuredBufferRHI();
	auto res2 = (float*)cmd.LockStructuredBuffer(buf, 0, memsize, EResourceLockMode::RLM_ReadOnly);
	FMemory::Memcpy(lastResults.GetData(),res2,memsize);
	cmd.UnlockStructuredBuffer(buf);
}

void CustomFluidShaderManager::Dispatch(TArray<float>& x, const TArray<float>& x0, const TArray<float> ground, const TArray<float> diffuseMap, float diffuseProps,int size)
{
	auto lambda = [&](FRHICommandListImmediate& cmd)
	{
		this->DispatchInternal(cmd, x, x0, ground, diffuseMap, diffuseProps, size);
	};
	ENQUEUE_RENDER_COMMAND(Balancer)(lambda);
	ReleaseResourcesFence.BeginFence();
	ReleaseResourcesFence.Wait();
}


