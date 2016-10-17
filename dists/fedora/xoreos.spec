# If you want to build the current git checkout, run "build-from-git.sh".
# If you want to build the last stable release of xoreos instead,
# build from this specfile directly.

# Note: xoreos depends on packages from rpmfusion-free.
# So it won't be able to build in e.g. a Fedora Copr
# (https://copr.fedorainfracloud.org/)
# Presumaby OBS will handle this properly?

# Globals, overridden by build script.
%global snapshot 0

Name:           xoreos
Version:        0.0.4

# This is a bit ugly but it works.
%if "%{snapshot}" == "0"
Release:        1%{?dist}
%else
Release:        1.%{snapshot}%{?dist}
%endif

Summary:        A reimplementation of BioWare's Aurora engine (and derivatives)

License:        GPLv3
URL:            https://xoreos.org/

Source0:        https://github.com/xoreos/xoreos/releases/download/%{version}/%{name}-%{version}.tar.gz

BuildRequires:  zlib-devel, freetype-devel, openal-soft-devel, libvorbis-devel,
BuildRequires:  libogg-devel, SDL2-devel, libxml2-devel, lzma-devel, glew-devel

BuildRequires:  libtool, gettext

# Boost dependencies.
BuildRequires:  boost-devel, boost-system, boost-filesystem, boost-atomic,
BuildRequires:  boost-regex, boost-locale, boost-date-time

# Isolated; these are rpmfusion-free dependencies.
BuildRequires:  faad2-devel, libmad-devel, xvidcore-devel

#Requires:

%description
xoreos is an open source implementation of BioWare's Aurora engine and its
derivatives, licensed under the terms of the GNU General Public License
version 3 (or later). The goal is to have all games using this engines
working in a portable manner, starting from Neverwinter Nights and ending
with Dragon Age II.

Currently, the "foundation" work of managing resources, reading many basic
file formats, displaying graphics and playing sounds has been done. All
targeted games show partial ingame graphics, such as the area geometry and
objects, letting you fly around in a "spectator mode". Some games show partial
menus, and something resembling a starting point for a script system is there.

No actual "normal" gameplay is implemented yet, though.

%prep
%setup -q

%build
./autogen.sh
%configure

# When building in place we want to do a make clean.
make clean

make %{?_smp_mflags}

%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}
# We'll get the documentation manually.
rm %{buildroot}%{_pkgdocdir}/*

%files
%{_bindir}/xoreos
%{_mandir}/man6/xoreos.6*
%doc *.md AUTHORS ChangeLog TODO doc/xoreos.conf.example
%license COPYING*

%changelog
* Mon Feb 15 2016 Ben Rosser <rosser.bjr@gmail.com> 0.0.4-1
- Initial package.
