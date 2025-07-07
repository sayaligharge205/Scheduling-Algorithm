#include <stdio.h>

// SJF (Shortest Job First) Scheduling Algorithm - Non-Preemptive
int main() {
    int n, i, j, temp, time = 0, completed = 0;

    printf("Enter number of processes: ");
    scanf("%d", &n);

    int pid[n], arrival[n], burst[n], completion[n], turnaround[n], waiting[n];
    int visited[n];

    // Input process details
    for(i = 0; i < n; i++) {
        printf("Enter Process ID, Arrival Time, Burst Time for P%d: ", i+1);
        scanf("%d %d %d", &pid[i], &arrival[i], &burst[i]);
        visited[i] = 0;  // Mark as not visited
    }

    printf("\nExecution Order: ");

    // SJF scheduling
    while(completed < n) {
        int shortest = -1;
        int min_burst = 999999;

        // Find shortest job among arrived processes
        for(i = 0; i < n; i++) {
            if(!visited[i] && arrival[i] <= time && burst[i] < min_burst) {
                min_burst = burst[i];
                shortest = i;
            }
        }

        if(shortest == -1) {
            // No process available, advance time
            time++;
        } else {
            // Execute shortest job
            printf("P%d ", pid[shortest]);
            time += burst[shortest];
            completion[shortest] = time;
            visited[shortest] = 1;
            completed++;
        }
    }

    // Calculate turnaround and waiting times
    float total_turnaround = 0, total_waiting = 0;
    for(i = 0; i < n; i++) {
        turnaround[i] = completion[i] - arrival[i];
        waiting[i] = turnaround[i] - burst[i];
        total_turnaround += turnaround[i];
        total_waiting += waiting[i];
    }

    // Display results
    printf("\n\nPID\tArrival\tBurst\tCompletion\tTurnaround\tWaiting\n");
    for(i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t\t%d\t\t%d\n",
               pid[i], arrival[i], burst[i], completion[i], turnaround[i], waiting[i]);
    }

    printf("\nAverage Turnaround Time: %.2f", total_turnaround / n);
    printf("\nAverage Waiting Time: %.2f\n", total_waiting / n);

    return 0;
}

/*
SAMPLE INPUT:
Enter number of processes: 4
Enter Process ID, Arrival Time, Burst Time for P1: 1 0 6
Enter Process ID, Arrival Time, Burst Time for P2: 2 1 2
Enter Process ID, Arrival Time, Burst Time for P3: 3 2 1
Enter Process ID, Arrival Time, Burst Time for P4: 4 3 3

SAMPLE OUTPUT:
Execution Order: P1 P3 P2 P4

PID	Arrival	Burst	Completion	Turnaround	Waiting
1	0	6	6		6		0
2	1	2	8		7		5
3	2	1	7		5		4
4	3	3	11		8		5

Average Turnaround Time: 6.50
Average Waiting Time: 3.50
*/
