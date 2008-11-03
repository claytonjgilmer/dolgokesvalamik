// incrementArray.cu
#include <stdio.h>
#include <assert.h>
#include <cuda.h>

#define TYPE float
typedef struct { float x; float y[3];} vec2;
//typedef struct { float x;} vec2;

void incrementArrayOnHost(vec2 *a, int N)
{
	int i;
	for (i=0; i < N; i++)
	{
		a[i].x+=1.f;
//		a[i].y+=1.f;
	}
}
__global__ void incrementArrayOnDevice(vec2 *a, int N, int LoopC)
{
	int idx = blockIdx.x*blockDim.x + threadIdx.x;
//if (idx<N) a[idx] = a[idx]+1.f;
	for (int n=0; n<LoopC; ++n)
		a[idx].x+=1.f;
//	a[idx].y+=1.f;
}
int main(void)
{
vec2 *a_h, *b_h;           // pointers to host memory
vec2 *a_d;                 // pointer to device memory
int i, N = 256*256;
int loopc=100000;
size_t size = N*sizeof(vec2);
// allocate arrays on host
a_h = (vec2*)malloc(size);
b_h = (vec2*)malloc(size);
// allocate array on device 
cudaMalloc((void**) &a_d, size);
// initialization of host data

for (i=0; i<N; i++)
{
	a_h[i].x = (float)i;
//	a_h[i].y = -(float)i;
}

// copy data from host to device
cudaMemcpy(a_d, a_h, sizeof(vec2)*N, cudaMemcpyHostToDevice);
printf("do calculation on host\n");

for (int n=0; n<loopc; ++n)
	incrementArrayOnHost(a_h, N);
printf("do calculation on device\n");
// Part 1 of 2. Compute execution configuration
int blockSize = 128;
int nBlocks = N/blockSize + (N%blockSize == 0?0:1);
// Part 2 of 2. Call incrementArrayOnDevice kernel 

//for (int n=0; n<loopc; ++n)
incrementArrayOnDevice <<< nBlocks, blockSize >>> (a_d, N,loopc);
cudaThreadSynchronize();
	
printf("Retrieve result from device and store in b_h\n");
cudaMemcpy(b_h, a_d, sizeof(vec2)*N, cudaMemcpyDeviceToHost);
printf("check results\n");

int diffc=0;
for (i=0; i<N; i++)
{
	if (a_h[i].x != b_h[i].x)// || a_h[i].y != b_h[i].y)
		++diffc;
}
	
printf("diffcount:%d\n",diffc);
printf("cleanup\n");
free(a_h); free(b_h); cudaFree(a_d); 
}
