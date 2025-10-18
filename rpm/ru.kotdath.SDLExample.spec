%define __provides_exclude_from ^%{_datadir}/%{name}/lib/.*$
%define __requires_exclude ^(libSDL2-2.0.*).*$

Name:       ru.kotdath.SDLExample
Summary:    Template
Version:    1.0.0
Release:    1
License:    BSD-3-Clause
URL:        https://developer.auroraos.ru/open-source
Source0:    %{name}-%{version}.tar.bz2

Requires:   sailfishsilica-qt5 >= 0.10.9
BuildRequires:  pkgconfig(auroraapp)
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires: cmake
BuildRequires: pkgconfig(wayland-egl)
BuildRequires: pkgconfig(wayland-client)
BuildRequires: pkgconfig(wayland-cursor)
BuildRequires: pkgconfig(wayland-protocols)
BuildRequires: pkgconfig(wayland-scanner)
BuildRequires: pkgconfig(egl)
BuildRequires: pkgconfig(glesv1_cm)
BuildRequires: pkgconfig(glesv2)
BuildRequires: pkgconfig(xkbcommon)
BuildRequires: pkgconfig(libpulse-simple)

%description
Aurora OS Application Template.

%prep
%autosetup

%build
%cmake \
    -GNinja
%ninja_build

%install

mkdir -p %{buildroot}/%{_datadir}/%{name}/lib/
cp $RPM_BUILD_DIR/sdlbin/*.so* %{buildroot}/%{_datadir}/%{name}/lib/
chmod 600 %{buildroot}/%{_datadir}/%{name}/lib/*.so*

%ninja_install

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%defattr(644,root,root,-)
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
