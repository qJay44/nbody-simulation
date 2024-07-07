#pragma once
#define WIDTH 1200
#define HEIGHT 720

#define INITIAL_PARTICLES 30000
#define SPIRAL_ARMS 5
#define SPIRAL_ARMS_WIDTH 20         // Mini arms in arms
#define SPIRAL_ARMS_WIDTH_VALUE 9.f // Distance between each mini arm (pi divider)
#define SPIRAL_ARM_TWIST_VALUE 100.f  // How much the arm is twisted (pi divider)
#define SPIRAL_ARM_LENGTH (int)(INITIAL_PARTICLES / SPIRAL_ARMS / SPIRAL_ARMS_WIDTH)
#define ZERO_DIVISION_PREVENT_VALUE 0.1f

#define RADIUS 1
#define CIRCLE_TEXTURE_SIZE 1024
#define INITIAL_MASS 1.f

#define QUAD_TREE_MAX_DEPTH 0xff
#define QUAD_TREE_MAX_CAPACITY 4
#define QUAD_TREE_THETA 0.5f
