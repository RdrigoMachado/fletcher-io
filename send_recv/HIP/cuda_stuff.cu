#include "hip/hip_runtime.h"
#include "cuda_defines.h"
#include "cuda_stuff.h"

static size_t sxsy=0;

void CUDA_Initialize(const int sx, const int sy, const int sz, const int bord,
	       float dx, float dy, float dz, float dt,
	       float * restrict ch1dxx, float * restrict ch1dyy, float * restrict ch1dzz, 
	       float * restrict ch1dxy, float * restrict ch1dyz, float * restrict ch1dxz, 
	       float * restrict v2px, float * restrict v2pz, float * restrict v2sz, float * restrict v2pn,
	       float * restrict vpz, float * restrict vsv, float * restrict epsilon, float * restrict delta,
	       float * restrict phi, float * restrict theta, 
	       float * restrict pp, float * restrict pc, float * restrict qp, float * restrict qc)
{

   extern float* dev_ch1dxx;
   extern float* dev_ch1dyy;
   extern float* dev_ch1dzz;
   extern float* dev_ch1dxy;
   extern float* dev_ch1dyz;
   extern float* dev_ch1dxz;
   extern float* dev_v2px;
   extern float* dev_v2pz;
   extern float* dev_v2sz;
   extern float* dev_v2pn;
   extern float* dev_pp;
   extern float* dev_pc;
   extern float* dev_qp;
   extern float* dev_qc;

 
  int deviceCount;
  CUDA_CALL(hipGetDeviceCount(&deviceCount));
  const int device=0;
  hipDeviceProp_t deviceProp;
  CUDA_CALL(hipGetDeviceProperties(&deviceProp, device));
  printf("CUDA source using device(%d) %s with compute capability %d.%d.\n", device, deviceProp.name, deviceProp.major, deviceProp.minor);
  CUDA_CALL(hipSetDevice(device));


  // Check sx,sy values
  if (sx%BSIZE_X != 0)
  {
     printf("sx(%d) must be multiple of BSIZE_X(%d)\n", sx, (int)BSIZE_X);
     exit(1);
  } 
  if (sy%BSIZE_Y != 0)
  {
     printf("sy(%d) must be multiple of BSIZE_Y(%d)\n", sy, (int)BSIZE_Y);
     exit(1);
  } 

   sxsy=sx*sy; // one plan
   const size_t sxsysz=sxsy*sz;
   const size_t msize_vol=sxsysz*sizeof(float);
   const size_t msize_vol_extra=msize_vol+2*sxsy*sizeof(float); // 2 extra plans for wave fields

   CUDA_CALL(hipMalloc(&dev_ch1dxx, msize_vol));
   CUDA_CALL(hipMemcpy(dev_ch1dxx, ch1dxx, msize_vol, hipMemcpyHostToDevice));
   CUDA_CALL(hipMalloc(&dev_ch1dyy, msize_vol));
   CUDA_CALL(hipMemcpy(dev_ch1dyy, ch1dyy, msize_vol, hipMemcpyHostToDevice));
   CUDA_CALL(hipMalloc(&dev_ch1dzz, msize_vol));
   CUDA_CALL(hipMemcpy(dev_ch1dzz, ch1dzz, msize_vol, hipMemcpyHostToDevice));
   CUDA_CALL(hipMalloc(&dev_ch1dxy, msize_vol));
   CUDA_CALL(hipMemcpy(dev_ch1dxy, ch1dxy, msize_vol, hipMemcpyHostToDevice));
   CUDA_CALL(hipMalloc(&dev_ch1dyz, msize_vol));
   CUDA_CALL(hipMemcpy(dev_ch1dyz, ch1dyz, msize_vol, hipMemcpyHostToDevice));
   CUDA_CALL(hipMalloc(&dev_ch1dxz, msize_vol));
   CUDA_CALL(hipMemcpy(dev_ch1dxz, ch1dxz, msize_vol, hipMemcpyHostToDevice));
   CUDA_CALL(hipMalloc(&dev_v2px, msize_vol));
   CUDA_CALL(hipMemcpy(dev_v2px, v2px, msize_vol, hipMemcpyHostToDevice));
   CUDA_CALL(hipMalloc(&dev_v2pz, msize_vol));
   CUDA_CALL(hipMemcpy(dev_v2pz, v2pz, msize_vol, hipMemcpyHostToDevice));
   CUDA_CALL(hipMalloc(&dev_v2sz, msize_vol));
   CUDA_CALL(hipMemcpy(dev_v2sz, v2sz, msize_vol, hipMemcpyHostToDevice));
   CUDA_CALL(hipMalloc(&dev_v2pn, msize_vol));
   CUDA_CALL(hipMemcpy(dev_v2pn, v2pn, msize_vol, hipMemcpyHostToDevice));

   // Wave field arrays with an extra plan
   CUDA_CALL(hipMalloc(&dev_pp, msize_vol_extra));
   CUDA_CALL(hipMemset(dev_pp, 0, msize_vol_extra));
   CUDA_CALL(hipMalloc(&dev_pc, msize_vol_extra));
   CUDA_CALL(hipMemset(dev_pc, 0, msize_vol_extra));
   CUDA_CALL(hipMalloc(&dev_qp, msize_vol_extra));
   CUDA_CALL(hipMemset(dev_qp, 0, msize_vol_extra));
   CUDA_CALL(hipMalloc(&dev_qc, msize_vol_extra));
   CUDA_CALL(hipMemset(dev_qc, 0, msize_vol_extra));
   dev_pp+=sxsy;
   dev_pc+=sxsy;
   dev_qp+=sxsy;
   dev_qc+=sxsy;



  CUDA_CALL(hipGetLastError());
  CUDA_CALL(hipDeviceSynchronize());
  printf("GPU memory usage = %ld MiB\n", 15*msize_vol/1024/1024);

}


void CUDA_Finalize()
{

   extern float* dev_ch1dxx;
   extern float* dev_ch1dyy;
   extern float* dev_ch1dzz;
   extern float* dev_ch1dxy;
   extern float* dev_ch1dyz;
   extern float* dev_ch1dxz;
   extern float* dev_v2px;
   extern float* dev_v2pz;
   extern float* dev_v2sz;
   extern float* dev_v2pn;
   extern float* dev_pp;
   extern float* dev_pc;
   extern float* dev_qp;
   extern float* dev_qc;

   dev_pp-=sxsy;
   dev_pc-=sxsy;
   dev_qp-=sxsy;
   dev_qc-=sxsy;

   CUDA_CALL(hipFree(dev_ch1dxx));
   CUDA_CALL(hipFree(dev_ch1dyy));
   CUDA_CALL(hipFree(dev_ch1dzz));
   CUDA_CALL(hipFree(dev_ch1dxy));
   CUDA_CALL(hipFree(dev_ch1dyz));
   CUDA_CALL(hipFree(dev_ch1dxz));
   CUDA_CALL(hipFree(dev_v2px));
   CUDA_CALL(hipFree(dev_v2pz));
   CUDA_CALL(hipFree(dev_v2sz));
   CUDA_CALL(hipFree(dev_v2pn));
   CUDA_CALL(hipFree(dev_pp));
   CUDA_CALL(hipFree(dev_pc));
   CUDA_CALL(hipFree(dev_qp));
   CUDA_CALL(hipFree(dev_qc));

   printf("CUDA_Finalize: SUCCESS\n");
}



void CUDA_Update_pointers(const int sx, const int sy, const int sz, float *pc)
{
   extern float* dev_pc;
   const size_t sxsysz=((size_t)sx*sy)*sz;
   const size_t msize_vol=sxsysz*sizeof(float);
   if (pc) CUDA_CALL(hipMemcpy(pc, dev_pc, msize_vol, hipMemcpyDeviceToHost));
}
