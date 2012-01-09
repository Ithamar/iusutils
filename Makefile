all:		iuspack iusunpack mkbootsd

clean:
		rm iuspack iusunpack mkbootsd

iuspack:	ius.h iuspack.c
		$(CC) iuspack.c -oiuspack -lz

iusunpack:	ius.h iusunpack.c
		$(CC) iusunpack.c -oiusunpack

mkbootsd:	sdheader.h mkbootsd.c
		$(CC) mkbootsd.c -omkbootsd
