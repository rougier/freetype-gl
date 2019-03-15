Name:           freetype-gl
Version:        0.1.3
Release:        1%{?dist}
Summary:        FreeType GL - A C OpenGL FreeType engine

License:        BSD
URL:            https://github.com/1div0
Source0:        %url/%{name}/archive/%{name}-%{version}.tar.gz

BuildRequires:  gcc
BuildRequires:  gcc-c++
BuildRequires:  cmake3
BuildRequires:  make

%description
OpenGL text using one vertex buffer, one texture and FreeType

%package devel
Summary:        FreeType GL development package
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description devel
Header files, libraries and documentation for building the FreeType GL applications

%prep
%autosetup -p1 -n %{name}-%{version}

%build
%cmake3 -Wno-dev \
  -DCMAKE_INSTALL_BINDIR:PATH=%{_bindir} \
  -DCMAKE_INSTALL_LIBDIR:PATH=%{_libdir} \
  -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON \
  -DCMAKE_SKIP_RPATH:BOOL=YES \
  -DENABLE_PIC:BOOL=ON \
  -DCMAKE_BUILD_TYPE=Release \
  .
%make_build

%install
%make_install

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files
%license LICENSE
%doc README.md
%{_bindir}/makefont
%{_libdir}/lib*.so.*

%files devel
%doc doc/html/*.{html,png}
%{_includedir}/%{name}
%{_libdir}/lib*.so

%changelog
