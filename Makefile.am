MAINTAINERCLEANFILES=aclocal.m4 config.h.in config.h.in~ config.guess config.log config.status config.sub configure install-sh missing mkinstalldirs Makefile.in depcomp compile ltmain.sh
SUBDIRS=libevent src include

maintainer-clean-local:
	rm -f ${PACKAGE}-${VERSION}.tar.gz

install-data-local:
	$(INSTALL) -d $(DESTDIR)${localstatedir}/log
	$(INSTALL) -d $(DESTDIR)${localstatedir}/run
