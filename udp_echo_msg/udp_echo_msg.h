#include <vnet/vnet.h>
#include <vnet/ip/ip.h>

#include <vppinfra/hash.h>
#include <vppinfra/error.h>
#include <vppinfra/elog.h>



#ifndef __HDRCAP_H__
#define __HDRCAP_H__
//to prevent redefine the header multiple times

extern vlib_node_registration_t echo_repley_node;

typedef struct{

	u16 msg_id_base;
	vlib_main_t * vlib_main;
	vnet_main_t *vnet_main;
	int udp_ports_registered;


} udp_echo_msg_main_t;



#define UDP_ECHO_MSG_PLUGIN_VERSION "1.0"



#endif





