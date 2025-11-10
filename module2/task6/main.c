#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PRIORITY 255

typedef struct Node {
    char data[64];
    int priority;
    struct Node* next;
} Node;

typedef struct {
    Node* heads[MAX_PRIORITY + 1];
    Node* tails[MAX_PRIORITY + 1];
} PriorityQueue;

void initQueue(PriorityQueue* q) {
    for (int i = 0; i <= MAX_PRIORITY; i++) {
        q->heads[i] = NULL;
        q->tails[i] = NULL;
    }
}

void enqueue(PriorityQueue* q, const char* data, int priority) {
    if (priority < 0 || priority > MAX_PRIORITY) {
        printf("Unknown priority %d\n", priority);
        return;
    }

    Node* newNode = malloc(sizeof(Node));
    strcpy(newNode->data, data);
    newNode->priority = priority;
    newNode->next = NULL;

    if (q->tails[priority])
        q->tails[priority]->next = newNode;
    else
        q->heads[priority] = newNode;

    q->tails[priority] = newNode;
}

Node* dequeueFirst(PriorityQueue* q) {
    for (int i = 0; i <= MAX_PRIORITY; i++) {
        if (q->heads[i]) {
            Node* node = q->heads[i];
            q->heads[i] = node->next;
            if (!q->heads[i]) q->tails[i] = NULL;
            char data = node->data;
            return node;
        }
    }
    return NULL;
}

Node* dequeueByPriority(PriorityQueue* q, int priority) {
    if (priority < 0 || priority > MAX_PRIORITY) return NULL;
    Node* node = q->heads[priority];
    if (!node) return NULL;

    q->heads[priority] = node->next;
    if (!q->heads[priority]) q->tails[priority] = NULL;
    return node;
}

Node* dequeueMinPriority(PriorityQueue* q, int maxPriority) {
    if (maxPriority < 0) maxPriority = 0;
    if (maxPriority > MAX_PRIORITY) maxPriority = MAX_PRIORITY;

    for (int i = 0; i <= maxPriority; i++) {
        if (q->heads[i]) {
            Node* node = q->heads[i];
            q->heads[i] = node->next;
            if (!q->heads[i]) q->tails[i] = NULL;
            return node;
        }
    }
    return NULL;
}

void freeQueue(PriorityQueue* q) {
    for (int i = 0; i <= MAX_PRIORITY; i++) {
        Node* node = q->heads[i];
        while (node) {
            Node* tmp = node;
            node = node->next;
            free(tmp);
        }
    }
}

int main() {
    PriorityQueue q;
    initQueue(&q);

    enqueue(&q, "A", 5);
    enqueue(&q, "B", 2);
    enqueue(&q, "C", 10);
    enqueue(&q, "D", 2);
    enqueue(&q, "E", 0);

    printf("Извлечение первого элемента (самого приоритетного):\n");
    Node* n = dequeueFirst(&q);
    if (n) {
        printf("  %s (приоритет %d)\n", n->data, n->priority);
        free(n);
    }

    printf("\nИзвлечение элемента с приоритетом = 2:\n");
    n = dequeueByPriority(&q, 2);
    if (n) {
        printf("  %s (приоритет %d)\n", n->data, n->priority);
        free(n);
    }

    printf("\nИзвлечение элемента с приоритетом не ниже 5:\n");
    n = dequeueMinPriority(&q, 5);
    if (n) {
        printf("  %s (приоритет %d)\n", n->data, n->priority);
        free(n);
    }

    printf("\nОставшиеся элементы (в порядке приоритетов):\n");
    while ((n = dequeueFirst(&q)) != NULL) {
        printf("  %s (приоритет %d)\n", n->data, n->priority);
        free(n);
    }

    freeQueue(&q);
    return 0;
}
