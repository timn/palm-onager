#!/bin/sh

. *.mk

echo "Packing source for $PROGNAME"

cd ..
tar --exclude=CVS -cvzf $PROGNAME-source-$VERSION.tar.gz $MODNAME
zip -r $PROGNAME-source-$VERSION.zip $MODNAME -x \*CVS\*
mkdir $MODNAME/$PROGNAME-${VERSION}_dist
mv $PROGNAME-source-$VERSION.tar.gz $MODNAME/$PROGNAME-${VERSION}_dist
mv $PROGNAME-source-$VERSION.zip $MODNAME/$PROGNAME-${VERSION}_dist
cd $MODNAME
