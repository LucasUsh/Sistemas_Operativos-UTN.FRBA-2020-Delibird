MONTAJE=/home/utnso/Escritorio/TallGrass
FILES=/home/utnso/Escritorio/TallGrass/Files

if [ -d "$MONTAJE" ]; then
	for Pokemon in "$FILES"/*
	do
		if [ -d "${Pokemon}" ]; then
			echo "-----${Pokemon}:"
			cat "${Pokemon}/Metadata.bin"
			echo -ne "\n"
		fi
	done
else
	echo "no existe $MONTAJE"
fi