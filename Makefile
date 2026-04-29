# Usage:
#   make translate ARGS=path/to/script.m       # transpile + compile
#   make run       ARGS=path/to/script.m       # transpile + compile + spike
#   make compare   ARGS=path/to/script.m       # transpile + compile + spike + octave diff
#   make matrix                                # rebuild RISCV-Matrix/src/libmatrix.a
#   make clean                                 # remove top-level build artefacts

REPO := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
TRANSLATE := $(REPO)/bin/translate

ifndef ARGS
ARGS_ERR := you must pass ARGS=<path-to-.m>
endif

.PHONY: translate run compare matrix clean help

help:
	@sed -n '1,/^$$/p' $(MAKEFILE_LIST) | sed 's/^# \?//'

translate:
ifdef ARGS_ERR
	@echo "$(ARGS_ERR)"; exit 2
endif
	@$(TRANSLATE) $(ARGS)

run:
ifdef ARGS_ERR
	@echo "$(ARGS_ERR)"; exit 2
endif
	@$(TRANSLATE) --run $(ARGS)

compare:
ifdef ARGS_ERR
	@echo "$(ARGS_ERR)"; exit 2
endif
	@$(TRANSLATE) --compare $(ARGS)

matrix:
	@$(MAKE) -C $(REPO)/RISCV-Matrix/src

clean:
	@rm -f $(REPO)/RISCV-Matrix/src/*.o $(REPO)/RISCV-Matrix/src/libmatrix.a
	@rm -f $(REPO)/ts-traversal/generatedCode/{matrix,convertSubscript,unknownTypes,wrapper}.o
	@find $(REPO) -maxdepth 2 -name '*.riscv' -type f -delete 2>/dev/null || true
	@echo "cleaned"
