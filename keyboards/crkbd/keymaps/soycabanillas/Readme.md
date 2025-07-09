```
clear && cd /workspaces/qmk_userspace && qmk compile -kb crkbd -km soycabanillas --compiledb
clear && cd /workspaces/qmk_userspace && make -f Makefile.fixed build/tests/test_basic_tap_fixed
clear && cd /workspaces/qmk_userspace && make -f Makefile.fixed clean
clear && cd /workspaces/qmk_userspace && make -f Makefile.fixed comprehensive-fixed
```
