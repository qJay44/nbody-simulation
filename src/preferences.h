#pragma once
#define WIDTH 1200
#define HEIGHT 720

#define RADIUS 2
#define INITIAL_PARTICLES 5000
#define INITIAL_MASS 1e10f

#define CELL_SIZE 60
#define COLUMNS ((int)(WIDTH / CELL_SIZE))
#define ROWS ((int)(HEIGHT / CELL_SIZE))

#define QUAD_TREE_DEPTH 50
#define QUAD_TREE_CAPACITY 1
#define QUAD_TREE_THETA 0.5f
