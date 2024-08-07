#include <stdint.h>
#include <vector>

#include "CL/opencl.h"
#include "../Particle.hpp"

class RuntimeOpenCL {
  public:
    RuntimeOpenCL(const std::vector<Particle>& particles);
    ~RuntimeOpenCL();

    [[nodiscard]]
    const cl_float4* getComputedParticlesPtr() const;

    void run(const float& dt);

  private:
    const uint32_t n;
    cl_float4* currentParticles;
    cl_float4* nextParticles;

    cl_device_id device = nullptr;
    size_t maxLocalSize;
    size_t maxDimensions;

    cl_context context;
    cl_command_queue commandQueue;

    cl_mem gpuCurrentParticles;
    cl_mem gpuNextParticles;

    cl_kernel kernel;
    cl_program program;
};

