%global debug_package %{nil}
%global source_date_epoch_from_changelog 0

Name:           steam-stream-colorspace
# Always update the version when you update the package
Version:        1
Release:        1%{?dist}.armada
Summary:        Colorspace fix for Steam Remote Play hardware decoding
License:        GPL-2.0-or-later
URL:            https://github.com/armada-os/armada

Source0:        steam-stream-colorspace.tar.gz

BuildRequires:  gcc
BuildRequires:  make

%description
LD_PRELOAD shim that gives the YUV textures Steam's ARM64 streaming_client
creates a BT.709 colorspace, so SDL3's Vulkan renderer accepts them and Remote
Play keeps hardware decoding.

%prep
%autosetup -n work

%build
%make_build CFLAGS="%{optflags}" LDFLAGS="%{build_ldflags}"

%install
%make_install LIBDIR=%{_libdir}

%files
%license LICENSE.md
%doc README.md
%{_libdir}/armada/libsteam-stream-colorspace.so

%changelog
