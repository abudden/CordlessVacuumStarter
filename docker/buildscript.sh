if [ ! -d tools/ ]
then
	rm -rf tools
	hg clone https://mbed.org/users/mbed_official/code/mbed-sdk-tools && \
		mv mbed-sdk-tools tools || \
		exit 3
fi
if [ ! -e /home/al/.setupcomplete ]
then
	cd tools && \
		pip install -r requirements.txt && \
		pip install jsonschema future pyelftools && \
		cd .. && \
		touch /home/al/.setupcomplete
fi
PATH=$PATH:/home/al/gcc-arm-none/bin python3 compile.py "$@"
