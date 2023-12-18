// https://github.com/CobaltXII/cosmos/blob/master/cosmos_simulate.cpp

#include <cassert>
#include <string>

#include "RuntimeOpenCL.hpp"
#include "myutils.hpp"

#define ATTRIBUTE_COUNT 5

const cl_platform_info attributeTypes[ATTRIBUTE_COUNT] = {
  CL_PLATFORM_NAME,
  CL_PLATFORM_VENDOR,
  CL_PLATFORM_VERSION,
  CL_PLATFORM_PROFILE,
  CL_PLATFORM_EXTENSIONS
};

const char* const attributeNames[ATTRIBUTE_COUNT] = {
  "CL_PLATFORM_NAME",
  "CL_PLATFORM_VENDOR",
  "CL_PLATFORM_VERSION",
  "CL_PLATFORM_PROFILE",
  "CL_PLATFORM_EXTENSIONS"
};

RuntimeOpenCL::RuntimeOpenCL(const std::vector<Particle>& particles) : n(particles.size()) {
  cl_platform_id platforms[64];
  cl_uint platformCount;
  cl_int platformsResult = clGetPlatformIDs(64, platforms, &platformCount);
  assert(platformsResult == CL_SUCCESS);

  size_t infosize;
  cl_int getPlatformInfoResult;
  for (int i = 0; i < platformCount; i++) {
    for (int j = 0; j < ATTRIBUTE_COUNT; j++) {
      // Get platform attribute value size
      getPlatformInfoResult = clGetPlatformInfo(platforms[i], attributeTypes[j], 0, nullptr, &infosize);
      assert(getPlatformInfoResult == CL_SUCCESS);
      char info[infosize];

      // Get platform attribute value
      getPlatformInfoResult = clGetPlatformInfo(platforms[i], attributeTypes[j], infosize, info, nullptr);
      assert(getPlatformInfoResult == CL_SUCCESS);
      printf("%d.%d %-11s: %s\n", i+1, j+1, attributeNames[j], info);
    }
  }

  printf("\n");

  for (int i = 0; i < platformCount; i++) {
    cl_device_id devices[64];
    cl_uint deviceCount;
    cl_int deviceResult = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, 64, devices, &deviceCount);

    if (deviceResult == CL_SUCCESS) {
      for (int j = 0; j < deviceCount; j++) {
        char vendorName[256];
        size_t vendorNameLength;
        cl_int deviceInfoResult = clGetDeviceInfo(devices[j], CL_DEVICE_VENDOR, 256, vendorName, &vendorNameLength);
        if (deviceInfoResult == CL_SUCCESS) {
          device = devices[j];
          break;
        }
      }
    }
  }

  assert(device);

  clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(maxDimensions), &maxDimensions, nullptr);
  printf("2.1 CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS: %zu\n", maxDimensions);

  clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(maxLocalSize), &maxLocalSize, nullptr);
  printf("2.2 CL_DEVICE_MAX_WORK_GROUP_SIZE: %zu\n", maxLocalSize);

  size_t maxDimensionsValues[maxDimensions];
  clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, maxDimensions * sizeof(maxDimensionsValues[0]), maxDimensionsValues, nullptr);

  printf("2.3 CL_DEVICE_MAX_WORK_ITEM_SIZES: ");
  for (int i = 0; i < maxDimensions; i++) printf("%zu ", maxDimensionsValues[i]);
  printf("\n\n");

  cl_int contextResult;
  context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &contextResult);
  assert(contextResult == CL_SUCCESS);

  cl_int commandQueueResult;
  commandQueue = clCreateCommandQueueWithProperties(context, device, 0, &commandQueueResult);
  assert(commandQueueResult == CL_SUCCESS);

  currentParticles = new cl_float4[n];
  nextParticles = new cl_float4[n];

  for (int i = 0; i < n; i++) {
    const Particle& p = particles[i];
    currentParticles[i] = {
      p.getPosition().x,
      p.getPosition().y,
      p.getVelocity().x,
      p.getVelocity().y
    };
  }

  // Clear the second state (fill the array with zeros).
	memset(nextParticles, 0, n * sizeof(cl_float4));

  cl_int gpuMallocResult;
  gpuCurrentParticles = clCreateBuffer(context, CL_MEM_READ_WRITE, n * sizeof(cl_float4), nullptr, &gpuMallocResult);
  gpuNextParticles    = clCreateBuffer(context, CL_MEM_READ_WRITE, n * sizeof(cl_float4), nullptr, &gpuMallocResult);
  assert(gpuMallocResult == CL_SUCCESS);

  cl_int cpuCopyResult1;
  cl_int cpuCopyResult2;
  cpuCopyResult1 = clEnqueueWriteBuffer(commandQueue, gpuCurrentParticles, CL_TRUE, 0, n * sizeof(cl_float4), currentParticles, 0, nullptr, nullptr);
  cpuCopyResult2 = clEnqueueWriteBuffer(commandQueue, gpuNextParticles,    CL_TRUE, 0, n * sizeof(cl_float4), nextParticles, 0, nullptr, nullptr);
  assert(cpuCopyResult1 == CL_SUCCESS);
  assert(cpuCopyResult2 == CL_SUCCESS);

  cl_int programResult;
  std::string clFile = readFromFile("../../src/engine/opencl-bruteforce/particle-attraction.cl");
  const char* programSource = clFile.c_str();
  size_t programSourceLength = 0;
  program = clCreateProgramWithSource(context, 1, &programSource, &programSourceLength, &programResult);
  assert(programResult == CL_SUCCESS);

  cl_int buildResult = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
  assert(buildResult == CL_SUCCESS);

  cl_int kernelResult;
  kernel = clCreateKernel(program, "attraction", &kernelResult);
  assert(kernelResult == CL_SUCCESS);

  cl_int kernelArgResult1 = clSetKernelArg(kernel, 1, sizeof(cl_int), &n);
  cl_int kernelArgResult2 = clSetKernelArg(kernel, 2, sizeof(cl_mem), &gpuCurrentParticles);
  cl_int kernelArgResult3 = clSetKernelArg(kernel, 3, sizeof(cl_mem), &gpuNextParticles);
  assert(kernelArgResult1 == CL_SUCCESS);
  assert(kernelArgResult2 == CL_SUCCESS);
  assert(kernelArgResult3 == CL_SUCCESS);
}

RuntimeOpenCL::~RuntimeOpenCL() {
  clReleaseMemObject(gpuCurrentParticles);
	clReleaseMemObject(gpuNextParticles);
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(commandQueue);
	clReleaseContext(context);
	clReleaseDevice(device);

  delete currentParticles;
  delete nextParticles;
}

void RuntimeOpenCL::run(const float& dt) {
  static const size_t globalWorkSize = n;
  static const size_t localWorkSize = maxLocalSize;

  clSetKernelArg(kernel, 0, sizeof(cl_float), &dt);
  clSetKernelArg(kernel, 2, sizeof(cl_mem), &gpuCurrentParticles);
  clSetKernelArg(kernel, 3, sizeof(cl_mem), &gpuNextParticles);

  clEnqueueNDRangeKernel(commandQueue, kernel, 1, nullptr, &globalWorkSize, &localWorkSize, 0, nullptr, nullptr);
  clEnqueueReadBuffer(commandQueue, gpuNextParticles, CL_TRUE, 0, n * sizeof(cl_float4), nextParticles, 0, nullptr, nullptr);

  std::swap(gpuCurrentParticles, gpuNextParticles);

  clFinish(commandQueue);
}

const cl_float4* RuntimeOpenCL::getComputedParticlesPtr() const {
  return nextParticles;
}

