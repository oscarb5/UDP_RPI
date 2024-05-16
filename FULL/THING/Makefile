LIBS= -lpthread -lm -lmosquitto # Additional Libraries used if needed

SRCS= IOTClient.c accelerometer.c color_sensor.c
BIN= thingclient

CFLAGS+= -g -O
OBJS= $(subst .c,.o,$(SRCS))

all: $(BIN)

$(BIN): $(OBJS)
	@echo [link] $@
	$(CC) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)

%.o: %.c
	@echo [Compile] $<
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	@rm -f $(OBJS) $(BIN)
