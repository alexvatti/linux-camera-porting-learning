#!/bin/sh

echo ""
echo "Turn Off/On Wayland (Weston)"
echo "============================"

echo " select Weston state"
echo " ==================="
echo " 1) disable Weston (for this session)"
echo " 2) disable Weston (remove from autostart)"
echo " 3) activate Weston (for this session)"
echo " 4) activate Weston (activate in autostart)"
read WINDOWS_MANAGER
echo " Your select = $WINDOWS_MANAGER"
case $WINDOWS_MANAGER in
  "1") systemctl stop weston@root;;
  "2") systemctl disable weston@root;;
  "3") systemctl start weston@root;;
  "4") systemctl enable weston@root;;
esac
