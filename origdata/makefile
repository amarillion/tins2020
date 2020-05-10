# Set shell to bash instead of sh allow for brace expansion
SHELL=/bin/bash

DEST=../data
SRC=.

.PHONY: default
default: all ;

SPRITES=BellBlue BellOrange BugBlue BugGreen BugRed bullets Cover FlowerBlue FlowerPink FoleFront1 pickups teleport tiles2
SHEET_FILES=$(patsubst %,$(DEST)/%.png,$(SPRITES))
	
$(SHEET_FILES): $(DEST)/%.png: $(SRC)/%.aseprite
	aseprite -b $< --sheet $@

.PHONY: all
all: $(SHEET_FILES)
