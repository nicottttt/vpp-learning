#include <vnet/plugin/plugin.h>
#include <udp_echo_msg/udp_echo_msg.h>
#include <vnet/udp/udp.h>
#include <vnet/vnet.h>
#include <vnet/udp/udp_local.h>
#include <vnet/ip/ip_sas.h>
#include <vlibapi/api.h>
#include <vlibmemory/api.h>
#include <vpp/app/version.h>
#include <stdbool.h>

#include <udp_echo_msg/udp_echo_msg.api_enum.h>
#include <udp_echo_msg/udp_echo_msg.api_types.h>

#define REPLY_MSG_ID_BASE pt->msg_id_base
#include <vlibapi/api_helper_macros.h>
#include <vhost/vhost_user.h>



udp_echo_msg_main_t udp_echo_msg_main;


int udp_echo_msg_enable_disable (vlib_main_t * vm, udp_echo_msg_main_t * hmp,
                                   int enable_disable){
	udp_echo_msg_main_t *pt = &udp_echo_msg_main;
	
	if(enable_disable == 1){
		if( pt->udp_ports_registered != 1){
			udp_register_dst_port (vm, 8889, echo_repley_node.index, 1 /* is_ip4 */ );
			pt->udp_ports_registered = 1;
		}
	}
	else if(enable_disable == 0){
		if( pt->udp_ports_registered == 1){
			udp_unregister_dst_port (vm, 8889, 1 /* is_ip4 */ );
			pt->udp_ports_registered = 0;
		}
	}
	
	return 0;
}

//check the format of the command line
static clib_error_t* udp_echo_msg_enable_disable_command_fn(vlib_main_t* vm,
                                    unformat_input_t *input,
                                    vlib_cli_command_t *cmd)
{
//enable or disable of the plugin in the command line
	udp_echo_msg_main_t *pt = &udp_echo_msg_main;

	int enable_disable = -1;

	while(unformat_check_input(input) != UNFORMAT_END_OF_INPUT) {
			if (unformat(input, "disable"))
					enable_disable = 0;
			else if (unformat(input, "enable"))
        			enable_disable = 1; 
			else
					break;
	}

	if (enable_disable == -1)
			return clib_error_return(0, "Please specify if it is enable or disable...");


	udp_echo_msg_enable_disable(vm, pt, enable_disable);

	return 0;

}



static void vl_api_udp_echo_msg_enable_disable_t_handler (vl_api_udp_echo_msg_enable_disable_t * mp){

  vl_api_udp_echo_msg_enable_disable_reply_t * rmp;
  udp_echo_msg_main_t *pt = &udp_echo_msg_main;

  int rv;
  rv = udp_echo_msg_enable_disable (pt->vlib_main, pt, (int) (mp->enable_disable));

  REPLY_MACRO(VL_API_UDP_ECHO_MSG_ENABLE_DISABLE_REPLY);


}

#include <udp_echo_msg/udp_echo_msg.api.c> //


VLIB_CLI_COMMAND (udp_echo_msg_command, static) = {
    .path = "udp_echo_msg",
    .short_help = 
    "udp_echo_msg <interface-name> <enable disable>",
    .function = udp_echo_msg_enable_disable_command_fn,
};




VLIB_PLUGIN_REGISTER()={
	.version = UDP_ECHO_MSG_PLUGIN_VERSION,
	.description = "This plugin is to send back the msg to where it comes from",
};


//initialize procedure
static clib_error_t *udp_echo_msg_init(vlib_main_t *vm){

	udp_echo_msg_main_t *pt = &udp_echo_msg_main;

	pt->vlib_main = vm;
	pt->vnet_main = vnet_get_main();

	/* Add our API messages to the global name_crc hash table */
  	pt->msg_id_base = setup_message_id_table ();

	return NULL;
	
}


VLIB_INIT_FUNCTION(udp_echo_msg_init);










