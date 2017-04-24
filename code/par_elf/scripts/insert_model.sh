#!/bin/sh
cd ../build/kernel && insmod *.ko && cd - && echo "insmod OK!"

