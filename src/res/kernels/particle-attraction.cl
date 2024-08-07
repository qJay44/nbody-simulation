#define ZERO_DIVISION_PREVENT_VALUE 0.1f

__kernel void attraction(float dt, const int n, __global float4* before, __global float4* after) {
  int globalId = get_global_id(0);
  float4 p1 = before[globalId];

  float accelerationX = 0.f;
  float accelerationY = 0.f;

  for (int j = 0; j < n; j++) {
    if (j != globalId) {
      float4 p2 = before[j];

      float dx = p2.x - p1.x;
      float dy = p2.y - p1.y;

      float dist= sqrt(dx * dx + dy * dy);
      float distInv = 1.f / (dist + ZERO_DIVISION_PREVENT_VALUE);
      float distInv3 = distInv * distInv * distInv;

      accelerationX += dx * distInv3;
      accelerationY += dy * distInv3;
    }
  }

  // Calculate velocity
  p1.z += accelerationX * dt * dt;
  p1.w += accelerationY * dt * dt;

  // Add velocity to the position
  p1.x += p1.z;
  p1.y += p1.w;

  after[globalId] = p1;
}

