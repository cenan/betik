CC = gcc
CFLAGS = -c -Wall -std=c99 -Isrc
BUILDDIR = build
SOURCEDIR = src
DISTDIR = dist
ifeq ($(OS),Windows_NT)
	TARGET = $(DISTDIR)/betik.exe
else
	TARGET = $(DISTDIR)/betik
endif
SRCS = $(wildcard src/*.c)
OBJS = $(addprefix $(BUILDDIR)/,$(notdir $(patsubst %.c,%.o,$(wildcard src/*.c))))
DEPS = $(addprefix $(BUILDDIR)/,$(notdir $(patsubst %.c,%.d,$(wildcard src/*.c))))

all: $(TARGET)

$(OBJS): $(BUILDDIR)/%.o: $(SOURCEDIR)/%.c
	@$(CC) $(CFLAGS) $< -o $@
	@echo [CC ] $<
	@$(CC) -MM -MT $@ $< > $(BUILDDIR)/$*.d
	@echo [DEP] $<

$(TARGET): $(OBJS)
	@$(CC) $(OBJS) -m64 -o $(TARGET)
	@echo [LNK] $(TARGET)

%o: %c
	$(CC) $(CFLAGS) $< -o $@

-include $(DEPS)

clean:
	@rm -f $(OBJS)
	@echo [RM ] $(OBJS)
	@rm -f $(BUILDDIR)/resource.o
	@echo [RM ] $(BUILDDIR)/resource.o
	@rm -f $(DEPS)
	@echo [RM ] $(DEPS)
	@rm -f $(DISTDIR)/*
	@echo [RM ] $(TARGET)
