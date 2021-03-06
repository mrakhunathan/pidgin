perl_dirs = common

common_sources = \
	common/GtkAccount.xs \
	common/GtkBlist.xs \
	common/GtkConn.xs \
	common/GtkConv.xs \
	common/GtkConvWin.xs \
	common/GtkDebug.xs \
	common/GtkDialogs.xs \
	common/GtkFt.xs \
	common/GtkLog.xs \
	common/GtkMenuTray.xs \
	common/GtkPlugin.xs \
	common/GtkPluginPref.xs \
	common/GtkPounce.xs \
	common/GtkPrefs.xs \
	common/GtkPrivacy.xs \
	common/GtkRoomlist.xs \
	common/GtkSavedStatuses.xs \
	common/GtkSession.xs \
	common/GtkSound.xs \
	common/GtkStatusBox.xs \
	common/GtkUtils.xs \
	common/Makefile.PL.in \
	common/Pidgin.pm \
	common/Pidgin.xs \
	common/gtkmodule.h \
	common/typemap

EXTRA_DIST = \
	Makefile.mingw \
	common/Makefile.mingw \
	$(common_sources)

common/Makefile: common/Makefile.PL
	$(AM_V_GEN)if test "x${top_srcdir}" != "x${top_builddir}"; then \
		for f in ${common_sources}; do \
			srcloc=${srcdir}; \
			case $$srcloc in /*) ;; *) srcloc=../${srcdir} ;; esac; \
			${LN_S} -f $$srcloc/$$f $$f; \
		done; \
	fi
	$(AM_V_at)cd common && $(perlpath) Makefile.PL > /dev/null

common/Makefile.PL: common/Makefile.PL.in $(top_builddir)/config.status
	cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@ $(am__depfiles_maybe)

all-local: common/Makefile
	@for dir in $(perl_dirs); do \
		cd $$dir && \
		if [ ! -f Makefile ]; then \
			$(perlpath) Makefile.PL; \
		fi && \
		($(MAKE) CC="@$(abs_top_srcdir)/libpurple/tag.sh CC $(CC)" LD="@$(abs_top_srcdir)/libpurple/tag.sh LD $(CC)" PERLRUN="@$(abs_top_srcdir)/libpurple/tag.sh PERL $(PERL)" CCFLAGS="$(PERL_CFLAGS) $(CFLAGS)" CP="@cp" RM_F="@rm -f" CHMOD="@chmod" $(PERL_EXTRA_OPTS) || \
		$(MAKE) CC="@$(abs_top_srcdir)/libpurple/tag.sh CC $(CC)" LD="@$(abs_top_srcdir)/libpurple/tag.sh LD $(CC)" PERLRUN="@$(abs_top_srcdir)/libpurple/tag.sh PERL $(PERL)" CCFLAGS="$(PERL_CFLAGS) $(CFLAGS)" CP="@cp" RM_F="@rm -f" CHMOD="@chmod" $(PERL_EXTRA_OPTS)) && \
		cd ..; \
	done

install-exec-local:
	@for dir in $(perl_dirs); do \
		cd $$dir; \
		$(MAKE) install; \
		cd ..; \
	done

# Evil Hack (TM)
# ... which doesn't work with DESTDIR installs. FIXME?
uninstall-local:
	@for dir in $(perl_dirs); do \
		cd $$dir && \
		`$(MAKE) uninstall | grep unlink | sed -e 's#/usr#${prefix}#' -e 's#unlink#rm -f#'` && \
		cd ..; \
	done

clean-generic:
	@for dir in $(perl_dirs); do \
		cd $$dir; \
		$(MAKE) clean; \
		cd ..; \
	done
	cd common ; rm -rf *.c *.o pm_to_blib Pidgin.bs MYMETA.* blib/*/.exists blib/*/auto/Pidgin blib/*/Pidgin.*pm ; cd ..
	rm -f *.so

distclean-generic:
	@for dir in $(perl_dirs); do \
		cd $$dir; \
		$(MAKE) realclean; \
		rm -f Makefile.PL; \
		rm -f Makefile.old; \
		rm -f Makefile; \
		cd ..; \
	done

	@rm -f Makefile

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/libpurple \
	-I$(top_builddir)/libpurple \
	-I$(top_srcdir)/pidgin \
	$(DEBUG_CFLAGS) \
	$(GTK_CFLAGS) \
	$(GPLUGIN_CFLAGS) \
	$(PLUGIN_CFLAGS) \
	$(PERL_CFLAGS)
