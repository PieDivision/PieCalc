Name:           piecalc
Version:        1.0
Release:        1%{?dist}
Summary:        Solving equations

License:        GPLv3
URL:			ssh://ivs.fit.vutbr.cz/ivs-proj3/Pie_Division/repo
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  gcc
BuildRequires:	gtk3-devel
Requires:       gtk3

%description
Calculator for solving equations. This calculator supports any mathematical functions, for example log, ln, etc.


%prep
#setup q
cd ~/rpmbuild/BUILD/ && tar -xzf ../SOURCES/piecalc-1.0.tar.gz

%build
make linux


%install
#rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/bin
mkdir -p $RPM_BUILD_ROOT/usr/share/man/man1
mkdir -p $RPM_BUILD_ROOT/usr/share/applications
%make_install


%files
%doc README LICENSE
/usr/bin/piecalc
/usr/share/piecalc/piecalc.glade
/usr/share/piecalc/icon_*.png
/usr/share/man/man1/piecalc.1.gz
/usr/share/applications/pie_calc.desktop


%changelog
* Wed Apr 15 2015 David Benes <xbenes41@stud.fit.vutbr.cz> 1.0
- Initial package for Fedora
