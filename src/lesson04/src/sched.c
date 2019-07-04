#include "sched.h"
#include "irq.h"
#include "printf.h"

static struct task_struct init_task = INIT_TASK;
struct task_struct *current = &(init_task);
struct task_struct * task[NR_TASKS] = {&(init_task), };
int nr_tasks = 1;

void preempt_disable(void)
{
	current->preempt_count++;
}

void preempt_enable(void)
{
	current->preempt_count--;
}


void _schedule(void)
{
	preempt_disable();
	int actual, limiteprioridad;
	int next,c;


	struct task_struct * p;
 //asignacion de prioridades RR
	for (int j =0; j < NR_TASKS; j++){
  task_struct[j].prioridadRR = j;
		limiteprioridad = j;
	}
	//asignacion de prioridades RR
	while (1) {
		c = -1;
		next = 0;
		for (int i = 0; i < NR_TASKS; i++){ //ciclo original
			p = task[i];
			actual = i;
			//ecitar bugs supongo
			if (actual > limiteprioridad){
				actual = 0;
				i = 0; //reset ciclo original
			}
			//evitar bugs supongo
			if(task_struct[i].prioridadRR == actual){
				//if para RR
			if (p && p->state == TASK_RUNNING && p->counter > c) {
				c = p->counter;
				//reingresar a la lista de prioridadesRR y cambiar a otro cuando termine el counter
				task_struct[i].prioridadRR = limiteprioridad + 1;
				limiteprioridad++;
				//reset de prioridades en caso de numero muy algo
				if (limiteprioridad > 3000){ //numero arbitrario para resetear prioridades
					for (int k =0; k < NR_TASKS; k++){
				  task_struct[j].prioridadRR = k;
						limiteprioridad = k;
						i = 0; //reset ciclo original
					}
					//reset de prioridades en caso de numero muy algo
				}
				switch_to(task[next]);
				//reingresar a la lista de prioridadesRR y cambiar a otro cuando termine el counter
				next = i;

			}
		}//} para RR
		else //para RR
		 switch_to(task[next]); //si no culple la prioridad, cambiar de estado


		}

		if (c) {
			break;
		}
		for (int i = 0; i < NR_TASKS; i++) {
			p = task[i];
			if (p) {
				p->counter = (p->counter >> 1) + p->priority;
			}
		}
	}

	preempt_enable();


}

void schedule(void)
{
	current->counter = 0;
	_schedule();
}

void switch_to(struct task_struct * next)
{
	if (current == next)
		return;
	struct task_struct * prev = current;
	current = next;
	cpu_switch_to(prev, next);
}

void schedule_tail(void) {
	preempt_enable();
}


void timer_tick()
{
	--current->counter;
	if (current->counter>0 || current->preempt_count >0) {
		return;
	}
	current->counter=0;
	enable_irq();
	_schedule();
	disable_irq();
}
