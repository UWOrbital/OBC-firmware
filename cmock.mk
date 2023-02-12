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
PATHINC += -I"tests/mocks/"
PATHINC += -I"examples/unit_test_demo/"
PATHINC += -I"cmock/src/"
PATHINC += -I"unity/src/"

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

# SOURCES :=
# SOURCES += main.c
# SOURCES += adcs/source/adcs_manager.c
# SOURCES += cdh/source/supervisor.c
# SOURCES += cdh/source/telemetry.c
# SOURCES += common/source/obc_logging.c
# SOURCES += comms/source/comms_manager.c
# SOURCES += drivers/source/obc_i2c_io.c
# SOURCES += drivers/source/obc_sci_io.c
# SOURCES += drivers/source/obc_spi_io.c
# SOURCES += eps/source/eps_manager.c
# SOURCES += hal/source/dabort.s
# SOURCES += hal/source/errata_SSWF021_45.c
# SOURCES += hal/source/esm.c
# SOURCES += hal/source/gio.c
# SOURCES += hal/source/i2c.c
# SOURCES += hal/source/notification.c
# SOURCES += hal/source/os_croutine.c
# SOURCES += hal/source/os_event_groups.c
# SOURCES += hal/source/os_heap.c
# SOURCES += hal/source/os_list.c
# SOURCES += hal/source/os_mpu_wrappers.c
# SOURCES += hal/source/os_port.c
# SOURCES += hal/source/os_portasm.s
# SOURCES += hal/source/os_queue.c
# SOURCES += hal/source/os_required_fns.c
# SOURCES += hal/source/os_tasks.c
# SOURCES += hal/source/os_timer.c
# SOURCES += hal/source/pinmux.c
# SOURCES += hal/source/sci.c
# SOURCES += hal/source/spi.c
# SOURCES += hal/source/sys_calls.c
# SOURCES += hal/source/sys_core.s
# SOURCES += hal/source/sys_dma.c
# SOURCES += hal/source/sys_intvecs.s
# SOURCES += hal/source/sys_mpu.s
# SOURCES += hal/source/sys_pcr.c
# SOURCES += hal/source/sys_phantom.c
# SOURCES += hal/source/sys_pmm.c
# SOURCES += hal/source/sys_pmu.s
# SOURCES += hal/source/sys_selftest.c
# SOURCES += hal/source/sys_startup.c
# SOURCES += hal/source/sys_vim.c
# SOURCES += hal/source/system.c
# SOURCES += payload/source/payload_manager.c

# SOURCES += tests/mocks/mock_rectangle.c
# SOURCES += examples/unit_test_demo/shape_container.c

PATHT := tests/
PATHB := build/
PATHD := build/depends/
PATHO := build/objs/
PATHR := build/results/

BUILD_PATHS = $(PATHB) $(PATHD) $(PATHO) $(PATHR)

SRCT = $(wildcard $(PATHT)*.c)

# SRCS := $(foreach dir,$(PATHS),$(wildcard $(dir)/*.c))
# SRCS += $(foreach dir,$(PATHS),$(wildcard $(dir)/*.s))
# OBJS := $(foreach file,$(SRCS),$(BUILD_DIR)/$(basename $(file)).o)
# OBJ_DIRS := $(sort $(foreach obj,$(OBJS),$(dir $(obj))))

# $(foreach dir,$(OBJ_DIRS), $(shell mkdir -p $(dir)))

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

$(PATHB)test_%.$(TARGET_EXTENSION): $(PATHO)test_%.o $(PATHO)%.o $(PATHO)unity.o $(PATHO)mock_rectangle.o $(PATHO)cmock.o
	$(LINK) -o $@ $^

$(PATHO)%.o: $(PATHT)%.c
	$(COMPILE) $(CFLAGS) $< -o $@

$(PATHO)%.o: $(PATHU)%.c $(PATHU)%.h
	$(COMPILE) $(CFLAGS) $< -o $@

$(PATHO)shape_container.o: examples/unit_test_demo/shape_container.c
	$(COMPILE) $(CFLAGS) $< -o $@

$(PATHO)mock_rectangle.o: tests/mocks/mock_rectangle.c
	$(COMPILE) $(CFLAGS) $< -o $@

$(PATHO)cmock.o: cmock/src/cmock.c
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
