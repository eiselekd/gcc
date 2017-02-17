all:

gtags:
	find gcc include gcc libcpp  -type f | grep -e '.c$$\|.h$$' | gtags -i -f -

GCC-PINFO-VERSION=github

E=export PATH=$(CURDIR)/build-autoconfig/bin:$$PATH;

#--enable-checking=release
#--enable-maintainer-mode
c_:
	-mkdir -p $(CURDIR)/build
	$(E)cd $(CURDIR)/build; ../configure \
        --prefix=/opt/gcc-$(GCC-PINFO-VERSION) --disable-nls --enable-languages=c,c++ \
	--target=x86_64-linux-gnu --host=x86_64-linux-gnu --build=x86_64-linux-gnu  \
	--disable-bootstrap --program-suffix=-pinfo --disable-multilib   \
        --disable-shared --disable-nls --disable-libstdcxx-pch --disable-libgomp \
	$(EXTRA_CONF_$(GCC-PINFO-VERSION))  \
	| tee _configure.out

c:
	$(E)cd $(CURDIR)/build; make all-gcc \
	| tee _compile.out

i:
	$(E)cd $(CURDIR)/build; make install-gcc \
	| tee _compile.out

ca:
	$(E)cd $(CURDIR)/build; make all \
	| tee _compile.out

ia:
	$(E)cd $(CURDIR)/build; make install \
	| tee _compile.out

test:
	/opt/gcc-$(GCC-PINFO-VERSION)/bin/gcc-pinfo -fparse-info=t/m.pinfo -undef -nostdinc -c t/m.c -o t/m.o
	/opt/gcc-$(GCC-PINFO-VERSION)/bin/g++-pinfo -Ibuild/build-x86_64-linux-gnu/libcpp -Ilibcpp -I. -Ilibcpp/../include -Ilibcpp/include  -g -O2 -W -Wall -Wno-narrowing -Wwrite-strings -Wmissing-format-attribute -pedantic -Wno-long-long  -fno-exceptions -fno-rtti -Ilibcpp -I. -Ilibcpp/../include -Ilibcpp/include  -fparse-info=t/macro.pinfo -c -o t/macro.o libcpp/macro.c

#--param ggc-min-expand=0 --param ggc-min-heapsize=0

ve:
	export FLYCHECK_GENERIC_SRC=$(CURDIR); \
	export FLYCHECK_GENERIC_BUILD=$(CURDIR)/build; \
	export FLYCHECK_GENERIC_CMD=all-gcc; \
	emacs . &

apt:
	sudo apt-get install python-pip
	sudo pip install tornado python-dev

pinfo:
	cd libcpp; rm -rf build pinfo.so;  \
		python setup.py build_ext --inplace; 


a:
	if [ ! -f autoconf-2.64.tar.bz2 ]; then wget https://ftp.gnu.org/pub/gnu/autoconf/autoconf-2.64.tar.bz2; fi
	tar xvf autoconf-2.64.tar.bz2
	cd autoconf-2.64; ./configure --prefix=$(CURDIR)/build-autoconfig; make; make install
	if [ ! -f automake-1.11.tar.bz2 ]; then wget https://ftp.gnu.org/gnu/automake/automake-1.11.tar.bz2; fi
	tar xvf automake-1.11.tar.bz2
	cd automake-1.11; ./configure --prefix=$(CURDIR)/build-autoconfig; make; make install
