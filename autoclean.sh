make distclean

find . -name "*~" -delete
find . -regex "\.[^\.]*\.in\$" -not -path "*po*" -delete

rm po/Makefile.in.in

rm -rf m4 autom4te.cache

rm aclocal.m4 compile configure config.guess config.h.in config.sub configure.ac depcomp install-sh ltmain.sh missing INSTALL
