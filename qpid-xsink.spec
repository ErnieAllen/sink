Name:           qpid-xsink
Version:        1.0.0
Release:        1%{?dist}
Summary:        GUI utilitiy to spout and drain messages for Red Hat MRG qpid

License:        ASL 2.0
URL:            http://eallen.fedorapeople.org/
Source0:        http://eallen.fedorapeople.org/%{name}-%{version}.tar.bz2
Group:          Applications/System
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:  qt4-devel
BuildRequires:  qpid-qmf-devel
BuildRequires:  qpid-cpp-client-devel
BuildRequires:  cmake
BuildRequires:  desktop-file-utils

Requires:       hicolor-icon-theme

%description
qpid-xsink - Graphical utility built with qt that sends
and receives messages to a MRG queue.

%global _app    qpid-gsink

%prep
%setup -q -c %{name}

%build
%cmake %{name}
make %{?_smp_mflags}

%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}

#install the application icon
mkdir -p %{buildroot}%{_datadir}/icons/hicolor/48x48/apps
cp -p %{name}/images/%{name}.png %{buildroot}%{_datadir}/icons/hicolor/48x48/apps

# install the .desktop file
desktop-file-install --dir=%{buildroot}%{_datadir}/applications  \
%{name}/%{name}.desktop

%clean
rm -rf %{buildroot}

%files
%{_bindir}/%{_app}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/48x48/apps/%{name}.png
%doc %{name}/README.txt
%doc %{name}/license.txt

%post
/bin/touch --no-create %{_datadir}/icons/hicolor &>/dev/null || :

%postun
if [ $1 -eq 0 ] ; then
    /bin/touch --no-create %{_datadir}/icons/hicolor &>/dev/null
    /usr/bin/gtk-update-icon-cache %{_datadir}/icons/hicolor &>/dev/null || :
fi

%posttrans
/usr/bin/gtk-update-icon-cache %{_datadir}/icons/hicolor &>/dev/null || :

%changelog

* Tue Mar 20 2012 Ernie Allen <eallen@redhat.com> 1.0.0-1
- Initial version containing /usr/bin/qpid-gsink
