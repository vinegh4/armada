#!/usr/bin/bash
# Runs inside the builder container. See ../build-local.sh for the contract.
#
# The tarball's top-level directory must stay `work`: steam-stream-colorspace.spec
# unpacks it with `%autosetup -n work`.
set -euxo pipefail

NAME=steam-stream-colorspace

rm -rf out
mkdir -p out

cat >/etc/rpm/macros.armada <<EOF
%_buildhost armada-builder
%packager Armada
%vendor Armada
EOF

dnf -y install --skip-unavailable \
  rpm-build rpmdevtools dnf-plugins-core \
  tar gzip
dnf -y builddep "${NAME}.spec"
rpmdev-setuptree

cp "${NAME}.spec" ~/rpmbuild/SPECS/
tar -C / \
  --exclude=work/out \
  --exclude=work/build \
  -czf ~/rpmbuild/SOURCES/${NAME}.tar.gz \
  work

rpmbuild -bb ~/rpmbuild/SPECS/${NAME}.spec

cp ~/rpmbuild/RPMS/aarch64/*.rpm /work/out/
