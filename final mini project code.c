#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESSES 10

typedef struct {
    int pid;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int completion_time;
    int turnaround_time;
    int waiting_time;
    int response_time;
    int started;
} Process;

// Function prototypes
void fcfs(Process processes[], int n);
void sjf(Process processes[], int n);
void round_robin(Process processes[], int n, int quantum);
void calculate_metrics(Process processes[], int n);
void display_table(Process processes[], int n, const char* algorithm);
void display_comparison(Process fcfs_proc[], Process sjf_proc[], Process rr_proc[], int n);
void copy_processes(Process dest[], Process src[], int n);
void reset_processes(Process processes[], int n);

int main() {
    int n, quantum;

    printf("=== CPU Scheduling Algorithm Comparison ===\n\n");

    printf("Enter number of processes: ");
    scanf("%d", &n);

    Process original[MAX_PROCESSES];
    Process fcfs_proc[MAX_PROCESSES];
    Process sjf_proc[MAX_PROCESSES];
    Process rr_proc[MAX_PROCESSES];

    // Input process details
    printf("\nEnter process details:\n");
    for(int i = 0; i < n; i++) {
        original[i].pid = i + 1;
        printf("Process P%d:\n", original[i].pid);
        printf("  Arrival Time: ");
        scanf("%d", &original[i].arrival_time);
        printf("  Burst Time: ");
        scanf("%d", &original[i].burst_time);
        original[i].remaining_time = original[i].burst_time;
        original[i].started = 0;
        printf("\n");
    }

    printf("Enter time quantum for Round Robin: ");
    scanf("%d", &quantum);

    // Copy processes for each algorithm
    copy_processes(fcfs_proc, original, n);
    copy_processes(sjf_proc, original, n);
    copy_processes(rr_proc, original, n);

    printf("\n");
    printf("SCHEDULING ALGORITHM RESULTS\n");

    // Execute algorithms
    fcfs(fcfs_proc, n);
    display_table(fcfs_proc, n, "First Come First Serve (FCFS)");

    sjf(sjf_proc, n);
    display_table(sjf_proc, n, "Shortest Job First (SJF)");

    round_robin(rr_proc, n, quantum);
    display_table(rr_proc, n, "Round Robin (RR)");

    // Display comparison
    display_comparison(fcfs_proc, sjf_proc, rr_proc, n);

    return 0;
}

void fcfs(Process processes[], int n) {
    // Sort by arrival time
    for(int i = 0; i < n - 1; i++) {
        for(int j = i + 1; j < n; j++) {
            if(processes[i].arrival_time > processes[j].arrival_time) {
                Process temp = processes[i];
                processes[i] = processes[j];
                processes[j] = temp;
            }
        }
    }

    int current_time = 0;

    for(int i = 0; i < n; i++) {
        if(current_time < processes[i].arrival_time) {
            current_time = processes[i].arrival_time;
        }

        processes[i].response_time = current_time - processes[i].arrival_time;
        current_time += processes[i].burst_time;
        processes[i].completion_time = current_time;
        processes[i].turnaround_time = processes[i].completion_time - processes[i].arrival_time;
        processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;
    }
}

void sjf(Process processes[], int n) {
    int current_time = 0;
    int completed = 0;
    int is_completed[MAX_PROCESSES] = {0};

    while(completed < n) {
        int shortest = -1;
        int min_burst = 999999;

        // Find shortest job among arrived processes
        for(int i = 0; i < n; i++) {
            if(!is_completed[i] && processes[i].arrival_time <= current_time) {
                if(processes[i].burst_time < min_burst) {
                    min_burst = processes[i].burst_time;
                    shortest = i;
                }
            }
        }

        if(shortest == -1) {
            current_time++;
            continue;
        }

        processes[shortest].response_time = current_time - processes[shortest].arrival_time;
        current_time += processes[shortest].burst_time;
        processes[shortest].completion_time = current_time;
        processes[shortest].turnaround_time = processes[shortest].completion_time - processes[shortest].arrival_time;
        processes[shortest].waiting_time = processes[shortest].turnaround_time - processes[shortest].burst_time;

        is_completed[shortest] = 1;
        completed++;
    }
}

void round_robin(Process processes[], int n, int quantum) {
    int current_time = 0;
    int completed = 0;
    int queue[MAX_PROCESSES * 10];
    int front = 0, rear = 0;
    int in_queue[MAX_PROCESSES] = {0};

    // Add processes that arrive at time 0
    for(int i = 0; i < n; i++) {
        if(processes[i].arrival_time == 0) {
            queue[rear++] = i;
            in_queue[i] = 1;
        }
    }

    while(completed < n) {
        if(front == rear) {
            current_time++;
            // Add newly arrived processes
            for(int i = 0; i < n; i++) {
                if(processes[i].arrival_time == current_time && !in_queue[i] && processes[i].remaining_time > 0) {
                    queue[rear++] = i;
                    in_queue[i] = 1;
                }
            }
            continue;
        }

        int current_process = queue[front++];
        in_queue[current_process] = 0;

        if(!processes[current_process].started) {
            processes[current_process].response_time = current_time - processes[current_process].arrival_time;
            processes[current_process].started = 1;
        }

        int execution_time = (processes[current_process].remaining_time > quantum) ? quantum : processes[current_process].remaining_time;

        processes[current_process].remaining_time -= execution_time;
        current_time += execution_time;

        // Add newly arrived processes
        for(int i = 0; i < n; i++) {
            if(processes[i].arrival_time <= current_time && !in_queue[i] && processes[i].remaining_time > 0 && i != current_process) {
                queue[rear++] = i;
                in_queue[i] = 1;
            }
        }

        if(processes[current_process].remaining_time > 0) {
            queue[rear++] = current_process;
            in_queue[current_process] = 1;
        } else {
            processes[current_process].completion_time = current_time;
            processes[current_process].turnaround_time = processes[current_process].completion_time - processes[current_process].arrival_time;
            processes[current_process].waiting_time = processes[current_process].turnaround_time - processes[current_process].burst_time;
            completed++;
        }
    }
}

void display_table(Process processes[], int n, const char* algorithm) {
    printf("%s\n", algorithm);
    printf("========================================\n");
    printf("| PID | AT | BT | CT | TAT | WT | RT |\n");
    printf("|-----|----|----|----|-----|----|----|\n");

    float total_tat = 0, total_wt = 0, total_rt = 0;

    for(int i = 0; i < n; i++) {
        printf("| P%-2d | %-2d | %-2d | %-2d | %-2d | %-2d | %-2d |\n",
               processes[i].pid,
               processes[i].arrival_time,
               processes[i].burst_time,
               processes[i].completion_time,
               processes[i].turnaround_time,
               processes[i].waiting_time,
               processes[i].response_time);

        total_tat += processes[i].turnaround_time;
        total_wt += processes[i].waiting_time;
        total_rt += processes[i].response_time;
    }

    printf("========================================\n");
    printf("Average Turnaround Time: %.2f\n", total_tat / n);
    printf("Average Waiting Time: %.2f\n", total_wt / n);
    printf("Average Response Time: %.2f\n", total_rt / n);

    // Calculate throughput
    int max_completion = 0;
    for(int i = 0; i < n; i++) {
        if(processes[i].completion_time > max_completion) {
            max_completion = processes[i].completion_time;
        }
    }
    printf("Throughput: %.2f processes/unit time\n\n", (float)n / max_completion);
}

void display_comparison(Process fcfs_proc[], Process sjf_proc[], Process rr_proc[], int n) {
    printf("========================================\n");
    printf("ALGORITHM COMPARISON\n");
    printf("========================================\n");

    // Calculate metrics for each algorithm
    float fcfs_avg_tat = 0, fcfs_avg_wt = 0, fcfs_avg_rt = 0;
    float sjf_avg_tat = 0, sjf_avg_wt = 0, sjf_avg_rt = 0;
    float rr_avg_tat = 0, rr_avg_wt = 0, rr_avg_rt = 0;

    int fcfs_max_ct = 0, sjf_max_ct = 0, rr_max_ct = 0;

    for(int i = 0; i < n; i++) {
        fcfs_avg_tat += fcfs_proc[i].turnaround_time;
        fcfs_avg_wt += fcfs_proc[i].waiting_time;
        fcfs_avg_rt += fcfs_proc[i].response_time;
        if(fcfs_proc[i].completion_time > fcfs_max_ct) fcfs_max_ct = fcfs_proc[i].completion_time;

        sjf_avg_tat += sjf_proc[i].turnaround_time;
        sjf_avg_wt += sjf_proc[i].waiting_time;
        sjf_avg_rt += sjf_proc[i].response_time;
        if(sjf_proc[i].completion_time > sjf_max_ct) sjf_max_ct = sjf_proc[i].completion_time;

        rr_avg_tat += rr_proc[i].turnaround_time;
        rr_avg_wt += rr_proc[i].waiting_time;
        rr_avg_rt += rr_proc[i].response_time;
        if(rr_proc[i].completion_time > rr_max_ct) rr_max_ct = rr_proc[i].completion_time;
    }

    fcfs_avg_tat /= n; fcfs_avg_wt /= n; fcfs_avg_rt /= n;
    sjf_avg_tat /= n; sjf_avg_wt /= n; sjf_avg_rt /= n;
    rr_avg_tat /= n; rr_avg_wt /= n; rr_avg_rt /= n;

    printf("| Metric                    | FCFS  | SJF   | RR    |\n");
    printf("|---------------------------|-------|-------|-------|\n");
    printf("| Avg Turnaround Time       | %.2f  | %.2f  | %.2f  |\n", fcfs_avg_tat, sjf_avg_tat, rr_avg_tat);
    printf("| Avg Waiting Time          | %.2f  | %.2f  | %.2f  |\n", fcfs_avg_wt, sjf_avg_wt, rr_avg_wt);
    printf("| Avg Response Time         | %.2f  | %.2f  | %.2f  |\n", fcfs_avg_rt, sjf_avg_rt, rr_avg_rt);
    printf("| Throughput                | %.2f  | %.2f  | %.2f  |\n", (float)n/fcfs_max_ct, (float)n/sjf_max_ct, (float)n/rr_max_ct);
    printf("========================================\n");

    // Determine best algorithm
    printf("\nALGORITHM ANALYSIS:\n");
    printf("========================================\n");

    // Best for each metric
    printf("Best Average Turnaround Time: ");
    if(fcfs_avg_tat <= sjf_avg_tat && fcfs_avg_tat <= rr_avg_tat) printf("FCFS (%.2f)\n", fcfs_avg_tat);
    else if(sjf_avg_tat <= rr_avg_tat) printf("SJF (%.2f)\n", sjf_avg_tat);
    else printf("RR (%.2f)\n", rr_avg_tat);

    printf("Best Average Waiting Time: ");
    if(fcfs_avg_wt <= sjf_avg_wt && fcfs_avg_wt <= rr_avg_wt) printf("FCFS (%.2f)\n", fcfs_avg_wt);
    else if(sjf_avg_wt <= rr_avg_wt) printf("SJF (%.2f)\n", sjf_avg_wt);
    else printf("RR (%.2f)\n", rr_avg_wt);

    printf("Best Average Response Time: ");
    if(fcfs_avg_rt <= sjf_avg_rt && fcfs_avg_rt <= rr_avg_rt) printf("FCFS (%.2f)\n", fcfs_avg_rt);
    else if(sjf_avg_rt <= rr_avg_rt) printf("SJF (%.2f)\n", sjf_avg_rt);
    else printf("RR (%.2f)\n", rr_avg_rt);

    printf("Best Throughput: ");
    float fcfs_throughput = (float)n/fcfs_max_ct;
    float sjf_throughput = (float)n/sjf_max_ct;
    float rr_throughput = (float)n/rr_max_ct;

    if(fcfs_throughput >= sjf_throughput && fcfs_throughput >= rr_throughput) printf("FCFS (%.2f)\n", fcfs_throughput);
    else if(sjf_throughput >= rr_throughput) printf("SJF (%.2f)\n", sjf_throughput);
    else printf("RR (%.2f)\n", rr_throughput);

    printf("\nOVERALL RECOMMENDATION:\n");
    printf("- SJF: Generally best for minimizing average waiting time\n");
    printf("- RR: Best for interactive systems (good response time)\n");
    printf("- FCFS: Simple but may cause convoy effect\n");
    printf("- Choose based on system requirements and process characteristics\n");
}

void copy_processes(Process dest[], Process src[], int n) {
    for(int i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}

void reset_processes(Process processes[], int n) {
    for(int i = 0; i < n; i++) {
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].started = 0;
        processes[i].completion_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].waiting_time = 0;
        processes[i].response_time = 0;
    }
}
