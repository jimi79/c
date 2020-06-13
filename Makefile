BIN=./bin
SOURCE=./

LIST=$(BIN)/cblk-regroup $(BIN)/cblk-split $(BIN)/short-uptime

all: $(LIST)

$(BIN)/%: $(SOURCE)%.c
	$(CC) $(INC) $< $(CFLAGS) -o $@ $(LIBS)

