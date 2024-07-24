import matplotlib.pyplot as plt
import re

def parse_log_file(file_path):
    events = []
    with open(file_path, 'r') as file:
        for line in file:
            if match := re.match(r"(D_CS|F_CS|R_T \d+|D_T \d+|F_T \d+|IDLE) : (\d+)ms", line):
                event, time = match.groups()
                time = int(time)
                events.append((event, time))
    return events

def create_schedule_plot(events):
    fig, ax = plt.subplots()
    y_labels = set()
    for event, time in events:
        if "T" in event:
            task = event.split(" ")[1]
            y_labels.add(task)
    
    y_labels = sorted(y_labels)
    y_ticks = {label: idx for idx, label in enumerate(y_labels, start=1)}
    y_ticks["IDLE"] = 0
    y_ticks["CS"] = -1

    current_task = None
    task_start_time = None

    for i, (event, time) in enumerate(events):
        if i > 0:
            prev_event, prev_time = events[i-1]
            if prev_event.startswith("D_T"):
                task = prev_event.split(" ")[1]
                ax.broken_barh([(prev_time, time - prev_time)], (y_ticks[task], 1), facecolors='tab:blue')
            elif prev_event.startswith("IDLE"):
                ax.broken_barh([(prev_time, time - prev_time)], (y_ticks["IDLE"], 1), facecolors='tab:gray')
            elif prev_event == "D_CS":
                ax.broken_barh([(prev_time, time - prev_time)], (y_ticks["CS"], 1), facecolors='tab:red')
                if current_task is not None:
                    ax.broken_barh([(task_start_time, prev_time - task_start_time)], (y_ticks[current_task], 1), facecolors='tab:blue')

        if event.startswith("D_T"):
            current_task = event.split(" ")[1]
            task_start_time = time
        elif event == "IDLE":
            current_task = "IDLE"
            task_start_time = time
        elif event.startswith("R_T") or event == "F_CS":
            current_task = None
            task_start_time = None

    for event, time in events:
        if event.startswith("R_T"):
            task = event.split(" ")[1]
            ax.plot(time, y_ticks[task], 'ro', label=f"R_T {task}" if not plt.gca().get_legend_handles_labels()[1].count(f"R_T {task}") else "")

    ax.set_yticks(list(y_ticks.values()))
    ax.set_yticklabels(list(y_ticks.keys()))
    ax.set_xlabel('Time (ms)')
    ax.set_ylabel('Tasks')
    ax.grid(True)
    ax.legend(loc='upper right')
    plt.show()

file_path = 'output.log'  # Remplacez ceci par le chemin de votre fichier de sortie
events = parse_log_file(file_path)
create_schedule_plot(events)
