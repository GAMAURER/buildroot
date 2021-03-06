
#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/unistd.h>

/* SSTF data structure. */
long lastsec = 0;
struct sstf_data {
	struct list_head queue;
};

static void sstf_merged_requests(struct request_queue *q, struct request *rq,
				 struct request *next)
{
	list_del_init(&next->queuelist);
}
static long diff(long x, long y)
{
	int a = x-y;
	if(a>0){return a;}
	return -a;
}
/* Esta função despacha o próximo bloco a ser lido. */
static int sstf_dispatch(struct request_queue *q, int force){
	struct sstf_data *nd = q->elevator->elevator_data;
	char direction = 'R';
	struct request *rq;
    int len = 0;
    
    
	/* Aqui deve-se retirar uma requisição da fila e enviá-la para processamento.
	 * Use como exemplo o driver noop-iosched.c. Veja como a requisição é tratada.
	 *
	 * Antes de retornar da função, imprima o sector que foi atendido.
	 

    //closestrequest tem o request mais proxuimo
    if(lastsec==0){
		rq = list_first_entry_or_null(&nd->queue, struct request, queuelist);
        printk(KERN_EMERG "lastsec==0\n");
	}
	else{*/
	if(!list_empty(&nd->queue))
        printk(KERN_EMERG "");//else\n
		long closestdiff = LONG_MAX;
		struct request* closestrequest;
		struct list_head *h;
		
		
		list_for_each(h,&nd->queue){
            printk(KERN_EMERG "");
            //len++;
			 
			
			struct request* curr_req = list_entry(h, struct request, queuelist);
			long hpos=blk_rq_pos(curr_req);
			if(diff(hpos,lastsec)<closestdiff){
                 printk(KERN_EMERG "");
				closestdiff=diff(hpos,lastsec);
				closestrequest=curr_req;        
			}
		}
		    
		rq=closestrequest;
		
		
	//}
	if (rq) {
		printk(KERN_EMERG "[SSTF] dsp %c %lu \n", direction, blk_rq_pos(rq));
        lastsec = blk_rq_pos(rq);

		list_del_init(&rq->queuelist);
		elv_dispatch_sort(q, rq);
		
        len = 0;
		return 1;
	}

    len = 0;
    
	return 0;
}

static void sstf_add_request(struct request_queue *q, struct request *rq){
	struct sstf_data *nd = q->elevator->elevator_data;
	char direction = 'R';

	/* Aqui deve-se adicionar uma requisição na fila do driver.
	 * Use como exemplo o driver noop-iosched.c
	 *
	 * Antes de retornar da função, imprima o sector que foi adicionado na lista.
	 */

	list_add_tail(&rq->queuelist, &nd->queue);
	printk(KERN_EMERG "",  blk_rq_pos(rq));//[SSTF] add  %lu\n
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

