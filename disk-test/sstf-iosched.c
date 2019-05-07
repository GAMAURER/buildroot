/*
 * SSTF IO Scheduler
 *
 * For Kernel 4.13.9
 */

#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>

/* SSTF data structure. */
struct sstf_data {
	struct list_head queue;
	struct list_head* enqueue;
	int count;
};

static void sstf_merged_requests(struct request_queue *q, struct request *rq,
				 struct request *next)
{
	list_del_init(&next->queuelist);
}

/* Esta função despacha o próximo bloco a ser lido. 

static int noop_dispatch(struct request_queue *q, int force)
{
	struct noop_data *nd = q->elevator->elevator_data;
	struct request *rq;

	rq = list_first_entry_or_null(&nd->queue, struct request, queuelist);
	if (rq) {
		list_del_init(&rq->queuelist);
		elv_dispatch_sort(q, rq);
		return 1;
	}
	return 0;
}

*/

static long diff(long x, long y)
{
	int a = x-y;
	if(a>0){return a;}
	return -a;
}
static int sstf_dispatch(struct request_queue *q, int force){
	struct sstf_data *nd = q->elevator->elevator_data;
	char direction = 'R';
	struct request *rq;

	if(!list_empty(&nd->queue)){
		struct request *next, *prev;
		next = list_entry(nd->queue.next, struct request, queuelist);
		prev = = list_entry(nextrq->queuelist.prev, struct request, queuelist);
		if(next==prev){
			rq=next;
		}		
		else{
			sector_t pos = nd->head_pos;
			if(diff(pos,next->__sector)<diff(pos,prev->__sector)){
				nd->direction = HEAD_FWD;
				rq=next;
			}
			else{
				nd->direction = HEAD_BCK;
				rq=prev;
			}
			
		}
		list_del_init(&rq->queuelist);
		elv_dispatch_sort(q, rq);
		printk(KERN_EMERG "[SSTF] dsp %c %lu\n", direction, blk_rq_pos(rq));

		return 1;
	}
	return 0;
}

		/* Aqui deve-se retirar uma requisição da fila e enviá-la para processamento.
	 * Use como exemplo o driver noop-iosched.c. Veja como a requisição é tratada.
	 *
	 * Antes de retornar da função, imprima o sector que foi atendido.
	 
	 

 	*/


static void sstf_add_request(struct request_queue *q, struct request *rq){
	struct sstf_data *nd = q->elevator->elevator_data;
	char direction = 'R';
	short a =0;
	if(list_empty(&nd->queue)){
		list_add(&rq->queuelist,&nd->queue);
		nd->enqueue=nd->queue.next;
		nd->count+=1;
	}
	else{
		
		struct list_head* head;
		sector_t new = blk_rq_pos(rq);
		list_for_each(head, &nd->queue) { 	
			
			if (nd->count == 1){
				a=1;
				list_add(&rq->queuelist, head);
				nd->count++;			
				break;
			}	

			struct request* next_req = list_entry(head->next, struct request, queuelist);
			sector_t next = blk_rq_pos(next_req);
		
			if (next >= new ){
				a=1;
				list_add(&rq->queuelist, position);
				nd->count++;			
				break;
			}	
		}
	}
	if(a==0){
		list_add_tail(&rq->queuelist, &nd->queue);

	}
	printk("Terminamos de adicionar um item na queue e agora temos %d requisiões a serem atendidas\n", nd->count);
	printk(KERN_EMERG "[SSTF] add %c %lu\n", direction, blk_rq_pos(rq));
}

static int sstf_init_queue(struct request_queue *q, struct elevator_type *e){
	struct sstf_data *nd;
	struct elevator_queue *eq;

	/* Implementação da inicialização da fila (queue).
	 *
	 * Use como exemplo a inicialização da fila no driver noop-iosched.c
	 *
	 */

	eq = elevator_alloc(q, e);
	if (!eq)
		return -ENOMEM;

	nd = kmalloc_node(sizeof(*nd), GFP_KERNEL, q->node);
	if (!nd) {
		kobject_put(&eq->kobj);
		return -ENOMEM;
	}
	eq->elevator_data = nd;

	INIT_LIST_HEAD(&nd->queue);
	nd->count =0;
	spin_lock_irq(q->queue_lock);
	q->elevator = eq;
	spin_unlock_irq(q->queue_lock);

	return 0;
}

static void sstf_exit_queue(struct elevator_queue *e)
{
	struct sstf_data *nd = e->elevator_data;

	/* Implementação da finalização da fila (queue).
	 *
	 * Use como exemplo o driver noop-iosched.c
	 *
	 */
	BUG_ON(!list_empty(&nd->queue));
	kfree(nd);
}

/* Infrastrutura dos drivers de IO Scheduling. */
static struct elevator_type elevator_sstf = {
	.ops.sq = {
		.elevator_merge_req_fn		= sstf_merged_requests,
		.elevator_dispatch_fn		= sstf_dispatch,
		.elevator_add_req_fn		= sstf_add_request,
		.elevator_init_fn		= sstf_init_queue,
		.elevator_exit_fn		= sstf_exit_queue,
	},
	.elevator_name = "sstf",
	.elevator_owner = THIS_MODULE,
};

/* Inicialização do driver. */
static int __init sstf_init(void)
{
	return elv_register(&elevator_sstf);
}

/* Finalização do driver. */
static void __exit sstf_exit(void)
{
	elv_unregister(&elevator_sstf);
}

module_init(sstf_init);
module_exit(sstf_exit);

MODULE_AUTHOR("Sergio Johann Filho, Guilherme Maurer");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("SSTF IO scheduler");
//ref https://github.com/rettigs/cs444-group4/blob/master/project2/sstf-iosched.c