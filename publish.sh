#!/bin/bash

last_version=$(hg tags -q | sort -n | tail -n 1 | xargs)
echo "Last released version was ${last_version}"
read -p "Enter release version (vN.N.N): " version

changeset=$(hg id -i | xargs)

# Check the changeset length (it'll be 13 if revision hasn't been committed)
if [ ${#changeset} -ne 12 ]
then
	echo "Changeset '${changeset}' looks wrong"
	exit 3
else
	docker-compose run cordlessvacuumstarterbuild --publish --version "$version"
	hg tag $version
	hg push
fi
