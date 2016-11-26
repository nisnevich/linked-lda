all:
	make -C src/ -f Makefile all

clean:
	make -C src/ -f Makefile clean

profile:
	make -C src/ -f Makefile profile	    
