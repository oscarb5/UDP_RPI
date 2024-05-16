LIBS= -lpthread -lm #Libraries used if needed

SRCS= IOTClient.c accelerometer.c color_sensor.c

BIN=udpclient
CFLAGS+= -g -O
OBJS=$(subst .c,.o,$(SRCS))
all : $(BIN)
$(BIN): $(OBJS)
	@echo [link] $@
	$(CXX) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)
%.o: %.cpp
	@echo [Compile] $<
	$(CXX) -c $(CFLAGS) $< -o $@
	
clean:
	@rm -f $(OBJS) $(BIN)
