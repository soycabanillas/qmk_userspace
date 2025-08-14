```
clear && cd /workspaces/qmk_userspace && qmk compile -kb crkbd -km soycabanillas --compiledb

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
