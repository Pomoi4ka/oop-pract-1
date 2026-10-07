CFLAGS = -Wall -Wextra -pedantic -std=c89 -pipe -g -MMD -MP
LIBS   = -lreadline

SRCS := $(shell find src/ -type f -name '*.c')
OBJS := $(SRCS:.c=.o)
DEPS := $(OBJS:.o=.d)

SRCS_WITH_HEADERS := $(shell find src/ -type f -name '*.h') $(SRCS)

.PHONY: all pdf

all: main

main: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

pdf: assets/source.tex
	pdflatex report.tex
	pdflatex report.tex

assets/source.tex: assets $(SRCS_WITH_HEADERS)
	($(foreach src,$(SRCS_WITH_HEADERS), \
printf "\n\\\texttt{$(src):}\n\n\\\begin{lstlisting}[language=C]\n" | sed 's|_|\\_|g'; \
cat $(src); \
printf "\\\end{lstlisting}\n";)) > $@

assets:
	mkdir -p $@

-include $(DEPS)
