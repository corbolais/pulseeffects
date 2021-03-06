#!/bin/bash
# I use this script to build and publish deb packages in ppa:mikhailnov/pulseeffects (https://launchpad.net/~mikhailnov/+archive/ubuntu/pulseeffects)
# I publish it to allow other people to use it and make it possible to maintain a new PPA easily in case I stop doing it for some reason
# I think, it can also be used for maintaining packages in mainline Debian (minor modifications required)

dir0="$(pwd)"
old_header=$(head -1 ./changelog)

for i in bionic cosmic
do
	old_version="$(cat ./changelog | head -n 1 | awk -F "(" '{print $2}' | awk -F ")" '{print $1}')"
	new_version="${old_version}~${i}1"
	sed -i -re "s/${old_version}/${new_version}/g" ./changelog
	sed -i -re "1s/unstable/$i/" ./changelog
	cd ..
	# -d to allow building *.changes file without installed build dependencies (useful for Launchpad PPA)
	dpkg-buildpackage -S -sa -d
	sed  -i -re "1s/.*/${old_header}/" ./debian/changelog
	cd ..
	# change ppa:mikhailnov/pulseeffects to your PPA at Launchpad.net
	# you must add your public GPG key to Launchpad
	# and add your public SSH key to Launchpad before running dput
	for ppa_name in ppa:mikhailnov/pulseeffects ppa:mikhailnov/utils
	do
		dput -f "$ppa_name" "$(/bin/ls -tr pulseeffects_*_source.changes | tail -n 1)"
	done
	cd "${dir0}"
	sleep 1
done

cd ..
