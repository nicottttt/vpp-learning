# vpp-learning
preparation work before my internship in Cisco Meraki

## Load customized plugins into vpp plugins
copy the hdrcap and udp_echo_msg folder into vpp plugins folder:
```
cp /hdrcap /path-to-vpp-root/src/plugins/
```

compile the customized plugin(take hdrcap as example):
```
cd /path-to-vpp-root/src/plugins/hdrcap
cmake ..
```

return back to the root path of vpp and rebuild all the plugin:
```
cd /path-to-vpp-roo/
make wipe
make build-release
```

start vpp with startup.conf and init.vpp:
```
sudo -E vpp -c startup.conf
```

link the real path to environment path:
```
ln -s /root/vpp_test/vpp/build-root/install-vpp-native/vpp/bin/vpp /usr/bin/vpp
```
