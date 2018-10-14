CC=gcc
CFLAGS=-Wall -g

default: obj/ammd obj/winclass
clean:
	rm -rf obj

$(HOME)/.appmodmaps/.created:
	mkdir $(HOME)/.appmodmaps || echo "Never mind."
	cp bitcmds/* $(HOME)/.appmodmaps
	touch $(HOME)/.appmodmaps/.created

$(HOME)/.appmodmaps/config.h: $(HOME)/.appmodmaps/.created
	cp config.h $(HOME)/.appmodmaps/config.h

obj/ammd: $(HOME)/.appmodmaps/config.h ammd.c
	mkdir obj || echo "Never mind."
	$(CC) -O3 $(CFLAGS) "-DUSER_CONFIG=\"$(HOME)/.appmodmaps/config.h\"" -o obj/ammd ammd.c -lX11

obj/winclass: obj/ammd ammd.c
	$(CC) -DDEBUG $(CFLAGS) -o obj/winclass ammd.c -lX11

