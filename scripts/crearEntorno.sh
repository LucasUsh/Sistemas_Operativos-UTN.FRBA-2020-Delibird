DIRECTORIO=/home/utnso/Escritorio/tp
PROYECTO=/home/utnso/workspace/tp-2020-1c-5rona

if [ -d "${DIRECTORIO}" ]; then
	echo "existe directorio"
else
	mkdir ${DIRECTORIO}
fi

cd ${PROYECTO}/Broker/Debug/
make clean
make all
cp Broker ${DIRECTORIO}

cd ${PROYECTO}/Game-Card/Debug/
make clean
make all
cp Game-Card ${DIRECTORIO}



cd ${PROYECTO}/Game-Boy/Debug/
make clean
make all
cp gameboy ${DIRECTORIO}

cd ${PROYECTO}/Game-Boy/Debug/
cp buddy_basico.sh ${DIRECTORIO}
cp new_pikachu.sh ${DIRECTORIO}
cp new_pokemon_varios.sh ${DIRECTORIO}
cp new_pokemon_antes_team.sh ${DIRECTORIO}
cp new_pokemon_post_team.sh ${DIRECTORIO}

clear
cd ${DIRECTORIO}
ls -l

