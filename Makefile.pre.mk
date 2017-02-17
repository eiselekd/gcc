all:

gtags:
	find gcc include gcc libcpp  -type f | grep -e '.c$$\|.h$$' | gtags -i -f -

GCC-PINFO-VERSION=github

#--enable-checking=release
c_:
	-mkdir -p $(CURDIR)/build
	cd $(CURDIR)/build; ../configure \
        --prefix=/opt/gcc-$(GCC-PINFO-VERSION) --disable-nls --enable-languages=c,c++ \
	--target=x86_64-linux-gnu --host=x86_64-linux-gnu --build=x86_64-linux-gnu  \
	--disable-bootstrap --program-suffix=-pinfo --disable-multilib   \
        --disable-shared --disable-nls --disable-libstdcxx-pch --disable-libgomp \
	$(EXTRA_CONF_$(GCC-PINFO-VERSION)) \
	| tee _configure.out

c:
	cd $(CURDIR)/build; make all-gcc \
	| tee _compile.out

i:
	cd $(CURDIR)/build; make install-gcc \
	| tee _compile.out

ca:
	cd $(CURDIR)/build; make all \
	| tee _compile.out

ia:
	cd $(CURDIR)/build; make install \
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
	sudo pip install tornado
