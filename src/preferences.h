#pragma once
#define WIDTH 1200
#define HEIGHT 720

#define RADIUS 2
#define INITIAL_PARTICLES 15000
#define INITIAL_MASS 1.f
#define CIRCLE_TEXTURE_SIZE 1024

#define SPIRAL_ARMS 8
#define SPIRAL_ARM_LENGTH (int)(INITIAL_PARTICLES / SPIRAL_ARMS)
#define SPIRAL_STEP 0.0015f   // Distance between each created particle
#define SPIRAL_OFFSET 0.125f  // Offest between spiral arms

#define QUAD_TREE_MAX_DEPTH 0xff
#define QUAD_TREE_MAX_CAPACITY 1
#define QUAD_TREE_THETA_SQ 0.25f
