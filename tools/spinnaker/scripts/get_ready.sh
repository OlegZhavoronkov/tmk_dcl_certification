#!/bin/sh

#configure USB subsystem
sudo sh -c 'echo 1000 > /sys/module/usbcore/parameters/usbfs_memory_mb'

#configure ethernet adapter for Blackfly S GigE camera
ADAPTER=enp59s0

sudo ifconfig ${ADAPTER} 169.255.0.1
sudo ifconfig ${ADAPTER}  netmask 255.255.0.0
sudo ifconfig ${ADAPTER}  mtu 9000

sudo sysctl -w net.ipv4.conf.all.rp_filter=0
sudo sysctl -w net.ipv4.conf.${ADAPTER}.rp_filter=0

sudo sysctl -w net.core.rmem_max=10485760
sudo sysctl -w net.core.rmem_default=10485760


