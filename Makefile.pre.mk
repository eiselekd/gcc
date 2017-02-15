all:

gtags:
	find gcc include gcc libcpp  -type f | grep -e '.c$$\|.h$$' | gtags -i -f -

GCC-PINFO-VERSION=github

c_:
	-mkdir -p $(CURDIR)/build
	cd $(CURDIR)/build; ../configure \
        --prefix=/opt/gcc-$(GCC-PINFO-VERSION) --disable-nls --enable-languages=c \
	--target=x86_64-linux-gnu --host=x86_64-linux-gnu --build=x86_64-linux-gnu  \
	--disable-bootstrap --program-suffix=-pinfo --disable-multilib --enable-checking=release  \
        --disable-shared --disable-nls --disable-libstdcxx-pch --disable-libgomp \
	$(EXTRA_CONF_$(GCC-PINFO-VERSION)) \
	| tee _configure.out

c:
	cd $(CURDIR)/build; make all-gcc \
	| tee _compile.out

i:
	cd $(CURDIR)/build; make install-gcc \
	| tee _compile.out


test:
	/opt/gcc-$(GCC-PINFO-VERSION)/bin/gcc-pinfo -c t/m.c -o t/m.o

