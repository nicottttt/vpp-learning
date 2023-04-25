#include <vlib/vlib.h>
#include <vnet/vnet.h>
#include <vnet/pg/pg.h>
#include <vnet/ethernet/ethernet.h>
#include <vppinfra/error.h>
#include <stdio.h>
#include <udp_echo_msg/udp_echo_msg.h>
#include <arpa/inet.h> 


#define ENABLE_TRACE_UDP 1
#define ENABLE_PRINT 1


typedef enum 
{
  UDP_ECHO_MSG_NEXT_DROP,//0
  UDP_ECHO_MSG_N_NEXT,//1
} udp_echo_msg_next_t;

typedef struct 
{
	u32 src_ip;
	u32 dst_ip;
} udp_echo_msg_trace_t;

static u8 *
my_format_ip_address (u8 * s, va_list * args)
{
  u8 *a = va_arg (*args, u8 *);
  return format (s, "%d.%d.%d.%d",  a[0], a[1], a[2], a[3]);
}


static u8 * format_udp_echo_msg_trace (u8 * s, va_list * args)
{
	CLIB_UNUSED (vlib_main_t * vm) = va_arg (*args, vlib_main_t *);
  	CLIB_UNUSED (vlib_node_t * node) = va_arg (*args, vlib_node_t *);
	udp_echo_msg_trace_t * t = va_arg (*args, udp_echo_msg_trace_t *);

  	s = format (s, "src ip addr %U -> dst ip addr %U",
              my_format_ip_address, &t->src_ip,//void pt 
              my_format_ip_address, &t->dst_ip);//t->a
	return s;
}



static uword echo_repley_node_fn(vlib_main_t *vm, vlib_node_runtime_t *node, vlib_frame_t * frame){
            
    printf("\nStart to process the pkt: \n");

    u32 n_left_from, * from, * to_next;
    udp_echo_msg_next_t next_index;
    //u32 pkts_swapped = 0;
    

    from = vlib_frame_vector_args (frame);
    n_left_from = frame->n_vectors;
    next_index = node->cached_next_index;

    

    while(n_left_from > 0){

		u32 n_left_to_next;

		vlib_get_new_next_frame(vm, node, next_index, to_next, n_left_to_next);//if there is no frame prepared for handling, n_left_to_next will be 0

        while (n_left_from > 0 && n_left_to_next > 0)
        {
            u32 bi0;
            vlib_buffer_t * b0;
            
            u32 next0 = UDP_ECHO_MSG_NEXT_DROP;
            //u32 sw_if_index0;
            //u8 tmp0[6];
            
            void *data;
            ip4_header_t *ip4;
            ethernet_header_t *eth;
            udp_header_t *udp;
            //void *udp;


            /* speculatively enqueue b0 to the current next frame */
            bi0 = from[0];
            to_next[0] = bi0;
            from += 1;
            to_next += 1;
            n_left_from -= 1;
            n_left_to_next -= 1;

            b0 = vlib_get_buffer (vm, bi0);
            
            eth = ethernet_buffer_get_header (b0); 
            ip4 = (void *)(b0->data + vnet_buffer (b0)->l3_hdr_offset);
            //udp = (udp_header_t *)(b0->data + vnet_buffer (b0)->l4_hdr_offset);//??this function doesn't work

            u8 ip_header_length = (ip4->ip_version_and_header_length & 0x0F) * 4;
            udp = (udp_header_t *)((u8 *)ip4 + ip_header_length);

            data = vlib_buffer_get_current (b0);//data
          
#if ENABLE_PRINT
            printf("------pkt------\n");
            //print mac address here
            printf("Ethernet header: \n");
            printf("Mac src address: ");
            for(int i=0;i<6;i++){
                printf("%02x", eth->src_address[i]);
                if(i != 5){printf(":");}
            }
            printf("  Mac dst address: ");
            for(int i=0;i<6;i++){
                printf("%02x", eth->dst_address[i]);
                if(i != 5){printf(":");}
            }
            printf("\n");

            //print ip msg here
            printf("\nIp Header: \n");
            printf("Ip version: %02x\n", ip4->ip_version_and_header_length);
            printf("Ip src address: ");
            for(int i=0;i<4;i++){
              printf("%u", ip4->src_address.as_u8[i]);
              if(i != 3){printf(".");}
            }

            printf("  Ip dst address: ");
            for(int i=0;i<4;i++){
              printf("%u", ip4->dst_address.as_u8[i]);
              if(i != 3){printf(".");}
            }
            printf("\n");
            
            printf("\nUDP header: \n");
            printf("Udp->src port: %u", ntohs(udp->src_port));//!!ntoh
            printf("  Udp->dst port: %u\n", ntohs(udp->dst_port));
            printf("Udp->checksum: %04x", ntohs(udp->checksum));
            printf("  Udp->len: %u\n", ntohs(udp->length));

            //data
            
            //b0->current_length
            int len = ntohs(udp->length)-sizeof(udp_header_t);
            printf("\ndata: ");   
            for (int i=0; i<len; i++){
              printf("%c",*(u8 *)(data+i));
            }
            printf("\n");
            printf("------pkt------\n");
#endif

            

#if ENABLE_TRACE_UDP//start the trace function
          if (b0->flags & VLIB_BUFFER_IS_TRACED) { //**
					//trace code
					udp_echo_msg_trace_t *t = vlib_add_trace (vm, node, b0, sizeof (*t));
					t->src_ip = ip4->src_address.as_u32;

					t->dst_ip = ip4->dst_address.as_u32;
					//ntohl()
					//192.168.77.134 host hton()--> internet 
				}
#endif

            //swap the addr
            u32 tmp = ip4->dst_address.as_u32;
            ip4->dst_address.as_u32 = ip4->src_address.as_u32;
            ip4->src_address.as_u32 = tmp;
            ip4->checksum = ip4_header_checksum (ip4);

            u16 tmp1 = udp->src_port;
            udp->src_port = udp->dst_port;
            udp->dst_port = tmp1;
            udp->checksum = 0;


#if 0
            /* Ship it to ip4_lookup */
            vlib_frame_t *f;
            u32 bi = vlib_get_buffer_index (vm, b0); 
            f = vlib_get_frame_to_node (vm, ip4_lookup_node.index);
            to_next = vlib_frame_vector_args (f);
            to_next[0] = bi;
            f->n_vectors = 1;
            vlib_put_frame_to_node (vm, ip4_lookup_node.index, f);

#endif
            //set the pointer back !!!
            int offset = sizeof (ip4_header_t) + sizeof (udp_header_t);
            vlib_buffer_advance(b0, -offset);





            vlib_validate_buffer_enqueue_x1(vm, node, next_index, to_next, n_left_to_next, bi0, next0);//put it to the next buffer
        }
        
        vlib_put_next_frame(vm, node, next_index, n_left_to_next);
    }



  
    return 0;//drop the frame here
}



vlib_node_registration_t echo_repley_node;

VLIB_REGISTER_NODE (echo_repley_node) =
{
  .function = echo_repley_node_fn,
  .name = "udp_echo_msg_node",
  .vector_size = sizeof (u32),
  .format_trace = format_udp_echo_msg_trace,
  .type = VLIB_NODE_TYPE_INTERNAL,
  .n_next_nodes = UDP_ECHO_MSG_N_NEXT,
  .next_nodes = {
    [UDP_ECHO_MSG_NEXT_DROP] = "ip4-lookup",//drop the frame
  },
};
