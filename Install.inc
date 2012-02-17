.PHONY: installdirs
installdirs:
	$(INSTALL) -d $(DESTDIR)$(libdir)
	$(INSTALL) -d $(DESTDIR)$(includedir)$(includesubdir)
	$(INSTALL) -d $(DESTDIR)$(bindir)
	$(INSTALL) -d $(DESTDIR)$(datadir)/Wt/Wc/locales
	$(INSTALL) -d $(DESTDIR)$(datadir)/Wt/resources/Wc/css
	$(INSTALL) -d $(DESTDIR)$(datadir)/Wt/resources/Wc/js
	$(INSTALL) -d $(DESTDIR)$(mandir)/man1
	$(INSTALL) -d $(DESTDIR)$(exampleslibdir)
	$(INSTALL) -d $(DESTDIR)$(examplesdocdir)
	$(INSTALL) -d $(DESTDIR)$(referencedir)

.PHONY: install-buildless
install-buildless: $(headers) locales-test.py $$(mans) \
		$$(locales) $$(css) $$(js) \
		installdirs
	$(INSTALL_DATA) $(headers) $(DESTDIR)$(includedir)$(includesubdir)
	$(INSTALL_PROGRAM) locales-test.py $(DESTDIR)$(bindir)/locales-test
	$(INSTALL_DATA) locales-test.1 $(DESTDIR)$(mandir)/man1/
	$(INSTALL_DATA) $(locales) $(DESTDIR)$(datadir)/Wt/Wc/locales/
	$(INSTALL_DATA) $(css) $(DESTDIR)$(datadir)/Wt/resources/Wc/css/
	$(INSTALL_DATA) $(js) $(DESTDIR)$(datadir)/Wt/resources/Wc/js/

.PHONY: install-lib
install-lib: build-lib install-buildless installdirs
	$(INSTALL_DATA) $(DYNAMIC_LIB_PATH) $(DESTDIR)$(libdir)
	ln -f -s $(DYNAMIC_LIB_SONAME) $(DESTDIR)$(libdir)/$(DYNAMIC_LIB_SHORT)
	ln -f -s $(DYNAMIC_LIB) $(DESTDIR)$(libdir)/$(DYNAMIC_LIB_SONAME)
	$(INSTALL_DATA) $(STATIC_LIB_PATH) $(DESTDIR)$(libdir)

.PHONY: install-examples
install-examples: examples installdirs
	$(INSTALL_PROGRAM) $(examples_binaries) $(DESTDIR)$(exampleslibdir)
	$(INSTALL_DATA) $(examples_cpp) $(DESTDIR)$(examplesdocdir)
	ln -f -s $(addprefix $(exampleslibreldir)/,$(notdir $(examples_binaries))) \
		$(DESTDIR)$(examplesdocdir)

.PHONY: install-doc
install-doc: doc
	cp -r doc/* $(DESTDIR)$(referencedir)
	rm -f $(DESTDIR)$(referencelink)
	ln -f -s $(call relpath,$(referencedir),$(dir $(referencelink))) \
		$(DESTDIR)$(referencelink)

.PHONY: install
install: install-lib install-buildless
ifeq (,$(INSTALL_NO_EXAMPLES))
	$(MAKE) install-examples
endif
ifeq (,$(INSTALL_NO_DOC))
	$(MAKE) install-doc
endif

.PHONY: dist
dist: $$(dist_files)
	mkdir -p $(dist_dir)
	cp --parents $^ $(dist_dir)
	tar -czf $(dist_tar) $(addprefix $(dist_dir)/, $(dist_files))

.PHONY: deb
deb:
	$(MAKE) dist
	cp -fl $(dist_tar) $(name)_$(VERSION).orig.tar.gz
	rm -rf $(dist_dir)/debian
	mkdir -p $(dist_dir)/debian
	cp -flr debian/* $(dist_dir)/debian
	sed -e 's@SONAME@$(SONAME)@g' -e '/DESCR/r debian/descr.in' -e '/DESCR/d' \
		< debian/control.in > $(dist_dir)/debian/control
	cp -fl debian/libwtclassesSONAME.install.in \
		$(dist_dir)/debian/libwtclasses$(SONAME).install
	cd $(dist_dir)/debian/ && rm -f *.in
	cd $(dist_dir) && debuild

