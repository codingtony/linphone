
AM_CPPFLAGS=\
	-I$(top_srcdir) \
	-I$(top_srcdir)/oRTP/include \
	-I$(top_srcdir)/mediastreamer2/include \
	-I$(top_srcdir)/coreapi \
	-I$(top_srcdir)/include 

COMMON_CFLAGS=\
	-DIN_LINPHONE \
	-D_ORTP_SOURCE \
	$(STRICT_OPTIONS) \
	$(ORTP_CFLAGS) \
	$(MEDIASTREAMER_CFLAGS) \
	$(READLINE_CFLAGS) 


if BUILD_DIALER
bin_PROGRAMS=dialer



dialer_includedir=$(includedir)/linphone

dialer_SOURCES = dialer.c
dialer_CFLAGS = $(COMMON_CFLAGS) 
dialer_LDADD=$(top_builddir)/coreapi/liblinphone.la \
		$(BELLESIP_LIBS) \
		$(LIBXML2_LIBS) -lm
endif
