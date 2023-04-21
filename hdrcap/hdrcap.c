#include <vnet/plugin/plugin.h>
#include <hdrcap/hdrcap.h>


hdrcap_main_t hdrcap_main;




int hdrcap_enable_disable (hdrcap_main_t * hmp, u32 sw_if_index,
                                   int enable_disable){
	hdrcap_main_t *pt = &hdrcap_main;
	
	if (pool_is_free_index (pt->vnet_main->interface_main.sw_interfaces,
							  sw_if_index))
		return VNET_API_ERROR_INVALID_SW_IF_INDEX;

	//the api link to the node??
	vnet_feature_enable_disable("ip4-local", "hdrcap", sw_if_index, enable_disable, 0, 0);
	return 0;
}



//check the format of the command line
static clib_error_t* hdrcap_enable_disable_command_fn(vlib_main_t* vm,
                                    unformat_input_t *input,
                                    vlib_cli_command_t *cmd)
{
//enable or disable of the plugin in the command line
	hdrcap_main_t *pt = &hdrcap_main;

	u32 sw_if_index = ~0;
	int enable_disable = -1;

	while(unformat_check_input(input) != UNFORMAT_END_OF_INPUT) {
			if (unformat(input, "disable"))
					enable_disable = 0;
			else if (unformat(input, "enable"))
        			enable_disable = 1; 
			else if (unformat(input, "%U",
					unformat_vnet_sw_interface,
					pt->vnet_main, &sw_if_index));
			else
					break;
	}

	if (enable_disable == -1)
			return clib_error_return(0, "Please specify if it is enable or disable...");

	if (sw_if_index == ~0)
			return clib_error_return(0, "Please specify an interface...");

	hdrcap_enable_disable(pt, sw_if_index, enable_disable);

	return 0;

}

// /* API message handler */
// static void vl_api_hdrcap_enable_disable_t_handler
// (vl_api_hdrcap_enable_disable_t * mp)
// {
//   vl_api_hdrcap_enable_disable_reply_t * rmp;
//   nico_example_main_t * nmp = &nico_example_main;
//   int rv;

//   rv = nico_example_enable_disable (nmp, ntohl(mp->sw_if_index),
//                                       (int) (mp->enable_disable));

//   REPLY_MACRO(VL_API_NICO_EXAMPLE_ENABLE_DISABLE_REPLY);
// }

VLIB_CLI_COMMAND (hdrcap_command, static) = {
    .path = "hdrcap",
    .short_help = 
    "hdrcap <interface-name> <enable disable>",
    //"hdrcap <interface-name> <enable disable>",
    //hdrcap inter enab a b c
    .function = hdrcap_enable_disable_command_fn,
};





VLIB_PLUGIN_REGISTER()={
	.version = HDRCAP_PLUGIN_VERSION,
	.description = "header capture example",

};


//how it initialize
static clib_error_t *hdrcap_init(vlib_main_t *vm){

	hdrcap_main_t *pt = &hdrcap_main;

	pt->vlib_main = vm;
	pt->vnet_main = vnet_get_main();

	return NULL;
	
}


VLIB_INIT_FUNCTION(hdrcap_init);

VNET_FEATURE_INIT(hdrcap, static) = {

	.arc_name = "ip4-local",//???????
	.node_name = "hdrcap",//here it will recognize hdrcap.c and hdrcap_node.c 
	.runs_after = VNET_FEATURES("ip4-icmp-input")
	//.runs_after = VNET_FEATURES("ip4-icmp-echo-request")

};










