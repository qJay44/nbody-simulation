#define WIDTH 1200
#define HEIGHT 720
#define QUAD_TREE_CAPACITY 50

#define COLUMNS (int)(WIDTH / CELL_SIZE)
#define ROWS (int)(HEIGHT / CELL_SIZE)
#define IX(x, y) ((x) + (y) * (COLUMNS))

