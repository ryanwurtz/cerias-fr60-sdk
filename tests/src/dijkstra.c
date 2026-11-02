#include <stdint.h>

#define VERTICES 7
#define NO_EDGE UINT32_MAX

uint32_t dijkstra_distances[VERTICES];
int dijkstra_parents[VERTICES];

static const uint32_t graph[VERTICES][VERTICES] = {
    {0,7,9,NO_EDGE,NO_EDGE,14,NO_EDGE},
    {7,0,10,15,NO_EDGE,NO_EDGE,NO_EDGE},
    {9,10,0,11,NO_EDGE,2,NO_EDGE},
    {NO_EDGE,15,11,0,6,NO_EDGE,NO_EDGE},
    {NO_EDGE,NO_EDGE,NO_EDGE,6,0,9,NO_EDGE},
    {14,NO_EDGE,2,NO_EDGE,9,0,NO_EDGE},
    {NO_EDGE,NO_EDGE,NO_EDGE,NO_EDGE,NO_EDGE,NO_EDGE,0}
};

int dijkstra(const uint32_t weights[VERTICES][VERTICES], unsigned int source,
    uint32_t distances[VERTICES], int parents[VERTICES])
{
    unsigned char visited[VERTICES];
    unsigned int i, step;

    if (source >= VERTICES) {return -1;}
    for (i = 0; i < VERTICES; i++) {
        distances[i] = NO_EDGE;
        parents[i] = -1;
        visited[i] = 0;
    }
    distances[source] = 0;
    for (step = 0; step < VERTICES; step++) {
        int nearest = -1;
        uint32_t best = NO_EDGE;
        for (i = 0; i < VERTICES; i++) {
            if (!visited[i] && distances[i] < best) {
                nearest = (int)i;
                best = distances[i];
            }
        }
        if (nearest < 0) {break;}
        visited[nearest] = 1;
        for (i = 0; i < VERTICES; i++) {
            uint32_t weight = weights[nearest][i];
            if (!visited[i] && weight != NO_EDGE && best < NO_EDGE - weight) {
                uint32_t candidate = best + weight;
                if (candidate < distances[i]) {
                    distances[i] = candidate;
                    parents[i] = nearest;
                }
            }
        }
    }
    return 0;
}

int main(void)
{
    const uint32_t expected[VERTICES] = {0,7,9,20,20,11,NO_EDGE};
    uint32_t weights[VERTICES][VERTICES];
    uint32_t distances[VERTICES];
    int parents[VERTICES];
    unsigned int i, j;

    dijkstra(graph, 0, dijkstra_distances, dijkstra_parents);
    for (i = 0; i < VERTICES; i++) {
        if (dijkstra_distances[i] != expected[i]) {return 1;}
        if (i != 0 && i != 6) {
            int p = dijkstra_parents[i];
            if (p < 0 || p >= VERTICES || graph[p][i] == NO_EDGE ||
                dijkstra_distances[p] + graph[p][i] != expected[i]) {return 2;}
        }
    }
    if (dijkstra_parents[0] != -1 || dijkstra_parents[6] != -1) {return 3;}
    for (i = 0; i < VERTICES; i++) {
        for (j = 0; j < VERTICES; j++) {weights[i][j] = NO_EDGE;}
    }
    weights[0][1] = 0;
    weights[1][2] = 3;
    weights[0][3] = UINT32_MAX - 2;
    weights[3][4] = 5;
    dijkstra(weights, 0, distances, parents);
    if (distances[1] != 0 || distances[2] != 3) {return 4;}
    if (distances[3] != UINT32_MAX - 2 || distances[4] != NO_EDGE) {return 5;}
    if (dijkstra(graph, VERTICES, distances, parents) != -1) {return 6;}
    return 0;
}
