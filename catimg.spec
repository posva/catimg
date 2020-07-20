Name:           catimg
Version:        2.7.0
Release:        1%{?dist}
Summary:        Print images in a terminal with 256 colors support

License:        MIT
URL:            https://github.com/posva/catimg
Source0:        https://github.com/posva/%{name}/archive/v%{version}/%{name}-%{version}.tar.gz

BuildRequires:  cmake >= 2.8
BuildRequires:  gcc

%description
%{name} is a little program written in C with no dependencies that prints
images in terminal. It supports JPEG, PNG, ICO and GIF formats.

%prep
%autosetup

%build
mkdir -p build
pushd build
%cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo \
       -DMAN_OUTPUT_PATH=%{_mandir}/man1 \
       ..
%make_build
popd

%install
pushd build
%make_install
popd

install -D -p -m 644 completion/_catimg $RPM_BUILD_ROOT%{_datadir}/zsh/site-functions/_catimg

%files
%license LICENSE
%doc README.md
%{_bindir}/%{name}
%dir %{_datadir}/zsh
%dir %{_datadir}/zsh/site-functions
%{_datadir}/zsh/site-functions/_catimg
%{_mandir}/man1/%{name}.1*

%changelog
