#define IX(i,j) ((i)+(N+2)*(j))
#define SWAP(x0,x) {float * tmp=x0;x0=x;x=tmp;}
#define FOR_EACH_CELL for ( i=1 ; i<=N ; i++ ) { for ( j=1 ; j<=N ; j++ ) {
#define END_FOR }}

void add_source(int N, float* x, float* s, float dt)
{
	const int size = (N + 2) * (N + 2);
	for (int i = 0; i < size; i++)
	{
		x[i] += dt * s[i];
	}
}

void set_bnd(int N, int b, float* x)
{
	for (int i = 1; i <= N; i++)
	{
		x[IX(0, i)] = b == 1 ? -x[IX(1, i)] : x[IX(1, i)];
		x[IX(N+1, i)] = b == 1 ? -x[IX(N, i)] : x[IX(N, i)];
		x[IX(i, 0)] = b == 2 ? -x[IX(i, 1)] : x[IX(i, 1)];
		x[IX(i, N+1)] = b == 2 ? -x[IX(i, N)] : x[IX(i, N)];
	}
	x[IX(0, 0)] = 0.5f * (x[IX(1, 0)] + x[IX(0, 1)]);
	x[IX(0, N+1)] = 0.5f * (x[IX(1, N+1)] + x[IX(0, N)]);
	x[IX(N+1, 0)] = 0.5f * (x[IX(N, 0)] + x[IX(N+1, 1)]);
	x[IX(N+1, N+1)] = 0.5f * (x[IX(N, N+1)] + x[IX(N+1, N)]);
}

void lin_solve(int N, int b, float* x, float* x0, float a, float c)
{
	int i, j;

	for (int k = 0; k < 20; k++)
	{
		FOR_EACH_CELL
			x[IX(i, j)] = (x0[IX(i, j)] + a * (x[IX(i-1, j)] + x[IX(i+1, j)] + x[IX(i, j-1)] + x[IX(i, j+1)])) / c;
		END_FOR
		set_bnd(N, b, x);
	}
}

float clamp(float v, float min, float max, int& d)
{
	if (v < min)
	{
		d--;
		return min;
	}
	if (v > max)
	{
		return max;
	}
	return v;
}

void lin_solve(int N, int b, float* x, float* x0, const float* gl, float a)
{
	int i, j;
	int d = 4;
	for (int k = 0; k < 20; k++)
	{
		FOR_EACH_CELL
			const float cX = x0[IX(i, j)];
			const float cH = gl[IX(i, j)];
			const float lX = clamp(x[IX(i-1, j)] + gl[IX(i-1, j)] - cH, 0, x[IX(i-1, j)], d);
			const float rX = clamp(x[IX(i+1, j)] + gl[IX(i+1, j)] - cH, 0, x[IX(i+1, j)], d);
			const float uX = clamp(x[IX(i, j-1)] + gl[IX(i, j-1)] - cH, 0, x[IX(i, j-1)], d);
			const float dX = clamp(x[IX(i, j+1)] + gl[IX(i, j+1)] - cH, 0, x[IX(i, j+1)], d);
			float c = 1 + 4 * a;
			x[IX(i, j)] = (cX + a * (
				lX + rX + uX + dX)) / c;
		END_FOR
		set_bnd(N, b, x);
	}
}

void diffuse(int N, int b, float* x, float* x0, float diff, float dt)
{
	const float a = dt * diff * N * N;
	lin_solve(N, b, x, x0, a, 1 + 4 * a);
}

void advect(int N, int b, float* d, float* d0, float* u, float* v, float dt)
{
	int i, j;

	const float dt0 = dt * N;
	FOR_EACH_CELL
		float x = i - dt0 * u[IX(i, j)];
		float y = j - dt0 * v[IX(i, j)];
		if (x < 0.5f)
		{
			x = 0.5f;
		}
		if (x > N + 0.5f)
		{
			x = N + 0.5f;
		}
		const int i0 = static_cast<int>(x);
		const int i1 = i0 + 1;
		if (y < 0.5f)
		{
			y = 0.5f;
		}
		if (y > N + 0.5f)
		{
			y = N + 0.5f;
		}
		const int j0 = static_cast<int>(y);
		const int j1 = j0 + 1;
		const float s1 = x - i0;
		const float s0 = 1 - s1;
		const float t1 = y - j0;
		const float t0 = 1 - t1;
		d[IX(i, j)] = s0 * (t0 * d0[IX(i0, j0)] + t1 * d0[IX(i0, j1)]) +
					  s1 * (t0 * d0[IX(i1, j0)] + t1 * d0[IX(i1, j1)]);
	END_FOR
	set_bnd(N, b, d);
}

void project(int N, float* u, float* v, float* p, float* div)
{
	int i, j;

	FOR_EACH_CELL
		div[IX(i, j)] = -0.5f * (u[IX(i+1, j)] - u[IX(i-1, j)] + v[IX(i, j+1)] - v[IX(i, j-1)]) / N;
		p[IX(i, j)] = 0;
	END_FOR
	set_bnd(N, 0, div);
	set_bnd(N, 0, p);

	lin_solve(N, 0, p, div, 1, 4);

	FOR_EACH_CELL
		u[IX(i, j)] -= 0.5f * N * (p[IX(i+1, j)] - p[IX(i-1, j)]);
		v[IX(i, j)] -= 0.5f * N * (p[IX(i, j+1)] - p[IX(i, j-1)]);
	END_FOR
	set_bnd(N, 1, u);
	set_bnd(N, 2, v);
}

void dens_step(int N, float* x, float* x0, float* u, float* v, float diff, float dt)
{
	add_source(N, x, x0, dt);
	SWAP(x0, x);
	diffuse(N, 0, x, x0, diff, dt);
	SWAP(x0, x);
	advect(N, 0, x, x0, u, v, dt);
}

void vel_step(int N, float* u, float* v, float* u0, float* v0, float visc, float dt)
{
	add_source(N, u, u0, dt);
	add_source(N, v, v0, dt);
	SWAP(u0, u);
	diffuse(N, 1, u, u0, visc, dt);
	SWAP(v0, v);
	diffuse(N, 2, v, v0, visc, dt);
	project(N, u, v, u0, v0);
	SWAP(u0, u);
	SWAP(v0, v);
	advect(N, 1, u, u0, u0, v0, dt);
	advect(N, 2, v, v0, u0, v0, dt);
	project(N, u, v, u0, v0);
}
