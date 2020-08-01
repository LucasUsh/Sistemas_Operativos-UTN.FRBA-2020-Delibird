MONTAJE=/home/utnso/Escritorio/TallGrass

if [ -d "$MONTAJE" ]; then
	echo "pokemones"
	ls /home/utnso/Escritorio/TallGrass/Files
else
	echo "no existe $MONTAJE"
fi