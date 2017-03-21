BUNDLE = punkconsole.lv2
INSTALL_DIR = /usr/local/lib/lv2

$(BUNDLE): manifest.ttl punkconsole.ttl punkconsole.so
	rm -rf $(BUNDLE)
	mkdir $(BUNDLE)
	cp $^ $(BUNDLE)

all: $(BUNDLE)

punkconsole.so: punkconsole.c
	gcc $< -o $@ -shared -fPIC -DPIC

clean:
	rm -rf $(BUNDLE) *.so

install: $(BUNDLE)
	mkdir -p $(INSTALL_DIR)
	rm -rf $(INSTALL_DIR)/$(BUNDLE)
	cp -R $(BUNDLE) $(INSTALL_DIR)

uninstall:
	rm -rf $(INSTALL_DIR)/$(BUNDLE)
