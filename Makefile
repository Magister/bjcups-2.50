dirs = libs bjcups bjcupsmon ppd pstocanonbj 

autogen=for dir in $(dirs); do\
			(cd $$dir; ./autogen.sh|| exit 1;\
		done

scripts=for dir in $(dirs); do\
			(cd $$dir ; make $$target)|| exit 1;\
		done

all :
	$(scripts)

clean :
	target=clean; $(scripts)

distclean :
	target=distclean; $(scripts)

install :
	target=install; $(scripts)

