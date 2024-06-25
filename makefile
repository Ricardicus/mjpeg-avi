CC := gcc

SRCS := src/avi.c src/main.c
OBJS := $(SRCS:.c=.o)
RULES := $(OBJS:.o=.d)

NO_WARNINGS := multichar
FLAGS := $(addprefix -Wno-, $(NO_WARNINGS))

OUTDIR := bin

all: avimake

-include $(RULES)

%.d: %.c
	@$(CC) -M -Isrc $< > $@

%.o: %.c
	$(CC) -c -o $@ $<  $(FLAGS)

avimake: $(OBJS)
	@mkdir -p $(OUTDIR)
	$(CC) -o $(OUTDIR)/$@ $^ $(FLAGS)

clean:
	rm -rf src/*.o src/*.d
