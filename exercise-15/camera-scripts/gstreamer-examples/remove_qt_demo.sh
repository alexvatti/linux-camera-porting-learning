#!/bin/sh

echo "remove QT-Demo of auto start"
echo "============================"
systemctl disable phytec-qtdemo
reboot
