#!/bin/bash

case $1 in
    insert)
        echo "Testing insert"
        make
        sudo insmod Hello.ko
        dmesg
        ;;
    remove)
        echo "Testing remove"
        sudo rmmod Hello
        make clean
        dmesg
        ;;
    test)
        echo "Testing search"
        make
        sudo insmod Hello.ko
        sudo cat /dev/helloworld
        sudo rmmod Hello
        make clean
        dmesg 
        ;;
    clear)
        echo "Clearing dmesg"
        sudo dmesg -c
        ;;
esac

