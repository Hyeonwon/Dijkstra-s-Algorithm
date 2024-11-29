#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

// 인접 리스트 노드를 나타내는 구조체
struct AdjListNode {
    int dest; // 목적지 정점
    int weight; // 간선의 가중치
    struct AdjListNode* next; // 다음 노드를 가리키는 포인터
};

// 인접 리스트를 나타내는 구조체
struct AdjList {
    struct AdjListNode* head; // 인접 리스트의 헤드 포인터
};

// 그래프를 나타내는 구조체
struct Graph {
    int V; // 그래프의 정점 개수
    struct AdjList* array; // 정점에 대한 인접 리스트 배열
};

// 최소 힙 노드를 나타내는 구조체
struct MinHeapNode {
    int v; // 정점 번호
    int dist; // 현재까지의 최단 거리
};

// 최소 힙을 나타내는 구조체
struct MinHeap {
    int size; // 현재 힙에 있는 요소의 수
    int capacity; // 힙의 용량
    int* pos; // 정점의 위치를 저장하는 배열
    struct MinHeapNode** array; // 힙 노드의 배열
};

// 새로운 인접 리스트 노드를 생성하는 함수
// 시간 복잡도: O(1)
struct AdjListNode* newAdjListNode(int dest, int weight) {
    struct AdjListNode* newNode = (struct AdjListNode*)malloc(sizeof(struct AdjListNode));
    newNode->dest = dest;
    newNode->weight = weight;
    newNode->next = NULL;
    return newNode;
}

// 주어진 정점 개수 V에 대해 그래프를 생성하는 함수
// 시간 복잡도: O(V)
struct Graph* createGraph(int V) {
    struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph));
    graph->V = V;
    graph->array = (struct AdjList*)malloc(V * sizeof(struct AdjList));
    for (int i = 0; i < V; ++i)
        graph->array[i].head = NULL;
    return graph;
}

// 그래프에 간선을 추가하는 함수
// 시간 복잡도: O(1)
void addEdge(struct Graph* graph, int src, int dest, int weight) {
    struct AdjListNode* newNode = newAdjListNode(dest, weight);
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;
}

// 새로운 최소 힙 노드를 생성하는 함수
// 시간 복잡도: O(1)
struct MinHeapNode* newMinHeapNode(int v, int dist) {
    struct MinHeapNode* minHeapNode = (struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
    minHeapNode->v = v;
    minHeapNode->dist = dist;
    return minHeapNode;
}

// 주어진 용량을 가지는 최소 힙을 생성하는 함수
// 시간 복잡도: O(1)
struct MinHeap* createMinHeap(int capacity) {
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
    minHeap->pos = (int*)malloc(capacity * sizeof(int));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (struct MinHeapNode**)malloc(capacity * sizeof(struct MinHeapNode*));
    return minHeap;
}

// 두 개의 최소 힙 노드를 교환하는 함수
// 시간 복잡도: O(1)
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b) {
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

// 주어진 인덱스에서 최소 힙 속성을 유지하도록 힙을 재구성하는 함수
// 시간 복잡도: O(log V)
void minHeapify(struct MinHeap* minHeap, int idx) {
    int smallest, left, right;
    smallest = idx;
    left = 2 * idx + 1;
    right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->dist < minHeap->array[smallest]->dist)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right]->dist < minHeap->array[smallest]->dist)
        smallest = right;

    if (smallest != idx) {
        struct MinHeapNode* smallestNode = minHeap->array[smallest];
        struct MinHeapNode* idxNode = minHeap->array[idx];

        minHeap->pos[smallestNode->v] = idx;
        minHeap->pos[idxNode->v] = smallest;

        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);

        minHeapify(minHeap, smallest);
    }
}

// 최소 힙이 비어 있는지 확인하는 함수
// 시간 복잡도: O(1)
int isEmpty(struct MinHeap* minHeap) {
    return minHeap->size == 0;
}

// 최소 힙에서 최솟값을 추출하고 힙을 재구성하는 함수
// 시간 복잡도: O(log V)
struct MinHeapNode* extractMin(struct MinHeap* minHeap) {
    if (isEmpty(minHeap))
        return NULL;

    struct MinHeapNode* root = minHeap->array[0];
    struct MinHeapNode* lastNode = minHeap->array[minHeap->size - 1];
    minHeap->array[0] = lastNode;

    minHeap->pos[root->v] = minHeap->size - 1;
    minHeap->pos[lastNode->v] = 0;

    --minHeap->size;
    minHeapify(minHeap, 0);

    return root;
}

// 주어진 정점 v의 거리를 새로운 값으로 업데이트하고 최소 힙 속성을 유지하는 함수
// 시간 복잡도: O(log V)
void decreaseKey(struct MinHeap* minHeap, int v, int dist) {
    int i = minHeap->pos[v];
    minHeap->array[i]->dist = dist;

    while (i && minHeap->array[i]->dist < minHeap->array[(i - 1) / 2]->dist) {
        minHeap->pos[minHeap->array[i]->v] = (i - 1) / 2;
        minHeap->pos[minHeap->array[(i - 1) / 2]->v] = i;
        swapMinHeapNode(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

// 주어진 정점이 최소 힙에 있는지 확인하는 함수
// 시간 복잡도: O(1)
int isInMinHeap(struct MinHeap* minHeap, int v) {
    if (minHeap->pos[v] < minHeap->size)
        return 1;
    return 0;
}

// 새로운 노드를 힙에 삽입하는 함수 (Insert 함수)
// 시간 복잡도: O(log V)
void insertMinHeap(struct MinHeap* minHeap, int v, int dist) {
    struct MinHeapNode* newNode = newMinHeapNode(v, dist);
    minHeap->array[minHeap->size] = newNode;
    minHeap->pos[v] = minHeap->size;
    minHeap->size++;
    decreaseKey(minHeap, v, dist);
}

// 다익스트라 알고리즘을 사용하여 최단 경로를 찾는 함수
// 시간 복잡도: O((V + E) log V)
void dijkstra(struct Graph* graph, int src, FILE* outputFile) {
    int V = graph->V;
    int dist[V]; // 최단 거리를 저장할 배열
    int prev[V]; // 이전 정점을 저장할 배열
    struct MinHeap* minHeap = createMinHeap(V);

    // 초기화
    for (int v = 0; v < V; ++v) {
        dist[v] = INT_MAX; // 초기 거리는 무한대
        prev[v] = -1; // 이전 정점은 -1로 초기화
        insertMinHeap(minHeap, v, dist[v]); // 각 정점을 힙에 삽입
    }

    // 시작 정점의 거리와 힙 위치를 설정
    dist[src] = 0; // 시작 정점의 거리는 0
    decreaseKey(minHeap, src, dist[src]);

    // 최소 힙이 비어 있지 않은 동안 반복
    while (!isEmpty(minHeap)) {
        // 최솟값 추출
        struct MinHeapNode* minHeapNode = extractMin(minHeap);
        int u = minHeapNode->v;

        // 인접한 모든 정점을 확인
        struct AdjListNode* pCrawl = graph->array[u].head;
        while (pCrawl != NULL) {
            int v = pCrawl->dest;

            // 만약 새로운 경로가 더 짧다면 거리와 이전 정점을 업데이트
            if (isInMinHeap(minHeap, v) && dist[u] != INT_MAX && pCrawl->weight + dist[u] < dist[v]) {
                dist[v] = dist[u] + pCrawl->weight;
                prev[v] = u;
                decreaseKey(minHeap, v, dist[v]);
            }
            pCrawl = pCrawl->next;
        }
    }

    // 결과를 파일에 출력
    for (int i = 0; i < V; ++i) {
        if (dist[i] == INT_MAX)
            fprintf(outputFile, "%d\tINFINITY\tNIL\n", i);
        else if (prev[i] == -1)
            fprintf(outputFile, "%d\t%d\tNIL\n", i, dist[i]);
        else
            fprintf(outputFile, "%d\t%d\t%d\n", i, dist[i], prev[i]);
    }
}

int main() {
    // 입력 파일을 엽니다.
    FILE* inputFile = fopen("input.txt", "r");
    if (inputFile == NULL) {
        perror("Error opening input file");
        return 1;
    }

    // 출력 파일을 엽니다.
    FILE* outputFile = fopen("output.txt", "w");
    if (outputFile == NULL) {
        perror("Error opening output file");
        fclose(inputFile);
        return 1;
    }

    int V, E, src;
    fscanf(inputFile, "%d\t%d\t%d", &V, &E, &src);

    struct Graph* graph = createGraph(V);

    for (int i = 0; i < E; ++i) {
        int u, v, w;
        fscanf(inputFile, "%d\t%d\t%d", &u, &v, &w);
        addEdge(graph, u, v, w);
    }

    // 다익스트라 알고리즘을 호출하여 최단 경로를 계산하고 결과를 파일에 저장합니다.
    dijkstra(graph, src, outputFile);

    fclose(inputFile);
    fclose(outputFile);

    return 0;
}
