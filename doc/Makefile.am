
EXTRA_DIST = frepple.doxyfile frepple.1 frepplectl.1 apiheader.html apifooter.html apistyles.css

man_MANS = frepple.1 frepplectl.1

if DOC

EXTRA_DIST += _build/html

all-local: rebuildHTML

rebuildHTML:
	-rm -rf _build
	@SPHINXBUILD@ -b html -d _build/doctrees . _build/html
	export PACKAGE_VERSION=$(PACKAGE_VERSION) HAVE_DOT=${HAVE_DOT}; @DOXYGEN@ frepple.doxyfile

clean-local:
	-rm -rf _build

install-data-local: rebuildHTML
	echo "Installing documentation in $(DESTDIR)$(docdir)"; \
	find _build/html -type d -exec $(mkinstalldirs) "$(DESTDIR)$(docdir)/{}" \;; \
	for p in `find _build/html -type f`; do \
	  $(INSTALL_DATA) $$p "$(DESTDIR)$(docdir)/$$p"; \
	done

uninstall-local:
	-rm -rf $(DESTDIR)$(docdir)
  
.PHONY: clean-local uninstall-local install-data-local all-local rebuildHTML

endif
