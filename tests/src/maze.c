#include <float.h>
#include <math.h>

#define ROWS 7
#define COLS 9
#define CELLS (ROWS * COLS)
#define START (COLS + 1)
#define GOAL (COLS + 7)

static const char maze[ROWS][COLS + 1] = {
    "#########",
    "#S..#..G#",
    "#.#.#.#.#",
    "#.#...#.#",
    "#.#####.#",
    "#.......#",
    "#########"
};

int bfs_path[CELLS];
int astar_path[CELLS];
int bfs_distance;
int astar_distance;
unsigned int bfs_expanded;
unsigned int astar_expanded;

static int walkable(const char grid[ROWS][COLS + 1], int cell)
{
    return cell >= 0 && cell < CELLS && grid[cell / COLS][cell % COLS] != '#';
}

static int neighbor(int cell, unsigned int direction)
{
    int row = cell / COLS, col = cell % COLS;
    if (direction == 0) {return row > 0 ? cell - COLS : -1;}
    if (direction == 1) {return col + 1 < COLS ? cell + 1 : -1;}
    if (direction == 2) {return row + 1 < ROWS ? cell + COLS : -1;}
    return col > 0 ? cell - 1 : -1;
}

static int reconstruct_path(const int parents[CELLS], int start, int goal, int path[CELLS])
{
    int reverse[CELLS], cell = goal, count = 0, i;
    while (cell != -1 && count < CELLS) {
        reverse[count++] = cell;
        if (cell == start) {
            for (i = 0; i < count; i++) {path[i] = reverse[count-1-i];}
            return count - 1;
        }
        cell = parents[cell];
    }
    return -1;
}

int bfs_solve(const char grid[ROWS][COLS + 1], int start, int goal,
    int path[CELLS], unsigned int *expanded)
{
    int parents[CELLS], queue[CELLS];
    unsigned char seen[CELLS];
    unsigned int head = 0, tail = 0, direction;
    int i;

    *expanded = 0;
    if (!walkable(grid, start) || !walkable(grid, goal)) {return -1;}
    for (i = 0; i < CELLS; i++) {parents[i] = -1; seen[i] = 0;}
    queue[tail++] = start;
    seen[start] = 1;
    while (head < tail) {
        int cell = queue[head++];
        (*expanded)++;
        if (cell == goal) {return reconstruct_path(parents, start, goal, path);}
        for (direction = 0; direction < 4; direction++) {
            int next = neighbor(cell, direction);
            if (walkable(grid, next) && !seen[next]) {
                seen[next] = 1;
                parents[next] = cell;
                queue[tail++] = next;
            }
        }
    }
    return -1;
}

float maze_heuristic(int cell, int goal)
{
    float dx = (float)(cell % COLS - goal % COLS);
    float dy = (float)(cell / COLS - goal / COLS);
    return sqrtf(dx * dx + dy * dy);
}

int astar_solve(const char grid[ROWS][COLS + 1], int start, int goal,
    int path[CELLS], unsigned int *expanded)
{
    int parents[CELLS], cost[CELLS];
    float estimate[CELLS];
    unsigned char open[CELLS], closed[CELLS];
    unsigned int direction;
    int i;

    *expanded = 0;
    if (!walkable(grid, start) || !walkable(grid, goal)) {return -1;}
    for (i = 0; i < CELLS; i++) {
        parents[i] = -1;
        cost[i] = CELLS + 1;
        estimate[i] = FLT_MAX;
        open[i] = 0;
        closed[i] = 0;
    }
    cost[start] = 0;
    estimate[start] = maze_heuristic(start, goal);
    open[start] = 1;
    for (;;) {
        int cell = -1;
        float best = FLT_MAX;
        for (i = 0; i < CELLS; i++) {
            if (open[i] && estimate[i] < best) {
                cell = i;
                best = estimate[i];
            }
        }
        if (cell < 0) {return -1;}
        open[cell] = 0;
        closed[cell] = 1;
        (*expanded)++;
        if (cell == goal) {return reconstruct_path(parents, start, goal, path);}
        for (direction = 0; direction < 4; direction++) {
            int next = neighbor(cell, direction);
            if (walkable(grid, next) && !closed[next] && cost[cell] + 1 < cost[next]) {
                parents[next] = cell;
                cost[next] = cost[cell] + 1;
                estimate[next] = (float)cost[next] + maze_heuristic(next, goal);
                open[next] = 1;
            }
        }
    }
}

static int valid_path(const char grid[ROWS][COLS + 1], const int path[CELLS],
    int distance, int start, int goal)
{
    int i;
    if (distance < 0 || distance >= CELLS || path[0] != start || path[distance] != goal) {return 0;}
    for (i = 0; i <= distance; i++) {
        if (!walkable(grid, path[i])) {return 0;}
        if (i != 0) {
            unsigned int direction;
            int adjacent = 0;
            for (direction = 0; direction < 4; direction++) {
                if (neighbor(path[i-1], direction) == path[i]) {adjacent = 1;}
            }
            if (!adjacent) {return 0;}
        }
    }
    return 1;
}

int main(void)
{
    char blocked[ROWS][COLS + 1];
    int scratch[CELLS], row, col;
    unsigned int expanded;

    bfs_distance = bfs_solve(maze, START, GOAL, bfs_path, &bfs_expanded);
    astar_distance = astar_solve(maze, START, GOAL, astar_path, &astar_expanded);
    if (bfs_distance != 10 || astar_distance != bfs_distance) {return 1;}
    if (!valid_path(maze, bfs_path, bfs_distance, START, GOAL)) {return 2;}
    if (!valid_path(maze, astar_path, astar_distance, START, GOAL)) {return 3;}
    if (bfs_solve(maze, START, START, scratch, &expanded) != 0 || scratch[0] != START) {return 4;}
    if (astar_solve(maze, START, START, scratch, &expanded) != 0 || scratch[0] != START) {return 5;}
    if (bfs_solve(maze, START, 0, scratch, &expanded) != -1) {return 6;}
    if (astar_solve(maze, -1, GOAL, scratch, &expanded) != -1) {return 7;}
    for (row = 0; row < ROWS; row++) {
        for (col = 0; col <= COLS; col++) {blocked[row][col] = maze[row][col];}
    }
    blocked[1][2] = '#';
    blocked[2][1] = '#';
    if (bfs_solve(blocked, START, GOAL, scratch, &expanded) != -1) {return 8;}
    if (astar_solve(blocked, START, GOAL, scratch, &expanded) != -1) {return 9;}
    return 0;
}
