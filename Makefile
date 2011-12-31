all:		iuspack iusunpack

clean:
		rm iuspack iusunpack

iuspack:	ius.h iuspack.c
		$(CC) iuspack.c -oiuspack -lz

iusunpack:	ius.h iusunpack.c
		$(CC) iusunpack.c -oiusunpack
