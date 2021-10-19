#!/bin/bash +x
swap="/home/tmk/swapfile"
fallocate -l 64G $swap
chmod 600 $swap
mkswap --label swap_file $swap
swapon $swap
swapon --show