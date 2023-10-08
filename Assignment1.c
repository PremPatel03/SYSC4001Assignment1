#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct PCB {
    int PID;
    int ArrivalTime;
    int TotalCPUTime;
    int IOFrequency;
    int IODuration;
    int RemainingCPUTime;
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

void printPCBList(struct PCB* head) {
    struct PCB* current = head;
    while (current != NULL) {
        printf("PID: %d, Arrival Time: %d, Total CPU Time: %d, I/O Frequency: %d, I/O Duration: %d, Remaining CPU Time: %d\n",
               current->PID, current->ArrivalTime, current->TotalCPUTime, current->IOFrequency, current->IODuration, current->RemainingCPUTime);
        current = current->next;
    }
}



int main() {
    struct PCB* head = NULL;

    FILE* file = fopen("test_case_1.csv", "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[256]; // Assuming lines in the CSV file won't exceed 255 characters

    while (fgets(line, sizeof(line), file)) {
        // Use strtok to split the line into values based on comma delimiter
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
            insertPCB(&head, PID, ArrivalTime, TotalCPUTime, IOFrequency, IODuration);
        }
    }

    fclose(file);

    printPCBList(head);

    struct PCB* current = head;
    while (current != NULL) {
        struct PCB* temp = current;
        current = current->next;
        free(temp);
    }

    return 0;
}
  