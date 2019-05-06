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
	return x >= y ? x - y : y - x;
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
	 
	 
	struct request *shortest = list_first_entry_or_null(&nd->queue, struct request, queuelist);

	if (prev_sector > -1 && shortest) {

		long shortest_aux = (long)blk_rq_pos(shortest);
		long shortest_distance = diff(prev_sector,shortest_aux);
		struct request *cursor = NULL;
		list_for_each_entry(cursor,&nd->queue,queuelist){
			long cursor_aux = (long)blk_rq_pos(cursor);
			long cursor_distance = diff(prev_sector,cursor_aux);
			if (cursor_distance < shortest_distance) {
				shortest_distance = cursor_distance;
				shortest = cursor;
			}
		}
	}
	rq =shortest
 	*/


static void sstf_add_request(struct request_queue *q, struct request *rq){
	struct sstf_data *nd = q->elevator->elevator_data;
	char direction = 'R';

	/* Aqui deve-se adicionar uma requisição na fila do driver.
	 * Use como exemplo o driver noop-iosched.c
	 *
	 * Antes de retornar da função, imprima o sector que foi adicionado na lista.
	 */

	list_add_tail(&rq->queuelist, &nd->queue);
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

MODULE_AUTHOR("Sergio Johann Filho");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("SSTF IO scheduler");
