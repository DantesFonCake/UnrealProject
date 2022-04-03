// Fill out your copyright notice in the Description page of Project Settings.


#include "UResourceMap.h"

void UResourceMap::Initialize(const int NewWidth, const int NewHeight,
                              const FMyResourceProperties NewResourceProperties,
                              const bool IsOscillateLeft, const float NewOscillationAmplitude,
                              const BoundaryCondition NewBoundaryCondition, const bool IsUsingResourceMax,
                              const float NewResourceMax, const float NewOscillationFrequency)
{
	Width = NewWidth;
	Height = NewHeight;
	bOscillateLeft = IsOscillateLeft;
	OscillationAmplitude = NewOscillationAmplitude;
	OscillationFrequency = NewOscillationFrequency;
	Boundary = NewBoundaryCondition;
	bUseResourceMax = IsUsingResourceMax;
	ResourceMax = NewResourceMax;
	ResourceProperties = NewResourceProperties;
	InitializeLevel(0);
	Phi2.Init(0, Width * Height);
	Psi2.Init(0, Width * Height);
	IsInitialized = true;
}

// Sets default values
UResourceMap::UResourceMap(): IsInitialized(false), bOscillateLeft(false),
                              OscillationAmplitude(0), OscillationFrequency(0),
                              Boundary(Absorbing), bUseResourceMax(false),
                              ResourceMax(0), Width(16), Height(16)
{
	Initialize(Width,Height,ResourceProperties,bOscillateLeft,
		OscillationAmplitude,Boundary,bUseResourceMax,ResourceMax,OscillationFrequency);
}

constexpr int UResourceMap::InlineCoords(const int X, const int Y) const
{
	return Y * Width + X;
}

void UResourceMap::SetResourceLevel(const int X, const int Y, const float NewLevel)
{
	At(X, Y) = NewLevel;
	PsiAt(X, Y) = NewLevel;
}

float& UResourceMap::At(const int X, const int Y)
{
	//UE_LOG(LogTemp,Warning,TEXT("Accessing tile at {%d, %d}"),x,y);
	return Phi[InlineCoords(X, Y)];
}

const float& UResourceMap::At(const int X, const int Y) const
{
	//UE_LOG(LogTemp,Warning,TEXT("Accessing const tile at {%d, %d}"),x,y);
	return Phi[InlineCoords(X, Y)];
}

const float& UResourceMap::PsiAt(const int X, const int Y) const
{
	return Psi[InlineCoords(X, Y)];
}

float& UResourceMap::PsiAt(const int X, const int Y)
{
	return Psi[InlineCoords(X, Y)];
}

void UResourceMap::AddResourceLevel(const int X, const int Y, const float AddLevel)
{
	At(X, Y) += AddLevel;
	PsiAt(X, Y) += AddLevel;
}

void UResourceMap::ClearDelta()
{
	int32 length = Psi.Num();
	Psi.Init(0, Height * Width);
	return;
	for (int i = 0; i < length; ++i)
	{
		Psi[i] *= 0.9f;
	}
}

void UResourceMap::FillSingeCellDelta(const int X, const int Y, const float DeltaTime)
{
	const auto cLevel = At(X, Y);
	const auto maxFlow = ResourceProperties.FlowSpeed * DeltaTime * cLevel;
	const auto xMin = X - 1 >= 0 ? X - 1 : X;
	const auto yMin = Y - 1 >= 0 ? Y - 1 : Y;
	const auto xMax = X + 1 < Width ? X + 1 : X;
	const auto yMax = Y + 1 < Height ? Y + 1 : Y;
	const char distribution = (xMax - xMin + 1) * (yMax - yMin + 1) - 1;
	if (distribution == 0)
	{
		return;
	}
	const auto transfer = maxFlow / distribution;
	for (int yOffset = yMin; yOffset <= yMax; ++yOffset)
	{
		for (int xOffset = xMin; xOffset <= xMax; ++xOffset)
		{
			if (xOffset == X && yOffset == Y)
			{
				continue;
			}
			PsiAt(xOffset, yOffset) += transfer;
			PsiAt(X, Y) -= transfer;
		}
	}
}

void UResourceMap::SetAreaResourceLevel(const int X, const int Y, const int AreaWidth, const int AreaHeight,
                                        const float NewLevel)
{
	const auto xMax = X + AreaWidth;
	const auto yMax = Y + AreaHeight;
	for (auto j = 0; j < yMax; ++j)
	{
		for (int i = X; i < xMax; ++i)
		{
			At(i, j) = NewLevel;
			PsiAt(i, j) = NewLevel;
		}
	}
}

void UResourceMap::AddAreaResourceLevel(const int X, const int Y, const int AreaWidth, const int AreaHeight,
                                        const float AddLevel)
{
	const auto xMax = X + AreaWidth;
	const auto yMax = Y + AreaHeight;
	for (auto j = 0; j < yMax; ++j)
	{
		for (int i = X; i < xMax; ++i)
		{
			At(i, j) += AddLevel;
			PsiAt(i, j) += AddLevel;
		}
	}
}

void UResourceMap::EqualDistributionRebalance(const float DeltaTime)
{
	if (DeltaTime <= 0.016)
	{
		return;
	}
	for (auto y = 0; y < Height; ++y)
	{
		for (auto x = 0; x < Width; ++x)
		{
			FillSingeCellDelta(x, y, DeltaTime);
		}
	}
	for (auto y = 0; y < Height; ++y)
	{
		for (auto x = 0; x < Width; ++x)
		{
			At(x, y) += PsiAt(x, y);
		}
	}
	ClearDelta();
}

void UResourceMap::WaveRebalance(float DeltaTime)
{
	evolve_wave_half(Phi, Psi, Phi2, Psi2);
	evolve_wave_half(Phi2, Psi2, Phi, Psi);
}

constexpr bool UResourceMap::IsInRange(const int X, const int Y) const
{
	return X >= 0 && X < Width && Y >= 0 && Y < Height;
}

void UResourceMap::InitializeLevel(const float Level)
{
	const auto length = Width * Height;
	Phi.Init(Level, length);
	Psi.Init(0, length);
}

void UResourceMap::evolve_wave_half(TArray<float>& phi_in, TArray<float>& psi_in, TArray<float>& phi_out,
                                    TArray<float>& psi_out)
/* time step of field evolution */
/* phi is value of field at time t, psi at time t-1 */
{
	int i, j, iplus, iminus, jmid = Height;
	float delta, x, y;
	static long time = 0;
	time++;

#pragma omp parallel for private(i,j,iplus,iminus,delta,x,y)
	/* evolution in the bulk */
	for (i = 1; i < Width - 1; i++)
	{
		for (j = 1; j < jmid - 1; j++)
		{
			x = phi_in[InlineCoords(i, j)];
			y = psi_in[InlineCoords(i, j)];

			/* discretized Laplacian */
			delta = phi_in[InlineCoords(i + 1, j)] + phi_in[InlineCoords(i - 1, j)]
				+ phi_in[InlineCoords(i, j + 1)] + phi_in[InlineCoords(i, j - 1)] - 4.0 * x;

			/* evolve phi */
			phi_out[InlineCoords(i, j)] = -y + 2 * x + ResourceProperties.Courant2 * delta - ResourceProperties.Kappa *
				x - ResourceProperties.Gamma * (x - y);
			psi_out[InlineCoords(i, j)] = x;
		}
	}

	/* left boundary */
	if (bOscillateLeft)
	{
		for (j = 1; j < jmid; j++)
		{
			phi_out[InlineCoords(0, j)] = OscillationAmplitude * FMath::Cos(
				static_cast<float>(time) * OscillationFrequency);
		}
	}
	else
	{
		for (j = 1; j < jmid - 1; j++)
		{
			x = phi_in[InlineCoords(0, j)];
			y = psi_in[InlineCoords(0, j)];
			switch (Boundary.GetValue())
			{
			case (Dirichlet):
				{
					delta = phi_in[InlineCoords(1, j)] + phi_in[InlineCoords(0, j + 1)] + phi_in[InlineCoords(0, j - 1)]
						- 3.0 * x;
					phi_out[InlineCoords(0, j)] = -y + 2 * x + ResourceProperties.Courant2 * delta
						- ResourceProperties.Kappa * x - ResourceProperties.Gamma * (x - y);
					break;
				}
			case (Periodic):
				{
					delta = phi_in[InlineCoords(1, j)] + phi_in[InlineCoords(Width - 1, j)]
						+ phi_in[InlineCoords(0, j + 1)] + phi_in[InlineCoords(0, j - 1)] - 4.0 * x;
					phi_out[InlineCoords(0, j)] = -y + 2 * x + ResourceProperties.Courant2 * delta
						- ResourceProperties.Kappa * x - ResourceProperties.Gamma * (x - y);
					break;
				}
			case (Absorbing):
				{
					delta = phi_in[InlineCoords(1, j)] + phi_in[InlineCoords(0, j + 1)] + phi_in[InlineCoords(0, j - 1)]
						- 3.0 * x;
					phi_out[InlineCoords(0, j)] = x - ResourceProperties.Courant * (x - phi_in[InlineCoords(1, j)])
						- ResourceProperties.Kappa_Absorbing * x - ResourceProperties.Gamma_Absorbing * (x - y);
					break;
				}
			case (Vper_Habs):
				{
					delta = phi_in[InlineCoords(1, j)] + phi_in[InlineCoords(0, j + 1)]
						+ phi_in[InlineCoords(0, j - 1)] - 3.0 * x;
					phi_out[InlineCoords(0, j)] = x - ResourceProperties.Courant * (x - phi_in[InlineCoords(1, j)])
						- ResourceProperties.Kappa_Absorbing * x - ResourceProperties.Gamma_Absorbing * (x - y);
					break;
				}
			}
			psi_out[InlineCoords(0, j)] = x;
		}
	}

	/* right boundary */
	for (j = 1; j < jmid - 1; j++)
	{
		x = phi_in[InlineCoords(Width - 1, j)];
		y = psi_in[InlineCoords(Width - 1, j)];

		switch (Boundary)
		{
		case (Dirichlet):
			{
				delta = phi_in[InlineCoords(Width - 2, j)]
					+ phi_in[InlineCoords(Width - 1, j + 1)] + phi_in[InlineCoords(Width - 1, j - 1)] - 3.0 * x;
				phi_out[InlineCoords(Width - 1, j)] = -y + 2 * x + ResourceProperties.Courant2 * delta
					- ResourceProperties.Kappa * x - ResourceProperties.Gamma *
					(x - y);
				break;
			}
		case (Periodic):
			{
				delta = phi_in[InlineCoords(Width - 2, j)] + phi_in[InlineCoords(0, j)]
					+ phi_in[InlineCoords(Width - 1, j + 1)] + phi_in[InlineCoords(Width - 1, j - 1)] -
					4.0 * x;
				phi_out[InlineCoords(Width - 1, j)] = -y + 2 * x + ResourceProperties.Courant2 * delta
					- ResourceProperties.Kappa * x - ResourceProperties.Gamma *
					(x - y);
				break;
			}
		case (Absorbing):
			{
				delta = phi_in[InlineCoords(Width - 2, j)] + phi_in[InlineCoords(Width - 1, j + 1)]
					+ phi_in[InlineCoords(Width - 1, j - 1)] - 3.0 * x;
				phi_out[InlineCoords(Width - 1, j)] = x - ResourceProperties.Courant * (x - phi_in[InlineCoords(
						Width - 2, j)])
					- ResourceProperties.Kappa_Absorbing * x -
					ResourceProperties.Gamma_Absorbing * (x - y);
				break;
			}
		case (Vper_Habs):
			{
				delta = phi_in[InlineCoords(Width - 2, j)] + phi_in[InlineCoords(Width - 1, j + 1)] + phi_in[
					InlineCoords(Width - 1, j - 1)] - 3.0 * x;
				phi_out[InlineCoords(Width - 1, j)] = x - ResourceProperties.Courant * (x - phi_in[InlineCoords(
						Width - 2, j)])
					- ResourceProperties.Kappa_Absorbing * x -
					ResourceProperties.Gamma_Absorbing * (x - y);
				break;
			}
		}
		psi_out[InlineCoords(Width - 1, j)] = x;
	}

	/* top boundary */
	for (i = 0; i < Width; i++)
	{
		x = phi_in[InlineCoords(i, jmid - 1)];
		y = psi_in[InlineCoords(i, jmid - 1)];

		switch (Boundary)
		{
		case (Dirichlet):
			{
				iplus = i + 1;
				if (iplus == Width)
				{
					iplus = Width - 1;
				}
				iminus = i - 1;
				if (iminus == -1)
				{
					iminus = 0;
				}

				delta = phi_in[InlineCoords(iplus, jmid - 1)] + phi_in[InlineCoords(iminus, jmid - 1)]
					+ phi_in[InlineCoords(i, jmid - 2)] - 3.0 * x;
				phi_out[InlineCoords(i, jmid - 1)] = -y + 2 * x + ResourceProperties.Courant2 * delta
					- ResourceProperties.Kappa * x - ResourceProperties.Gamma_Absorbing * (x
						- y);
				break;
			}
		case (Periodic):
			{
				iplus = (i + 1) % Width;
				iminus = (i - 1) % Width;
				if (iminus < 0)
				{
					iminus += Width;
				}

				delta = phi_in[InlineCoords(iplus, jmid - 1)] + phi_in[InlineCoords(iminus, jmid - 1)] + phi_in[
						InlineCoords(i, jmid - 2)] + phi_in[InlineCoords(i, 0)] -
					4.0 * x;
				phi_out[InlineCoords(i, jmid - 1)] = -y + 2 * x + ResourceProperties.Courant2 * delta
					- ResourceProperties.Kappa * x - ResourceProperties.Gamma * (x
						- y);
				break;
			}
		case (Absorbing):
			{
				iplus = (i + 1);
				if (iplus == Width)
				{
					iplus = Width - 1;
				}
				iminus = (i - 1);
				if (iminus == -1)
				{
					iminus = 0;
				}

				delta = phi_in[InlineCoords(iplus, jmid - 1)] + phi_in[InlineCoords(iminus, jmid - 1)] + phi_in[
					InlineCoords(i, jmid - 2)] - 3.0 * x;
				phi_out[InlineCoords(i, jmid - 1)] = x - ResourceProperties.Courant * (x - phi_in[InlineCoords(
						i, jmid - 2)])
					- ResourceProperties.Kappa_Absorbing * x -
					ResourceProperties.Gamma_Absorbing * (x - y);
				break;
			}
		case (Vper_Habs):
			{
				iplus = (i + 1);
				if (iplus == Width)
				{
					iplus = Width - 1;
				}
				iminus = (i - 1);
				if (iminus == -1)
				{
					iminus = 0;
				}

				delta = phi_in[InlineCoords(iplus, jmid - 1)] + phi_in[InlineCoords(iminus, jmid - 1)] + phi_in[
						InlineCoords(i, jmid - 2)] + phi_in[InlineCoords(i, 0)] -
					4.0 * x;
				if (i == 0)
				{
					phi_out[InlineCoords(0, jmid - 1)] = x - ResourceProperties.Courant * (x - phi_in[InlineCoords(
							1, jmid - 1)])
						- ResourceProperties.Kappa_Absorbing * x -
						ResourceProperties.Gamma_Absorbing * (x - y);
				}
				else
				{
					phi_out[InlineCoords(i, jmid - 1)] = -y + 2 * x + ResourceProperties.Courant2 * delta -
						ResourceProperties.Kappa * x - ResourceProperties.Gamma *
						(x - y);
				}
				//                      delta = phi_in[1][j] + phi_in[0][j+1] + phi_in[0][j-1] - 3.0*x;
				//                     phi_out[0][j] = x - tc[0][j]*(x - phi_in[1][j]) - KAPPA_SIDES*x - GAMMA_SIDES*(x-y);
				break;
			}
		}
		psi_out[InlineCoords(i, jmid - 1)] = x;
	}

	/* bottom boundary */
	for (i = 0; i < Width; i++)
	{
		x = phi_in[InlineCoords(i, 0)];
		y = psi_in[InlineCoords(i, 0)];

		switch (Boundary)
		{
		case (Dirichlet):
			{
				iplus = i + 1;
				if (iplus == Width)
				{
					iplus = Width - 1;
				}
				iminus = i - 1;
				if (iminus == -1)
				{
					iminus = 0;
				}

				delta = phi_in[InlineCoords(iplus, 0)] + phi_in[InlineCoords(iminus, 0)] + phi_in[InlineCoords(i, 1)] -
					3.0 * x;
				phi_out[InlineCoords(i, 0)] = -y + 2 * x + ResourceProperties.Courant2 * delta
					- ResourceProperties.Kappa * x - ResourceProperties.Gamma * (x - y);
				break;
			}
		case (Periodic):
			{
				iplus = (i + 1) % Width;
				iminus = (i - 1) % Width;
				if (iminus < 0)
				{
					iminus += Width;
				}

				delta = phi_in[InlineCoords(iplus, 0)] + phi_in[InlineCoords(iminus, 0)] + phi_in[InlineCoords(i, 1)] +
					phi_in[InlineCoords(i, jmid - 1)] - 4.0 * x;
				phi_out[InlineCoords(i, 0)] = -y + 2 * x + ResourceProperties.Courant2 * delta
					- ResourceProperties.Kappa * x - ResourceProperties.Gamma * (x - y);
				break;
			}
		case (Absorbing):
			{
				iplus = (i + 1);
				if (iplus == Width)
				{
					iplus = Width - 1;
				}
				iminus = (i - 1);
				if (iminus == -1)
				{
					iminus = 0;
				}

				delta = phi_in[InlineCoords(iplus, 0)] + phi_in[InlineCoords(iminus, 0)] + phi_in[InlineCoords(i, 1)] -
					3.0 * x;
				phi_out[InlineCoords(i, 0)] = x - ResourceProperties.Courant * (x - phi_in[InlineCoords(i, 1)])
					- ResourceProperties.Kappa_Absorbing * x - ResourceProperties.Gamma_Absorbing * (x - y);
				break;
			}
		case (Vper_Habs):
			{
				iplus = (i + 1);
				if (iplus == Width)
				{
					iplus = Width - 1;
				}
				iminus = (i - 1);
				if (iminus == -1)
				{
					iminus = 0;
				}

				delta = phi_in[InlineCoords(iplus, 0)] + phi_in[InlineCoords(iminus, 0)] + phi_in[InlineCoords(i, 1)] +
					phi_in[InlineCoords(i, jmid - 1)] - 4.0 * x;
				if (i == 0)
				{
					phi_out[InlineCoords(0, 0)] = x - ResourceProperties.Courant * (x - phi_in[InlineCoords(1, 0)])
						- ResourceProperties.Kappa_Absorbing * x - ResourceProperties.Gamma_Absorbing * (x - y);
				}
				else
				{
					phi_out[InlineCoords(i, 0)] = -y + 2 * x + ResourceProperties.Courant2 * delta
						- ResourceProperties.Kappa * x - ResourceProperties.Gamma * (x - y);
				}
				break;
			}
		}
		psi_out[InlineCoords(i, 0)] = x;
	}

	/* add oscillating boundary condition on the left corners */
	if ((i == 0) && (bOscillateLeft))
	{
		phi_out[InlineCoords(i, 0)] = OscillationAmplitude *
			FMath::Cos(static_cast<float>(time) * OscillationFrequency);
		phi_out[InlineCoords(i, jmid - 1)] = OscillationAmplitude * FMath::Cos(
			static_cast<float>(time) * OscillationFrequency);
	}

	/* for debugging purposes/if there is a risk of blow-up */
	if (bUseResourceMax)
	{
		for (i = 0; i < Width; i++)
		{
			for (j = 0; j < jmid; j++)
			{
				if (phi_out[InlineCoords(i, j)] > ResourceMax)
				{
					phi_out[InlineCoords(i, j)] = ResourceMax;
				}
				if (phi_out[InlineCoords(i, j)] < -ResourceMax)
				{
					phi_out[InlineCoords(i, j)] = -ResourceMax;
				}
				if (psi_out[InlineCoords(i, j)] > ResourceMax)
				{
					psi_out[InlineCoords(i, j)] = ResourceMax;
				}
				if (psi_out[InlineCoords(i, j)] < -ResourceMax)
				{
					psi_out[InlineCoords(i, j)] = -ResourceMax;
				}
			}
		}
	}
}
