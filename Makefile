
CFLAGS = -std=c11 -pedantic -Wall -O0 -g

CLR = "\033[0m"
RED = "\033[0;31m"
GRN = "\033[0;32m"


.PHONY: all clean

all: $(patsubst %.inc.c,%.chk,$(wildcard test/*/*.inc.c))

test/%.chk: test/%.out
	@echo $(GRN)PASS $(*F)$(CLR)

test/good/%.out: test/good/%.test
	$< >$@
	cat $@
	jq -M . $@ >/dev/null

test/bad/%.out: test/bad/%.test
	! $< >$@

test/%.test: test/%.test.o jsonwrite.o
	gcc -o $@ $^

test/%.test.o: test/%.inc.c
	@sed -e "/__TESTFILE__/r $<" \
	    -e "/__TESTFILE__/d" test/main.c |\
	    gcc -c -o $@ -I.. -xc -
clean:
	rm -fr test/*/*.test test/*/*.out test/*/*.chk
