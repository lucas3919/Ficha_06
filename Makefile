binary: main.c // muda o nome aqui oh <-
    gcc $< -pthread -o $@

.PHONY: run clean

run:
    ./binary

clean:
    rm binary