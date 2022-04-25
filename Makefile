BIN=./bin
SOURCE=./src/

LIST=cblk-regroup cblk-split 

all: $(LIST)

clean:
	rm -rf cblk-split cblk-regroup

%: $(SOURCE)%.c
	$(CC) $(INC) $< $(CFLAGS) -o $@ $(LIBS)

