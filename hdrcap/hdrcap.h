#include <vnet/vnet.h>
#include <vnet/ip/ip.h>

#include <vppinfra/hash.h>
#include <vppinfra/error.h>
#include <vppinfra/elog.h>



#ifndef __HDRCAP_H__
#define __HDRCAP_H__

typedef struct{

	u16 msg_id_base;
	vlib_main_t * vlib_main;
	vnet_main_t *vnet_main;


} hdrcap_main_t;


//hdrcap_main_t hdrcap_main;

#define HDRCAP_PLUGIN_VERSION "1.0"



#endif




