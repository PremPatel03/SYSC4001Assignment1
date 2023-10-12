#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

enum PCBState {
    READY,      // 0
    RUNNING,    // 1
    WAITING,    // 2
    NEW,        // 3
    TERMINATED  // 4
};

struct PCB {
    int PID;
    int ArrivalTime;
    int TotalCPUTime;
    int IOFrequency;
    int IODuration;
    int RemainingCPUTime;
    enum PCBState State;
    struct PCB* next;
};

struct PCB* createPCB(int PID, int ArrivalTime, int TotalCPUTime, int IOFrequency, int IODuration) {
    struct PCB* newNode = (struct PCB*)malloc(sizeof(struct PCB));
    if (newNode == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    newNode->PID = PID;
    newNode->ArrivalTime = ArrivalTime;
    newNode->TotalCPUTime = TotalCPUTime;
    newNode->IOFrequency = IOFrequency;
    newNode->IODuration = IODuration;
    newNode->RemainingCPUTime = TotalCPUTime;
    newNode->State = (ArrivalTime == 0) ? NEW : READY; // Use enum values
    newNode->next = NULL;
    return newNode;
}

void insertPCB(struct PCB** head, int PID, int ArrivalTime, int TotalCPUTime, int IOFrequency, int IODuration) {
    struct PCB* newNode = createPCB(PID, ArrivalTime, TotalCPUTime, IOFrequency, IODuration);
    if (*head == NULL) {
        *head = newNode;
    } else {
        struct PCB* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
}

void printTransition(int currentTime, struct PCB* pcb, int oldState, int newState) {
    char* state_str[5] = {"Ready", "Running", "Waiting", "New", "Terminated"};
    printf("Time %d PID %d %s %s\n", currentTime, pcb->PID, state_str[oldState], state_str[newState]);
}

// Separate PCBs into different lists based on their state
struct PCB* readyQueue = NULL;
struct PCB* runningQueue = NULL;
struct PCB* waitingQueue = NULL;
struct PCB* newQueue = NULL;
struct PCB* terminatedQueue = NULL;

// Helper function to move a PCB from one list to another
void movePCB(struct PCB** source, struct PCB** destination, struct PCB* pcb) {
    if (*source == pcb) {
        *source = pcb->next;
    } else {
        struct PCB* current = *source;
        while (current != NULL && current->next != pcb) {
            current = current->next;
        }
        if (current != NULL) {
            current->next = pcb->next;
        }
    }
    pcb->next = *destination;
    *destination = pcb;
}

// Helper function to move a PCB from New to Ready
void moveNewToReady(struct PCB* pcb) {
    movePCB(&newQueue, &readyQueue, pcb);
    pcb->State = 0; // Ready
}

void updateState(int currentTime) {
    struct PCB* current = readyQueue;
    while (current != NULL) {
        current->RemainingCPUTime--;
        if (current->RemainingCPUTime == 0) {
            movePCB(&readyQueue, &terminatedQueue, current);
            current->State = TERMINATED;
        }
        current = current->next;
    }

    current = runningQueue;
    while (current != NULL) {
        current->RemainingCPUTime--;
        if (current->RemainingCPUTime == 0) {
            movePCB(&runningQueue, &waitingQueue, current);
            current->State = WAITING;
        }
        current = current->next;
    }

    current = waitingQueue;
    while (current != NULL) {
        // Handle I/O events if needed
        // ...
        current = current->next;
    }

    current = newQueue;
    while (current != NULL) {
        if (current->ArrivalTime <= currentTime) {
            moveNewToReady(current);
        }
        current = current->next;
    }
}

void freePCBs(struct PCB* queue) {
    struct PCB* current = queue;
    while (current != NULL) {
        struct PCB* temp = current;
        current = current->next;
        free(temp);
    }
}

int main() {
    int currentTime = 0;
    struct PCB* currentProcess = NULL;

    FILE* file = fopen("test_case_1.csv", "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[256]; // Assuming lines in the CSV file won't exceed 255 characters

    while (fgets(line, sizeof(line), file)) {
        int PID, ArrivalTime, TotalCPUTime, IOFrequency, IODuration;
        char* token = strtok(line, ",");
        if (token != NULL) {
            PID = atoi(token);
            token = strtok(NULL, ",");
            ArrivalTime = atoi(token);
            token = strtok(NULL, ",");
            TotalCPUTime = atoi(token);
            token = strtok(NULL, ",");
            IOFrequency = atoi(token);
            token = strtok(NULL, ",");
            IODuration = atoi(token);
            insertPCB(&newQueue, PID, ArrivalTime, TotalCPUTime, IOFrequency, IODuration);
        }
    }

    fclose(file);

    while (readyQueue != NULL || runningQueue != NULL || waitingQueue != NULL || newQueue != NULL) {
        updateState(currentTime);

        // Determine which process is in the Running state
        currentProcess = runningQueue;

        if (currentProcess != NULL) {
            currentProcess->RemainingCPUTime--;
            if (currentProcess->RemainingCPUTime == 0) {
                movePCB(&runningQueue, &waitingQueue, currentProcess);
                currentProcess->State = WAITING;
            }
        }

        // Simulate a shorter delay (10 milliseconds)
        usleep(10000);
        currentTime++;
    }

    // Free memory
    struct PCB* queues[] = {readyQueue, runningQueue, waitingQueue, newQueue, terminatedQueue};

    for (int i = 0; i < 5; i++) {
        freePCBs(queues[i]);
    }

    return 0;
}

