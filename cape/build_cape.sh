#!/bin/sh
rm *.dtbo
dtc -O dtb -o BB-PACKTPUB-00A0.dtbo -b 0 -@ BB-PACKTPUB-00A0.dts
