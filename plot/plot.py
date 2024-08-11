import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
from matplotlib.patches import Patch
import sys

# Fonction pour lire le fichier de logs et extraire les informations
def read_log_file(filename):
    with open(filename, 'r') as file:
        lines = file.readlines()

    max_tasks = int(lines[0].split(":")[1].strip()) + 1
    logs = []
    
    for line in lines[1:]:
        parts = line.strip().split()
        if len(parts) == 3:
            action, task_id, timestamp = parts
            task_id = int(task_id)
        elif len(parts) == 2:
            action, timestamp = parts[0], parts[1]
            task_id = max_tasks if action == "IDLE" else None
        timestamp = int(timestamp)
        logs.append((action, task_id, timestamp))
    
    return max_tasks, logs

# Fonction pour traiter les logs et organiser les informations pour le graphe
def process_logs(max_tasks, logs):
    task_dict = {i: [] for i in range(max_tasks + 2)}  # Including the IDLE task as max_tasks
    current_task = -1
    start_time = 0
    missed_deadlines = []

    for log in logs:
        action, task_id, timestamp = log
        if task_id == max_tasks or task_id == -1:
            task_id = max_tasks
        if action == "D":
            if current_task != -1:
                task_dict[current_task].append((start_time, timestamp, 'running'))
            current_task = task_id
            start_time = timestamp
        elif action == "P":
            task_dict[task_id].append((start_time, timestamp, 'running'))
            start_time = timestamp
            current_task = -1
        elif action == "F":
            task_dict[task_id].append((start_time, timestamp, 'running'))
            current_task = -1
        elif action == "IDLE":
            if current_task != -1:
                task_dict[current_task].append((start_time, timestamp, 'running'))
            current_task = max_tasks
            start_time = timestamp
        elif action == "R":
            task_dict[task_id].append((timestamp, timestamp, 'appearance'))
        elif action == "M":
            missed_deadlines.append((task_id, timestamp))
    
    return task_dict, missed_deadlines


# Fonction pour créer le graphe
def create_gantt_chart(task_dict, missed_deadlines, name):
    fig, ax = plt.subplots(figsize=(15, 8))
    colors = ['#ffb3ba', '#baffc9', '#bae1ff', '#ffdfba', '#ffffba']
    legend_elements = []
    y_labels = []
    y_ticks = []
    
    for i, (task_id, intervals) in enumerate(task_dict.items()):
        if task_id > max_tasks:
            continue
        if task_id == max_tasks - 1:
            y_labels.append('IDLE')
            legend_elements.append(Patch(facecolor=colors[i], edgecolor='k', label=f'IDLE'))
        elif task_id == max_tasks:
            y_labels.append('Context Switching')
            legend_elements.append(Patch(facecolor=colors[i], edgecolor='k', label=f'CS'))
        else:
            y_labels.append(f'Tâche {task_id}')
            legend_elements.append(Patch(facecolor=colors[i], edgecolor='k', label=f'Tâche {i}'))
        y_ticks.append(i)
        for interval in intervals:
            start, end, state = interval
            if state == 'running':
                ax.barh(i, end-start, left=start, color=colors[task_id])
            elif state == 'appearance':
                # ax.plot(start, i, 'o', color='black')
                ax.annotate('', xy=(start, i+0.1), xytext=(start, i+0.5),
                            arrowprops=dict(facecolor='black', shrink=0.05, width=0.3, headwidth=5, headlength=5))
            elif state == 'idle':
                ax.barh(i, 0, left=start, color='white', edgecolor='k')

    for task_id, timestamp in missed_deadlines:
        ax.scatter(timestamp, task_id, color='red', marker='x', s=100, label='Missed Deadline' if 'Missed Deadline' not in [l.get_label() for l in legend_elements] else "")

    ax.set_yticks(y_ticks)
    ax.set_yticklabels(y_labels)
    ax.set_xlabel('Temps (ms)')
    ax.set_title('Exécution des tâches')
    ax.legend(handles=legend_elements, loc='upper right')

    # plt.show()
    plt.savefig(name, dpi=150)

# Lecture et traitement du fichier de logs

name = "plot.png"
file_log = "output.log"
if len(sys.argv) >= 2:
    file_log = sys.argv[1]
if len(sys.argv) >= 3:
    name = f"plot{sys.argv[2]}.png"
print(file_log, name)

max_tasks, logs = read_log_file(file_log)
task_dict, missed_deadlines = process_logs(max_tasks, logs)

# Création du graphe
create_gantt_chart(task_dict, missed_deadlines, name)
