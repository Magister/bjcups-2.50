dirs = libs bjcups bjcupsmon ppd pstocanonbj 

scripts=for dir in $(dirs); do\
			(cd $$dir; make $$target)|| exit 1;\
		done

all :
	$(scripts)

clean :
	target=clean; $(scripts)

install :
	target=install; $(scripts)

