C=gcc
DEB=-O2
CFLAGS=-g $(DEB) -Wall -c
GTKFLAGS=-DHAVE_CONFIG_H -I. -I. -I.. -I/usr/include/gtk-1.2 -I/usr/include/glib-1.2 -I/usr/lib/glib/include -I/usr/X11R6/include -g -O2 -Wall -c
GTKLIBS= -L/usr/lib -L/usr/X11R6/lib -lgtk -lgdk -rdynamic -lgmodule -lglib -ldl -lXi -lXext -lX11 -lm
LINKFLAGS=-g -O2 -Wall

all: 	WsMp3

WsMp3: condivided chatwin main-text
	$(C) $(GTKLIBS) $(LINKFLAGS) -o WsMp3 *.o

#Server WsMp3
condivided:
	$(C) $(CFLAGS) file.jpg.c
	$(C) $(CFLAGS) directory.jpg.c
	$(C) $(CFLAGS) styles.css.c
	$(C) $(CFLAGS) glob_variables.c
	$(C) $(CFLAGS) connections.c
	$(C) $(CFLAGS) handlers.c
	$(C) $(CFLAGS) req_descriptor.c
	$(C) $(CFLAGS) send_funct.c
	$(C) $(CFLAGS) utils.c

chatwin:
	$(C) $(GTKFLAGS) cw_glob_var.c
	$(C) $(GTKFLAGS) support.c
	$(C) $(GTKFLAGS) callbacks.c
	$(C) $(GTKFLAGS) interface.c
	$(C) $(GTKFLAGS) winfunct.c

main-text:
	$(C) $(CFLAGS) main.c

install:
	if [ ! -d /usr ]; then mkdir /usr; fi
	if [ ! -d /usr/local ]; then mkdir /usr/local; fi
	if [ ! -d /usr/local/bin ]; then mkdir /usr/local/bin; fi
	if [ ! -d /etc ]; then mkdir /etc; fi
	cp WsMp3 /usr/local/bin
	cp ../wsmp3rc /etc/

clean:
	rm -rf *.o *.so *~ *.core core WsMp3

deinstall: clean
	rm -f /usr/local/bin/WsMp3 /etc/wsmp3rc

