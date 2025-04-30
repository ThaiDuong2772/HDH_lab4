#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define max(a, b) ((a) > (b) ? (a) : (b))
#define SORT_BY_ARRIVAL 0
#define SORT_BY_PID 1
#define SORT_BY_BURST 2
#define SORT_BY_START 3


typedef struct{
    int iPID;
    int iArrival, iBurst;
    int iStart, iFinish, iWaiting, iResponse, iTaT;
} PCB;
void inputProcess(int n, PCB P[]);
void printProcess(int n, PCB P[]);
void exportGanttChart (int n, PCB P[]);
void pushProcess(int *n, PCB P[], PCB Q);
void removeProcess(int *n, int index, PCB P[]);
int swapProcess(PCB *P, PCB *Q);
int partition (PCB P[], int low, int high, int iCriteria);
void quickSort(PCB P[], int low, int high, int iCriteria);
void calculateAWT(int n, PCB P[]);
void calculateATaT(int n, PCB P[]);
int main()
{
    PCB Input[10];
    PCB ReadyQueue[10];
    PCB TerminatedArray[10];
    int iNumberOfProcess;
    printf("Please input number of Process: ");
    scanf("%d", &iNumberOfProcess);
    int iRemain = iNumberOfProcess, iReady = 0, iTerminated = 0;
    inputProcess(iNumberOfProcess, Input);
    printf("===== Input Process =====\n");
    printProcess(iNumberOfProcess, Input);
    quickSort(Input, 0, iNumberOfProcess - 1, SORT_BY_ARRIVAL);
    int minArrival = Input[0].iArrival;
    int i = 0;
    while (Input[i].iArrival == minArrival)
    {
        i++;
    }
    quickSort(Input, 0, i - 1, SORT_BY_BURST);
    Input[0].iStart = minArrival;
    Input[0].iFinish = Input[0].iStart + Input[0].iBurst;
    Input[0].iResponse = Input[0].iStart - 0;
    Input[0].iWaiting = Input[0].iResponse;
    Input[0].iTaT = Input[0].iFinish - Input[0].iArrival;
    pushProcess(&iTerminated, TerminatedArray, Input[0]);
    removeProcess(&iRemain, 0, Input);
    bool flag = true; // Use true and false from <stdbool.h>
    while (iTerminated < iNumberOfProcess)
    {
        while (iRemain > 0)
        {   
            if (Input[0].iArrival <= TerminatedArray[iTerminated-1].iFinish || flag)
            {
                flag = false;
                pushProcess(&iReady, ReadyQueue, Input[0]);
                removeProcess(&iRemain, 0, Input);
            }
            if ((Input[0].iArrival > TerminatedArray[iTerminated-1].iFinish || iRemain == 0) && iReady > 0)
            {
                quickSort(ReadyQueue, 0, iReady - 1, SORT_BY_BURST);
                while (iReady > 0)
                {
                    ReadyQueue[0].iStart = max(TerminatedArray[iTerminated - 1].iFinish, ReadyQueue[0].iArrival);
                    ReadyQueue[0].iFinish = ReadyQueue[0].iStart + ReadyQueue[0].iBurst;
                    ReadyQueue[0].iResponse = ReadyQueue[0].iStart - ReadyQueue[0].iArrival;
                    ReadyQueue[0].iWaiting = ReadyQueue[0].iResponse;
                    ReadyQueue[0].iTaT = ReadyQueue[0].iFinish - ReadyQueue[0].iArrival;
                    pushProcess(&iTerminated, TerminatedArray, ReadyQueue[0]);
                    removeProcess(&iReady, 0, ReadyQueue);
                }
                flag = true;
            }
        }
        //printProcess(iReady, ReadyQueue);
    }
    printf("\n===== SJF Scheduling =====\n");
    exportGanttChart(iTerminated, TerminatedArray);
    printf("==========================\n");
    //quickSort(TerminatedArray, 0, iTerminated - 1, SORT_BY_PID);
    calculateAWT(iTerminated, TerminatedArray);
    calculateATaT(iTerminated, TerminatedArray);
    return 0;
}

void inputProcess(int n, PCB P[])
{
    srand(time(NULL));
    for (int i = 0; i < n; i++)
    {
        P[i].iPID = i + 1;
        P[i].iArrival = rand()%21; // Random arrival time between 0 and 20
        P[i].iBurst = rand() % 11 + 2; // Random burst time between 2 and 12
    }
}

void printProcess(int n, PCB P[])
{
    for (int i = 0; i < n; i++)
    {
        printf("Process ID: %d, Arrival Time: %d, Burst Time: %d\n",
        P[i].iPID, P[i].iArrival, P[i].iBurst);
    }
}

void exportGanttChart (int n, PCB P[])
{
    printf("Gantt Chart: ");
    for (int i = 0; i < n; i++)
    {
        printf("| P%d ", P[i].iPID);
    }
    printf("|\n");
    printf("Time: %d ", P[0].iStart);
    for (int i = 0; i < n; i++)
    {
        printf("%d ", P[i].iFinish);
    }
    printf("\n");
}

void pushProcess(int *n, PCB P[], PCB Q)
{
    P[*n] = Q;
    (*n)++;
}

void removeProcess(int *n, int index, PCB P[])
{
    for (int i = index; i < *n - 1; i++)
    {
        P[i] = P[i + 1];
    }
    (*n)--;
}

int swapProcess(PCB *P, PCB *Q)
{
    PCB temp = *P;
    *P = *Q;
    *Q = temp;
}

int partition (PCB P[], int low, int high, int iCriteria)
{
    PCB pivot = P[high];
    int i = (low - 1);
    for (int j = low; j < high; j++)
    {
        if ((iCriteria == SORT_BY_ARRIVAL && P[j].iArrival <= pivot.iArrival) ||
            (iCriteria == SORT_BY_PID && P[j].iPID <= pivot.iPID) ||
            (iCriteria == SORT_BY_BURST && P[j].iBurst <= pivot.iBurst) ||
            (iCriteria == SORT_BY_START && P[j].iStart <= pivot.iStart))
        {
            i++;
            swapProcess(&P[i], &P[j]);
        }
    }
    swapProcess(&P[i + 1], &P[high]);
    return (i + 1);
}

void quickSort(PCB P[], int low, int high, int iCriteria)
{
    if (low < high)
    {
        int pi = partition(P, low, high, iCriteria);
        quickSort(P, low, pi - 1, iCriteria);
        quickSort(P, pi + 1, high, iCriteria);
    }
}

void calculateAWT(int n, PCB P[])
{
    float fTotalWaitingTime = P[0].iWaiting;
    printf("Sum of Waiting Time: %d ", P[0].iWaiting);
    for (int i = 1; i < n; i++)
    {
        printf("+ %d ", P[i].iWaiting);
        fTotalWaitingTime += P[i].iWaiting;
    }
    printf("\n");
    printf("Average Waiting Time: %.2f\n", fTotalWaitingTime / n);
}

void calculateATaT(int n, PCB P[])
{
    float fTotalTaT = P[0].iTaT;
    printf("Sum of Turnaround Time: %d ", P[0].iTaT);
    for (int i = 1; i < n; i++)
    {
        printf("+ %d ", P[i].iTaT);
        fTotalTaT += P[i].iTaT;
    }
    printf("\n");
    printf("Average Turnaround Time: %.2f\n", fTotalTaT / n);
}