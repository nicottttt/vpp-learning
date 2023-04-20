#include <vlib/vlib.h>
#include <vnet/vnet.h>
#include <vnet/pg/pg.h>
#include <vnet/ethernet/ethernet.h>
#include <vppinfra/error.h>
#include <hdrcap/hdrcap.h>
#include <stdio.h>



typedef enum 
{
  hdrcap_NEXT_INTERFACE_OUTPUT,//0
  //hdrcap_NEXT_INTERFACE_udp,//if define a node that never use, it will cause an erro!!
  hdrcap_N_NEXT,//1
} hdrcap_next_t;



extern vlib_node_registration_t hdrcap_node;

typedef struct 
{
//   u32 next_index;
//   u32 sw_if_index;
//   u8 new_src_mac[6];
//   u8 new_dst_mac[6];
	u32 src_ip;
	u32 dst_ip;

	//u8 a[4];
} hdrcap_trace_t;

static u8 *
my_format_ip_address (u8 * s, va_list * args)
{
  u8 *a = va_arg (*args, u8 *);
  return format (s, "%d.%d.%d.%d",  a[0], a[1], a[2], a[3]);
}


static u8 * format_hdrcap_trace (u8 * s, va_list * args)
{
	CLIB_UNUSED (vlib_main_t * vm) = va_arg (*args, vlib_main_t *);
  	CLIB_UNUSED (vlib_node_t * node) = va_arg (*args, vlib_node_t *);
	hdrcap_trace_t * t = va_arg (*args, hdrcap_trace_t *);

  	s = format (s, "src ip addr %U -> dst ip addr %U",
              my_format_ip_address, &t->src_ip,//void pt 
              my_format_ip_address, &t->dst_ip);//t->a
	return s;
}

static uword hdrcap_node_fn(vlib_main_t *vm, vlib_node_runtime_t *node,
        vlib_frame_t * frame){
        
        printf("start to run the functionality\n");
//here to write the functionality

	u32 n_left_from, * from, * to_next;
  	hdrcap_next_t next_index;
	//int ethhdr = 14;
	//int iphdr = 20;

	from = vlib_frame_vector_args(frame);
	n_left_from = frame->n_vectors; //the number of the element in frame
	next_index = node->cached_next_index;  //next frame index

	

	while(n_left_from > 0){

		u32 n_left_to_next;

		vlib_get_new_next_frame(vm, node, next_index, to_next, n_left_to_next);//if there is no frame prepared for handling, n_left_to_next will be 0

		while (n_left_from > 0 && n_left_to_next > 0) {

			vlib_buffer_t *b0;
			u32 bi0;//buffer index
			u32 next0 = 0;
			void *en0;
			ip4_header_t *eth0;
			int flag = 0;

			bi0 = from[0];//*from
	  		to_next[0] = bi0;

			//??
			from += 1;
			to_next += 1;
			n_left_from -= 1;//how many left
			n_left_to_next -= 1;
			//follow the example file:
			/*
				bi0 = from[0];
				to_next[0] = bi0;
				from += 1;
				to_next += 1;
				n_left_from -= 1;
				n_left_to_next -= 1;
			*/

			b0 = vlib_get_buffer (vm, bi0);//get the index here, the return value is a vlib_buffer_t type
			//b0 here is a pointer or index of this buffer
			//buffer的指针:表示要获取数据指针的缓冲区
			
			en0 = vlib_buffer_get_current (b0);//get the data here,current_data in it is an offset
			//en0 here is the pointer of data, different from the b0. 数据的指针
			eth0 = vlib_buffer_get_current (b0);
			//type the ethernet hdr and ip hdr of the packet here


			//*(u8 *)(en0) is already a value, unsigned value(c0...)
			
			if(*(u8 *)(en0+23)==0x01){
				printf("icmp, ");
				printf("%x\n",eth0->protocol);
				flag = 1;
			}
			else if(*(u8 *)(en0+23)==0x11){
				printf("UDP msg, ");
				printf("%x\n",eth0->protocol);
				flag = 1;
			
			}
			
			if(flag == 1){
				printf("Src addr:");
				for(int i=0;i<4;i++){
					u8 val = *(u8 *)(en0+26+i);// 0x0c *(en0+26+i)
				    printf("%u", val);  //let unsigned int value trun into decimal value
				    if(i != 3){printf(".");}
				}

				printf("  Dst addr:");
				for(int i=0;i<4;i++){
					u8 val = *(u8 *)(en0+30+i);
				    printf("%u", val);  //let unsigned int value trun into decimal value
				    if(i != 3){printf(".");}
				}

				//trace code
				hdrcap_trace_t *t = vlib_add_trace (vm, node, b0, sizeof (*t));
				t->src_ip = *(u32 *)(en0 + 26);

//				t->a[0]=*(u8 *)(en0+26);t->a[1]=*(u8 *)(en0+27)



				t->dst_ip = *(u32 *)(en0 + 30);
				//ntohl()
				//192.168.77.134 host hton()--> internet 
				flag = 0;
			}

			
			
			printf("\n");

	
			




			vlib_validate_buffer_enqueue_x1(vm, node, next_index, to_next, n_left_to_next, bi0, next0);//put it to the next buffer

			
		}

		vlib_put_next_frame(vm, node, next_index, n_left_to_next);
	}
	
	return 0;//0

}



VLIB_REGISTER_NODE (hdrcap_node) = {
        .name		= "hdrcap",
        .function       = hdrcap_node_fn,
        .vector_size    = sizeof(u32),
        .format_trace   = format_hdrcap_trace,
        .type           =  VLIB_NODE_TYPE_INTERNAL,//which means that it is a middle node
        //.n_errors       = ARRAY_LEN(hdrcap_error_strings),
        //.error_strings  = hdrcap_error_strings,
        .n_next_nodes   = hdrcap_N_NEXT,
        .next_nodes     = {
                [hdrcap_NEXT_INTERFACE_OUTPUT] = "ethernet-input",
					//
					//
        },
};

