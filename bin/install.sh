#!/bin/bash
#Install the whitelist executable and apparmor profiles
#This should be run any time the whitelist.txt file is updated to create the symbolic links
whitelist_exe="./whitelist"
whitelist_txt_file="/etc/whitelist.txt"


if [ ! -f $whitelist_exe ]; then
	echo "Could not locate whitelist executable"
	exit
fi

if [ ! -f $whitelist_txt_file ]; then

	if [ -f ./whitelist.txt ]; then
		sudo cp ./whitelist.txt $whitelist_txt_file
	else
		echo "Could not locate whitelist at $whitelist_txt_file"
		exit
	fi
fi

sudo chown root $whitelist_exe
sudo chgrp root $whitelist_exe
sudo chmod 511 $whitelist_exe

sudo chown root $whitelist_txt_file
sudo chgrp root $whitelist_txt_file
sudo chmod 644 $whitelist_txt_file

#Remove all symbolic links
sudo find ./ -type l -exec rm {} \;
	
#Loop through whitelist and create files.
cat $whitelist_txt_file | cut -d' ' -f1-2 | while read whitelist_string
do
	whitelist_cmds=`echo $whitelist_string | cut -d' ' -f2`
	whitelist_path=`echo $whitelist_string | cut -d' ' -f1`
	
	if [ ! -f $whitelist_cmds ]; then
		sudo ln -s $whitelist_exe $whitelist_cmds
	fi
	
	whitelist_apparmor=`echo $whitelist_path | cut -c2- | tr '/' '.'`
	whitelist_apparmor="/etc/apparmor.d/$whitelist_apparmor"
	if [ ! -f $whitelist_apparmor ]; then
		echo "Please add apparmor config to $whitelist_apparmor to restrict command from direct execution"
	fi
	
	if [ ! -f "/etc/apparmor.d/usr.bin.whitelist.whitelist" ]; then
		echo "Please add apparmor config to "/etc/apparmor.d/usr.bin.whitelist" to allow execution of whitelist command"
	fi
	
done

echo "Install complete"
