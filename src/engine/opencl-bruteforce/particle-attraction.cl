__kernel void attraction(float dt, int n, __global float4* before, __global float4* after) {
  int global_id = get_global_id(0);
  float4 p1 = before[global_id];

  float accelerationX = 0.f;
  float accelerationY = 0.f;

  // NOTE: May calculate on itself
  for (int j = 0; j < n; j++) {
    float4 p2 = before[j];

    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;

    float distInv = rsqrt(fmax(dx * dx + dy * dy, 0.1f));
    float distInv3 = distInv * distInv * distInv;

    accelerationX += dx * distInv3;
    accelerationY += dy * distInv3;
  }

  p1.z += accelerationX * dt * 1e6;
  p1.w += accelerationY * dt * 1e6;

  p1.x += p1.z;
  p1.y += p1.w;

  after[global_id] = p1;
}

