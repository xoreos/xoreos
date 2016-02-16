#!/bin/bash

# This is a helper script to update the Release tag of the specfile
# and then invoke rpmbuild --build-in-place with the correct options
# to build from the root of the git checkout.

# You need rpmbuild to build the rpm.

# Thanks to StackOverflow:
# https://stackoverflow.com/questions/59895/can-a-bash-script-tell-what-directory-its-stored-in
RPM_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

XOREOS_ROOT=`git rev-parse --show-toplevel`
SPEC_NAME=${RPM_DIR}/xoreos.spec

# Compute the release snapshot, create a temporary spec. Also redine _rpmdir
SNAPSHOT=`date +"%Y%m%d"`
sed "s,\%global snapshot .*,\%global snapshot ${SNAPSHOT}git," < ${SPEC_NAME} > ${SPEC_NAME}.tmp
sed "1s,^,\%define _rpmdir ${RPM_DIR}\n," -i ${SPEC_NAME}.tmp

# Create a buildroot directory.
mkdir -p ${RPM_DIR}/buildroot

# Invoke rpmbuild from the root of the git repo (so --build-in-place works).
# Also set the --buildroot and --
cd ${XOREOS_ROOT}
rpmbuild --build-in-place --rmspec --buildroot ${RPM_DIR}/buildroot -bb ${SPEC_NAME}.tmp
rm -rf ${RPM_DIR}/buildroot
