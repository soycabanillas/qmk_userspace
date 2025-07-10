```
clear && cd /workspaces/qmk_userspace && qmk compile -kb crkbd -km soycabanillas --compiledb
clear && cd /workspaces/qmk_userspace && make -f Makefile.fixed build/tests/test_basic_tap_fixed
clear && cd /workspaces/qmk_userspace && make -f Makefile.fixed clean
clear && cd /workspaces/qmk_userspace && make -f Makefile.fixed comprehensive-fixed

make crkbd:soycabanillas 2>&1 | grep -E "(warning|deprecated|unknown|invalid).*enable" -i
make clean > /dev/null 2>&1 && make crkbd:soycabanillas 2>&1 | tail -5



# First time or after rules.mk changes
qmk compile -kb crkbd -km soycabanillas --compiledb

# Regular development iterations
make crkbd:soycabanillas
make crkbd:soycabanillas
make crkbd:soycabanillas

# After adding new source files or changing includes
qmk compile -kb crkbd -km soycabanillas --compiledb
```
