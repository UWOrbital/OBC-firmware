ifeq ($(OS),Windows_NT)
	TARGET_EXTENSION=exe
else
	TARGET_EXTENSION=out
endif

.PHONY: clean
.PHONY: test

PATHU := unity/src/

PATHINC :=
PATHINC += -I"common/include/"
PATHINC += -I"drivers/include/"
PATHINC += -I"hal/include/"
PATHINC += -I"adcs/include/"
PATHINC += -I"cdh/include/"
PATHINC += -I"comms/include/"
PATHINC += -I"eps/include/"
PATHINC += -I"payload/include/"
PATHINC += -I"examples/unit_test_demo/"
PATHINC += -I"tests/mocks/"

PATHS := 
PATHS += common/source/
PATHS += drivers/source/
PATHS += hal/source/
PATHS += adcs/source/
PATHS += cdh/source/
PATHS += comms/source/
PATHS += eps/source/
PATHS += payload/source/
PATHS += tests/mocks/
PATHS += examples/unit_test_demo/

PATHT := tests/
PATHB := build/
PATHD := build/depends/
PATHO := build/objs/
PATHR := build/results/

BUILD_PATHS = $(PATHB) $(PATHD) $(PATHO) $(PATHR)

SRCT = $(wildcard $(PATHT)*.c)

COMPILE=gcc -c
LINK=gcc
DEPEND=gcc -MM -MG -MF
CFLAGS=-I. -I"$(PATHU)" $(PATHINC) -DTEST

RESULTS = $(patsubst $(PATHT)test_%.c,$(PATHR)test_%.txt,$(SRCT) )

PASSED = `grep -s PASS $(PATHR)*.txt`
FAIL = `grep -s FAIL $(PATHR)*.txt`
IGNORE = `grep -s IGNORE $(PATHR)*.txt`

test: $(BUILD_PATHS) $(RESULTS)
	@echo "-----------------------\nIGNORES:\n-----------------------"
	@echo "$(IGNORE)"
	@echo "-----------------------\nFAILURES:\n-----------------------"
	@echo "$(FAIL)"
	@echo "-----------------------\nPASSED:\n-----------------------"
	@echo "$(PASSED)"
	@echo "\nDONE"

$(PATHR)%.txt: $(PATHB)%.$(TARGET_EXTENSION)
	-./$< > $@ 2>&1

$(PATHB)test_%.$(TARGET_EXTENSION): $(PATHO)test_%.o $(PATHO)%.o $(PATHO)unity.o
	$(LINK) -o $@ $^

$(PATHO)%.o: $(PATHT)%.c
	$(COMPILE) $(CFLAGS) $< -o $@

$(PATHO)%.o: $(PATHU)%.c $(PATHU)%.h
	$(COMPILE) $(CFLAGS) $< -o $@

$(PATHO)%.o: $(PATHS)%.c
	$(COMPILE) $(CFLAGS) $< -o $@

$(PATHD)%.d: $(PATHT)%.c
	$(DEPEND) $@ $<

$(PATHB):
	mkdir -p $(PATHB)

$(PATHD):
	mkdir -p $(PATHD)

$(PATHO):
	mkdir -p $(PATHO)

$(PATHR):
	mkdir -p $(PATHR)

clean:
	rm -f $(PATHO)*.o
	rm -f $(PATHB)*.$(TARGET_EXTENSION)
	rm -f $(PATHR)*.txt

.PRECIOUS: $(PATHB)test_%.$(TARGET_EXTENSION)
.PRECIOUS: $(PATHD)%.d
.PRECIOUS: $(PATHO)%.o
.PRECIOUS: $(PATHR)%.txt
