
all:
	(cd lib/recipes; make)
	(cd lib/linpack; make)
	(cd C; make Filtermesh Recon -k)
clean:
	(cd lib/recipes; make clean)
	(cd lib/linpack; make clean)
	(cd C; make clean)
