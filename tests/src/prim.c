#include <float.h>
#include <math.h>

#define MAX_POINTS 6

struct point {
    float x;
    float y;
};

int prim_parents[MAX_POINTS];
float prim_edges[MAX_POINTS];
float prim_total;

float point_distance(const struct point *a, const struct point *b)
{
    float dx = a->x - b->x;
    float dy = a->y - b->y;
    return sqrtf(dx * dx + dy * dy);
}

float prim_mst(const struct point *points, unsigned int count,
    int *parents, float *edges)
{
    unsigned char included[MAX_POINTS];
    float total = 0.0f;
    unsigned int i, step;

    if (count == 0 || count > MAX_POINTS) {return -1.0f;}
    for (i = 0; i < count; i++) {
        parents[i] = -1;
        edges[i] = FLT_MAX;
        included[i] = 0;
    }
    edges[0] = 0.0f;
    for (step = 0; step < count; step++) {
        int nearest = -1;
        float best = FLT_MAX;
        for (i = 0; i < count; i++) {
            if (!included[i] && edges[i] < best) {
                best = edges[i];
                nearest = (int)i;
            }
        }
        if (nearest < 0) {return -1.0f;}
        included[nearest] = 1;
        total += best;
        for (i = 0; i < count; i++) {
            if (!included[i]) {
                float distance = point_distance(&points[nearest], &points[i]);
                if (distance < edges[i]) {
                    edges[i] = distance;
                    parents[i] = nearest;
                }
            }
        }
    }
    return total;
}

int main(void)
{
    const struct point points[MAX_POINTS] = {
        {0.0f,0.0f}, {3.0f,0.0f}, {0.0f,4.0f},
        {3.0f,4.0f}, {1.5f,2.0f}, {3.0f,7.0f}
    };
    const struct point duplicate[2] = {{2.0f,3.0f}, {2.0f,3.0f}};
    int parents[MAX_POINTS];
    float edges[MAX_POINTS];
    unsigned int i;

    prim_total = prim_mst(points, MAX_POINTS, prim_parents, prim_edges);
    if (fabsf(prim_total - 13.0f) > 0.0001f) {return 1;}
    if (prim_parents[0] != -1 || prim_edges[0] != 0.0f) {return 2;}
    for (i = 1; i < MAX_POINTS; i++) {
        int p = prim_parents[i];
        unsigned int vertex = i, steps = 0;
        if (p < 0 || p >= MAX_POINTS ||
            fabsf(prim_edges[i] - point_distance(&points[i], &points[p])) > 0.0001f) {return 3;}
        while (vertex != 0 && steps < MAX_POINTS) {
            int parent = prim_parents[vertex];
            if (parent < 0 || parent >= MAX_POINTS) {return 4;}
            vertex = (unsigned int)parent;
            steps++;
        }
        if (vertex != 0) {return 4;}
    }
    if (prim_mst(points, 1, parents, edges) != 0.0f) {return 5;}
    if (prim_mst(duplicate, 2, parents, edges) != 0.0f) {return 6;}
    if (prim_mst(points, 0, parents, edges) != -1.0f) {return 7;}
    return 0;
}
