#!/usr/bin/env python
 
from vpp_papi import VPPApiClient
import os                                                                                                                                                                 
import sys
import fnmatch
import time                                                                                                                                                               
def papi_event_handler(msgname, result):
  print(msgname)                                                                                                                                                          
  print(result)

vpp_json_dir = "/home/nico/Desktop/vpp/json"
# construct a list of all the json api files
jsonfiles = []                                                                                                                                                            
for root, dirnames, filenames in os.walk(vpp_json_dir):
    for filename in fnmatch.filter(filenames, '*.api.json'):
        jsonfiles.append(os.path.join(vpp_json_dir, filename))                                                                                                            
if not jsonfiles:
    print('Error: no json api files found')
    exit(-1)                                                                                                                                                              
# use all those files to create vpp.
vpp = VPPApiClient(apifiles = jsonfiles)
r = vpp.connect("test_papi")                                                                                                                                              
print(r)

print("Successfully trigger the udp_echo_msg plugin")

#vpp.api.hdrcap_enable_disable(client_index = 1, context = 1, enable_disable = 1, sw_if_index = 1)
vpp.api.udp_echo_msg_enable_disable(client_index = 1, context = 1, enable_disable = 1, sw_if_index = 1)
