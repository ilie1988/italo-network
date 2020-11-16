#!/bin/sh

touch /var/lib/italonet/suspend-launchd-service # Prevent restarting on exit
killall italonet
sleep 5  # Give it some time to shut down before we bring launchd into this
launchctl stop network.italo.italonet.daemon
launchctl unload /Library/LaunchDaemons/network.italo.italonet.daemon.plist

rm -rf /Library/LaunchDaemons/network.italo.italonet.daemon.plist
rm -rf /Applications/ItalonetGUI.app
rm -rf /var/lib/italonet
rm -rf /usr/local/italonet/
rm -rf /opt/italonet

