%define VERSION 2.50
%define RELEASE 3

%define _prefix	/usr/local
%define _bindir %{_prefix}/bin
%define _libdir /usr/lib
%define _ppddir /usr/

%define PR1 	pixusip3100
%define PR2 	pixusip4100
%define PR3 	pixusip8600
%define PR4 	pixmaip1000
%define PR5 	pixmaip1500
%define BUILD_PR 	%{PR1} %{PR2} %{PR3} %{PR4} %{PR5}

%define PR1_ID 	218
%define PR2_ID 	221
%define PR3_ID 	238
%define PR4_ID 	230
%define PR5_ID 	214
%define BUILD_PR_ID 	%{PR1_ID} %{PR2_ID} %{PR3_ID} %{PR4_ID} %{PR5_ID}

Summary: Canon Bubble Jet Printer Driver for Linux Ver.%{VERSION}
Name: bjfilter-common
Version: %{VERSION}
Release: %{RELEASE}
License: See the README files.
Vendor: CANON INC.
Group: Applications/Publishing
Source0: bjfilter-common-%{version}-%{release}.tar.gz
BuildRoot: %{_tmppath}/%{name}-root
Requires: gtk+ libxml libtiff 
#BuildRequires: gtk-devel cups-devel 

%package -n bjfilter-%{PR1}
Summary: Canon Bubble Jet Printer Driver for Linux Ver.%{VERSION}
License: See the README files.
Vendor: CANON INC.
Group: Applications/Publishing
Requires: %{name} = %{version}

%package -n bjfilter-%{PR2}
Summary: Canon Bubble Jet Printer Driver for Linux Ver.%{VERSION}
License: See the README files.
Vendor: CANON INC.
Group: Applications/Publishing
Requires: %{name} = %{version}

%package -n bjfilter-%{PR3}
Summary: Canon Bubble Jet Printer Driver for Linux Ver.%{VERSION}
License: See the README files.
Vendor: CANON INC.
Group: Applications/Publishing
Requires: %{name} = %{version}

%package -n bjfilter-%{PR4}
Summary: Canon Bubble Jet Printer Driver for Linux Ver.%{VERSION}
License: See the README files.
Vendor: CANON INC.
Group: Applications/Publishing
Requires: %{name} = %{version}

%package -n bjfilter-%{PR5}
Summary: Canon Bubble Jet Printer Driver for Linux Ver.%{VERSION}
License: See the README files.
Vendor: CANON INC.
Group: Applications/Publishing
Requires: %{name} = %{version}

%package -n bjfilter-%{PR1}-lprng
Summary: Canon Bubble Jet Printer Driver for Linux Ver.%{VERSION}
License: See the README files.
Vendor: CANON INC.
Group: Applications/Publishing
Requires: %{name} = %{version} bjfilter-%{PR1} = %{version}

%package -n bjfilter-%{PR2}-lprng
Summary: Canon Bubble Jet Printer Driver for Linux Ver.%{VERSION}
License: See the README files.
Vendor: CANON INC.
Group: Applications/Publishing
Requires: %{name} = %{version} bjfilter-%{PR2} = %{version}

%package -n bjfilter-%{PR3}-lprng
Summary: Canon Bubble Jet Printer Driver for Linux Ver.%{VERSION}
License: See the README files.
Vendor: CANON INC.
Group: Applications/Publishing
Requires: %{name} = %{version} bjfilter-%{PR3} = %{version}

%package -n bjfilter-%{PR4}-lprng
Summary: Canon Bubble Jet Printer Driver for Linux Ver.%{VERSION}
License: See the README files.
Vendor: CANON INC.
Group: Applications/Publishing
Requires: %{name} = %{version} bjfilter-%{PR4} = %{version}

%package -n bjfilter-%{PR5}-lprng
Summary: Canon Bubble Jet Printer Driver for Linux Ver.%{VERSION}
License: See the README files.
Vendor: CANON INC.
Group: Applications/Publishing
Requires: %{name} = %{version} bjfilter-%{PR5} = %{version}

%description
Canon Bubble Jet Printer Driver for Linux. 
This Bubble Jet printer driver provides printing functions for Canon Bubble Jet
printers operating under the CUPS (Common UNIX Printing System) environment.

%description -n bjfilter-%{PR1}
Canon Bubble Jet Printer Driver for Linux. 
This Bubble Jet printer driver provides printing functions for Canon Bubble Jet
printers operating under the CUPS (Common UNIX Printing System) environment.

%description -n bjfilter-%{PR1}-lprng
Canon Bubble Jet Printer Driver for Linux. 
This Bubble Jet printer driver provides printing functions for Canon Bubble Jet
printers operating under the LPRng environment.

%description -n bjfilter-%{PR2}
Canon Bubble Jet Printer Driver for Linux. 
This Bubble Jet printer driver provides printing functions for Canon Bubble Jet
printers operating under the CUPS (Common UNIX Printing System) environment.

%description -n bjfilter-%{PR2}-lprng
Canon Bubble Jet Printer Driver for Linux. 
This Bubble Jet printer driver provides printing functions for Canon Bubble Jet
printers operating under the LPRng environment.

%description -n bjfilter-%{PR3}
Canon Bubble Jet Printer Driver for Linux. 
This Bubble Jet printer driver provides printing functions for Canon Bubble Jet
printers operating under the CUPS (Common UNIX Printing System) environment.

%description -n bjfilter-%{PR3}-lprng
Canon Bubble Jet Printer Driver for Linux. 
This Bubble Jet printer driver provides printing functions for Canon Bubble Jet
printers operating under the LPRng environment.

%description -n bjfilter-%{PR4}
Canon Bubble Jet Printer Driver for Linux. 
This Bubble Jet printer driver provides printing functions for Canon Bubble Jet
printers operating under the CUPS (Common UNIX Printing System) environment.

%description -n bjfilter-%{PR4}-lprng
Canon Bubble Jet Printer Driver for Linux. 
This Bubble Jet printer driver provides printing functions for Canon Bubble Jet
printers operating under the LPRng environment.

%description -n bjfilter-%{PR5}
Canon Bubble Jet Printer Driver for Linux. 
This Bubble Jet printer driver provides printing functions for Canon Bubble Jet
printers operating under the CUPS (Common UNIX Printing System) environment.

%description -n bjfilter-%{PR5}-lprng
Canon Bubble Jet Printer Driver for Linux. 
This Bubble Jet printer driver provides printing functions for Canon Bubble Jet
printers operating under the LPRng environment.


%prep

%setup -q

cd libs
    ./autogen.sh --prefix=%{_prefix} 

cd ../bjcups
    ./autogen.sh --prefix=%{_prefix} --enable-progpath=%{_bindir}

cd ../pstocanonbj
    ./autogen.sh --prefix=/usr --enable-progpath=%{_bindir} 

cd ../ppd
    ./autogen.sh --prefix=/usr

cd ../bjcupsmon
    if [ -x /usr/bin/automake-1.8 ] ; then
        ./autogen.sh --prefix=%{_prefix} 
    elif [ -x /usr/bin/automake-1.6 ] ; then
        ./autogen.sh --prefix=%{_prefix} 
    else
        ./autogen-old.sh --prefix=%{_prefix} 
    fi

%build
#make 

%install
mkdir -p ${RPM_BUILD_ROOT}%{_bindir}
mkdir -p ${RPM_BUILD_ROOT}%{_libdir}/cups/filter
mkdir -p ${RPM_BUILD_ROOT}%{_prefix}/share/cups/model

make install DESTDIR=${RPM_BUILD_ROOT}

for PR in %{BUILD_PR}
do
cd bjfilter
    ./autogen.sh --prefix=%{_prefix} --program-suffix=${PR} --enable-libpath=%{_libdir}/bjlib --enable-binpath=%{_bindir}
    make clean
    make
    make install DESTDIR=${RPM_BUILD_ROOT}

cd ../printui
    if [ -x /usr/bin/automake-1.8 ] ; then
        ./autogen.sh --prefix=%{_prefix} --program-suffix=${PR}
    elif [ -x /usr/bin/automake-1.6 ] ; then
        ./autogen.sh --prefix=%{_prefix} --program-suffix=${PR}
    else
        ./autogen-old.sh --prefix=%{_prefix} --program-suffix=${PR}
    fi
    make clean
    make 
    make install DESTDIR=${RPM_BUILD_ROOT}

cd ../stsmon
    if [ -x /usr/bin/automake-1.8 ] ; then
        ./autogen.sh --prefix=%{_prefix} --program-suffix=${PR} --enable-progpath=%{_bindir}
    elif [ -x /usr/bin/automake-1.6 ] ; then
        ./autogen.sh --prefix=%{_prefix} --program-suffix=${PR} --enable-progpath=%{_bindir}
    else
        ./autogen-old.sh --prefix=%{_prefix} --program-suffix=${PR} --enable-progpath=%{_bindir}
    fi
    make clean
    make 
    make install DESTDIR=${RPM_BUILD_ROOT}

cd ..
done

mkdir -p ${RPM_BUILD_ROOT}%{_libdir}/bjlib

for PR_ID in %{BUILD_PR_ID}
do
    install -c -s -m 755 ${PR_ID}/database/*  		${RPM_BUILD_ROOT}%{_libdir}/bjlib
    install -c -s -m 755 ${PR_ID}/libs_bin/*.so.* 	${RPM_BUILD_ROOT}%{_libdir}
done

cd ${RPM_BUILD_ROOT}%{_libdir}
for PR_ID in %{BUILD_PR_ID}
do
	ln -s libcnbpcmcm${PR_ID}.so.* 		libcnbpcmcm${PR_ID}.so
	ln -s libcnbpcnclapi${PR_ID}.so.*	libcnbpcnclapi${PR_ID}.so
	ln -s libcnbpcnclbjcmd${PR_ID}.so.* 	libcnbpcnclbjcmd${PR_ID}.so
	ln -s libcnbpcnclui${PR_ID}.so.* 	libcnbpcnclui${PR_ID}.so
	ln -s libcnbpess${PR_ID}.so.* 		libcnbpess${PR_ID}.so
	ln -s libcnbpo${PR_ID}.so.* 		libcnbpo${PR_ID}.so
done
cd -

%clean
rm -rf $RPM_BUILD_ROOT

%post

%postun

%post -n bjfilter-%{PR1}
if [ -x /sbin/ldconfig ]; then
	/sbin/ldconfig
fi


%postun -n bjfilter-%{PR1}
if [ -x /sbin/ldconfig ]; then
	/sbin/ldconfig
fi
if [ "$1" = 0 ] ; then
	rmdir -p --ignore-fail-on-non-empty %{_prefix}/share/locale/*/LC_MESSAGES
	rmdir -p --ignore-fail-on-non-empty %{_prefix}/share/bjcupsmon
	rmdir -p --ignore-fail-on-non-empty %{_prefix}/share/printui%{PR1}
	rmdir -p --ignore-fail-on-non-empty %{_libdir}/bjlib
	rmdir -p --ignore-fail-on-non-empty %{_bindir}
fi
%post -n bjfilter-%{PR1}-lprng
%postun -n bjfilter-%{PR1}-lprng
if [ "$1" = 0 ] ; then
	rmdir -p --ignore-fail-on-non-empty %{_bindir}
	rmdir -p --ignore-fail-on-non-empty %{_prefix}/share/stsmon%{PR1}/pixmaps
	rmdir -p --ignore-fail-on-non-empty %{_prefix}/share/locale/*/LC_MESSAGES
fi

%post -n bjfilter-%{PR2}
if [ -x /sbin/ldconfig ]; then
	/sbin/ldconfig
fi
%postun -n bjfilter-%{PR2}
if [ -x /sbin/ldconfig ]; then
	/sbin/ldconfig
fi
if [ "$1" = 0 ] ; then
	rmdir -p --ignore-fail-on-non-empty %{_prefix}/share/locale/*/LC_MESSAGES
	rmdir -p --ignore-fail-on-non-empty %{_prefix}/share/bjcupsmon
	rmdir -p --ignore-fail-on-non-empty %{_prefix}/share/printui%{PR2}
	rmdir -p --ignore-fail-on-non-empty %{_libdir}/bjlib
	rmdir -p --ignore-fail-on-non-empty %{_bindir}
fi
%post -n bjfilter-%{PR2}-lprng
%postun -n bjfilter-%{PR2}-lprng
if [ "$1" = 0 ] ; then
	rmdir -p --ignore-fail-on-non-empty %{_bindir}
	rmdir -p --ignore-fail-on-non-empty %{_prefix}/share/stsmon%{PR2}/pixmaps
	rmdir -p --ignore-fail-on-non-empty %{_prefix}/share/locale/*/LC_MESSAGES
fi

%post -n bjfilter-%{PR3}
if [ -x /sbin/ldconfig ]; then
	/sbin/ldconfig
fi
%postun -n bjfilter-%{PR3}
if [ -x /sbin/ldconfig ]; then
	/sbin/ldconfig
fi
if [ "$1" = 0 ] ; then
	rmdir -p --ignore-fail-on-non-empty %{_prefix}/share/locale/*/LC_MESSAGES
	rmdir -p --ignore-fail-on-non-empty %{_prefix}/share/printui%{PR3}
	rmdir -p --ignore-fail-on-non-empty %{_prefix}/share/bjcupsmon
	rmdir -p --ignore-fail-on-non-empty %{_libdir}/bjlib
	rmdir -p --ignore-fail-on-non-empty %{_bindir}
fi
%post -n bjfilter-%{PR3}-lprng
%postun -n bjfilter-%{PR3}-lprng
if [ "$1" = 0 ] ; then
	rmdir -p --ignore-fail-on-non-empty %{_bindir}
	rmdir -p --ignore-fail-on-non-empty %{_prefix}/share/stsmon%{PR3}/pixmaps
	rmdir -p --ignore-fail-on-non-empty %{_prefix}/share/locale/*/LC_MESSAGES
fi

%post -n bjfilter-%{PR4}
if [ -x /sbin/ldconfig ]; then
	/sbin/ldconfig
fi
%postun -n bjfilter-%{PR4}
if [ -x /sbin/ldconfig ]; then
	/sbin/ldconfig
fi
if [ "$1" = 0 ] ; then
	rmdir -p --ignore-fail-on-non-empty %{_prefix}/share/locale/*/LC_MESSAGES
	rmdir -p --ignore-fail-on-non-empty %{_prefix}/share/printui%{PR4}
	rmdir -p --ignore-fail-on-non-empty %{_prefix}/share/bjcupsmon
	rmdir -p --ignore-fail-on-non-empty %{_libdir}/bjlib
	rmdir -p --ignore-fail-on-non-empty %{_bindir}
fi
%post -n bjfilter-%{PR4}-lprng
%postun -n bjfilter-%{PR4}-lprng
if [ "$1" = 0 ] ; then
	rmdir -p --ignore-fail-on-non-empty %{_bindir}
	rmdir -p --ignore-fail-on-non-empty %{_prefix}/share/stsmon%{PR4}/pixmaps
	rmdir -p --ignore-fail-on-non-empty %{_prefix}/share/locale/*/LC_MESSAGES
fi

%post -n bjfilter-%{PR5}
if [ -x /sbin/ldconfig ]; then
	/sbin/ldconfig
fi
%postun -n bjfilter-%{PR5}
if [ -x /sbin/ldconfig ]; then
	/sbin/ldconfig
fi
if [ "$1" = 0 ] ; then
	rmdir -p --ignore-fail-on-non-empty %{_prefix}/share/locale/*/LC_MESSAGES
	rmdir -p --ignore-fail-on-non-empty %{_prefix}/share/printui%{PR5}
	rmdir -p --ignore-fail-on-non-empty %{_prefix}/share/bjcupsmon
	rmdir -p --ignore-fail-on-non-empty %{_libdir}/bjlib
	rmdir -p --ignore-fail-on-non-empty %{_bindir}
fi
%post -n bjfilter-%{PR5}-lprng
%postun -n bjfilter-%{PR5}-lprng
if [ "$1" = 0 ] ; then
	rmdir -p --ignore-fail-on-non-empty %{_bindir}
	rmdir -p --ignore-fail-on-non-empty %{_prefix}/share/stsmon%{PR5}/pixmaps
	rmdir -p --ignore-fail-on-non-empty %{_prefix}/share/locale/*/LC_MESSAGES
fi

%files
%defattr(-,root,root)
%{_libdir}/cups/filter/pstocanonbj
%{_libdir}/cups/backend/canon_parallel
%{_libdir}/cups/backend/canon_usb
%{_bindir}/bjcups

%files -n bjfilter-%{PR1}
%defattr(-,root,root)
%{_bindir}/bjcupsmon
%{_bindir}/lgmon%{PR1}
%{_bindir}/printui%{PR1}
%{_ppddir}/share/cups/model/canon%{PR1}.ppd
%{_prefix}/share/locale/*/LC_MESSAGES/bjcupsmon.mo
%{_prefix}/share/locale/*/LC_MESSAGES/printui%{PR1}.mo
%{_prefix}/share/bjcupsmon/*
%{_prefix}/share/printui%{PR1}/*

%{_bindir}/bjfilter%{PR1}
%{_libdir}/libcnbp*%{PR1_ID}.so*
%{_libdir}/bjlib/bjfilter%{PR1}.conf
%{_libdir}/bjlib/cnb_%{PR1_ID}0.tbl
%{_libdir}/bjlib/cnbpname%{PR1_ID}.tbl

%files -n bjfilter-%{PR1}-lprng
%defattr(-,root,root)
%{_bindir}/%{PR1}_ps
%{_bindir}/%{PR1}_raw
%{_bindir}/bjcmd%{PR1}
%{_bindir}/stsmon%{PR1}
%{_prefix}/share/stsmon%{PR1}/pixmaps/*.xpm
%{_prefix}/share/locale/*/LC_MESSAGES/stsmon%{PR1}.mo

%files -n bjfilter-%{PR2}
%defattr(-,root,root)
%{_bindir}/bjcupsmon
%{_bindir}/lgmon%{PR2}
%{_bindir}/printui%{PR2}
%{_ppddir}/share/cups/model/canon%{PR2}.ppd
%{_prefix}/share/locale/*/LC_MESSAGES/bjcupsmon.mo
%{_prefix}/share/locale/*/LC_MESSAGES/printui%{PR2}.mo
%{_prefix}/share/bjcupsmon/*
%{_prefix}/share/printui%{PR2}/*

%{_bindir}/bjfilter%{PR2}
%{_libdir}/libcnbp*%{PR2_ID}.so*
%{_libdir}/bjlib/bjfilter%{PR2}.conf
%{_libdir}/bjlib/cnb_%{PR2_ID}0.tbl
%{_libdir}/bjlib/cnbpname%{PR2_ID}.tbl

%files -n bjfilter-%{PR2}-lprng
%defattr(-,root,root)
%{_bindir}/%{PR2}_ps
%{_bindir}/%{PR2}_raw
%{_bindir}/bjcmd%{PR2}
%{_bindir}/stsmon%{PR2}
%{_prefix}/share/stsmon%{PR2}/pixmaps/*.xpm
%{_prefix}/share/locale/*/LC_MESSAGES/stsmon%{PR2}.mo

%files -n bjfilter-%{PR3}
%defattr(-,root,root)
%{_bindir}/bjcupsmon
%{_bindir}/lgmon%{PR3}
%{_bindir}/printui%{PR3}
%{_ppddir}/share/cups/model/canon%{PR3}.ppd
%{_prefix}/share/locale/*/LC_MESSAGES/bjcupsmon.mo
%{_prefix}/share/locale/*/LC_MESSAGES/printui%{PR3}.mo
%{_prefix}/share/bjcupsmon/*
%{_prefix}/share/printui%{PR3}/*

%{_bindir}/bjfilter%{PR3}
%{_libdir}/libcnbp*%{PR3_ID}.so*
%{_libdir}/bjlib/bjfilter%{PR3}.conf
%{_libdir}/bjlib/cnb_%{PR3_ID}0.tbl
%{_libdir}/bjlib/cnbpname%{PR3_ID}.tbl

%files -n bjfilter-%{PR3}-lprng
%defattr(-,root,root)
%{_bindir}/%{PR3}_ps
%{_bindir}/%{PR3}_raw
%{_bindir}/bjcmd%{PR3}
%{_bindir}/stsmon%{PR3}
%{_prefix}/share/stsmon%{PR3}/pixmaps/*.xpm
%{_prefix}/share/locale/*/LC_MESSAGES/stsmon%{PR3}.mo

%files -n bjfilter-%{PR4}
%defattr(-,root,root)
%{_bindir}/bjcupsmon
%{_bindir}/lgmon%{PR4}
%{_bindir}/printui%{PR4}
%{_ppddir}/share/cups/model/canon%{PR4}.ppd
%{_prefix}/share/locale/*/LC_MESSAGES/bjcupsmon.mo
%{_prefix}/share/locale/*/LC_MESSAGES/printui%{PR4}.mo
%{_prefix}/share/bjcupsmon/*
%{_prefix}/share/printui%{PR4}/*

%{_bindir}/bjfilter%{PR4}
%{_libdir}/libcnbp*%{PR4_ID}.so*
%{_libdir}/bjlib/bjfilter%{PR4}.conf
%{_libdir}/bjlib/cnb_%{PR4_ID}0.tbl
%{_libdir}/bjlib/cnbpname%{PR4_ID}.tbl

%files -n bjfilter-%{PR4}-lprng
%defattr(-,root,root)
%{_bindir}/%{PR4}_ps
%{_bindir}/%{PR4}_raw
%{_bindir}/bjcmd%{PR4}
%{_bindir}/stsmon%{PR4}
%{_prefix}/share/stsmon%{PR4}/pixmaps/*.xpm
%{_prefix}/share/locale/*/LC_MESSAGES/stsmon%{PR4}.mo

%files -n bjfilter-%{PR5}
%defattr(-,root,root)
%{_bindir}/bjcupsmon
%{_bindir}/lgmon%{PR5}
%{_bindir}/printui%{PR5}
%{_ppddir}/share/cups/model/canon%{PR5}.ppd
%{_prefix}/share/locale/*/LC_MESSAGES/bjcupsmon.mo
%{_prefix}/share/locale/*/LC_MESSAGES/printui%{PR5}.mo
%{_prefix}/share/bjcupsmon/*
%{_prefix}/share/printui%{PR5}/*

%{_bindir}/bjfilter%{PR5}
%{_libdir}/libcnbp*%{PR5_ID}.so*
%{_libdir}/bjlib/bjfilter%{PR5}.conf
%{_libdir}/bjlib/cnb_%{PR5_ID}0.tbl
%{_libdir}/bjlib/cnbpname%{PR5_ID}.tbl

%files -n bjfilter-%{PR5}-lprng
%defattr(-,root,root)
%{_bindir}/%{PR5}_ps
%{_bindir}/%{PR5}_raw
%{_bindir}/bjcmd%{PR5}
%{_bindir}/stsmon%{PR5}
%{_prefix}/share/stsmon%{PR5}/pixmaps/*.xpm
%{_prefix}/share/locale/*/LC_MESSAGES/stsmon%{PR5}.mo

%ChangeLog

