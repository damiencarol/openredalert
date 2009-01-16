#!/bin/bash -x

#
# Generated - do not edit!
#

# Macros
TOP=`pwd`
PLATFORM=MinGW-Windows
TMPDIR=build/Release/${PLATFORM}/tmp-packaging
TMPDIRNAME=tmp-packaging
OUTPUT_PATH=Release/openredalert
OUTPUT_BASENAME=openredalert
PACKAGE_TOP_DIR=openredalert/

# Functions
function checkReturnCode
{
    rc=$?
    if [ $rc != 0 ]
    then
        exit $rc
    fi
}
function makeDirectory
# $1 directory path
# $2 permission (optional)
{
    mkdir -p "$1"
    checkReturnCode
    if [ "$2" != "" ]
    then
      chmod $2 "$1"
      checkReturnCode
    fi
}
function copyFileToTmpDir
# $1 from-file path
# $2 to-file path
# $3 permission
{
    cp "$1" "$2"
    checkReturnCode
    if [ "$3" != "" ]
    then
        chmod $3 "$2"
        checkReturnCode
    fi
}

# Setup
cd "${TOP}"
mkdir -p dist/Release/${PLATFORM}/package
rm -rf ${TMPDIR}
mkdir -p ${TMPDIR}

# Copy files and create directories and links
cd "${TOP}"
makeDirectory ${TMPDIR}/openredalert/bin
copyFileToTmpDir "${OUTPUT_PATH}.exe" "${TMPDIR}/${PACKAGE_TOP_DIR}bin/${OUTPUT_BASENAME}.exe" 0755

cd "${TOP}"
makeDirectory ${TMPDIR}/openredalert/bin
copyFileToTmpDir "Release/SDL.dll" "${TMPDIR}/${PACKAGE_TOP_DIR}bin/SDL.dll" 0644

cd "${TOP}"
makeDirectory ${TMPDIR}/openredalert/bin
copyFileToTmpDir "Release/SDL_mixer.dll" "${TMPDIR}/${PACKAGE_TOP_DIR}bin/SDL_mixer.dll" 0644

cd "${TOP}"
makeDirectory ${TMPDIR}/openredalert
copyFileToTmpDir "AUTHORS" "${TMPDIR}/${PACKAGE_TOP_DIR}AUTHORS" 0644

cd "${TOP}"
makeDirectory ${TMPDIR}/openredalert
copyFileToTmpDir "COPYING" "${TMPDIR}/${PACKAGE_TOP_DIR}COPYING" 0644

cd "${TOP}"
makeDirectory ${TMPDIR}/openredalert
copyFileToTmpDir "README" "${TMPDIR}/${PACKAGE_TOP_DIR}README" 0644

cd "${TOP}"
makeDirectory ${TMPDIR}/openredalert/bin/data/settings
copyFileToTmpDir "Debug/data/settings/ABANDON1.INI" "${TMPDIR}/${PACKAGE_TOP_DIR}bin/data/settings/ABANDON1.INI" 0644

cd "${TOP}"
makeDirectory ${TMPDIR}/openredalert/bin/data/settings
copyFileToTmpDir "Debug/data/settings/files.ini" "${TMPDIR}/${PACKAGE_TOP_DIR}bin/data/settings/files.ini" 0644

cd "${TOP}"
makeDirectory ${TMPDIR}/openredalert/bin/data/settings
copyFileToTmpDir "Debug/data/settings/freecnc.ini" "${TMPDIR}/${PACKAGE_TOP_DIR}bin/data/settings/freecnc.ini" 0644

cd "${TOP}"
makeDirectory ${TMPDIR}/openredalert/bin/data/settings
copyFileToTmpDir "Debug/data/settings/internal-global.ini" "${TMPDIR}/${PACKAGE_TOP_DIR}bin/data/settings/internal-global.ini" 0644

cd "${TOP}"
makeDirectory ${TMPDIR}/openredalert/bin/data/settings/ra
copyFileToTmpDir "Debug/data/settings/ra/art.ini" "${TMPDIR}/${PACKAGE_TOP_DIR}bin/data/settings/ra/art.ini" 0644

cd "${TOP}"
makeDirectory ${TMPDIR}/openredalert/bin/data/settings/ra
copyFileToTmpDir "Debug/data/settings/ra/cursors.ini" "${TMPDIR}/${PACKAGE_TOP_DIR}bin/data/settings/ra/cursors.ini" 0644

cd "${TOP}"
makeDirectory ${TMPDIR}/openredalert/bin/data/settings/ra
copyFileToTmpDir "Debug/data/settings/ra/internal.ini" "${TMPDIR}/${PACKAGE_TOP_DIR}bin/data/settings/ra/internal.ini" 0644

cd "${TOP}"
makeDirectory ${TMPDIR}/openredalert/bin/data/settings/ra
copyFileToTmpDir "Debug/data/settings/ra/protofiles.ini" "${TMPDIR}/${PACKAGE_TOP_DIR}bin/data/settings/ra/protofiles.ini" 0644

cd "${TOP}"
makeDirectory ${TMPDIR}/openredalert/bin/data/settings/ra
copyFileToTmpDir "Debug/data/settings/ra/sidebar.ini" "${TMPDIR}/${PACKAGE_TOP_DIR}bin/data/settings/ra/sidebar.ini" 0644

cd "${TOP}"
makeDirectory ${TMPDIR}/openredalert/bin/data/settings/ra
copyFileToTmpDir "Debug/data/settings/ra/sound.ini" "${TMPDIR}/${PACKAGE_TOP_DIR}bin/data/settings/ra/sound.ini" 0644

cd "${TOP}"
makeDirectory ${TMPDIR}/openredalert/bin/data/settings/ra
copyFileToTmpDir "Debug/data/settings/ra/structure.ini" "${TMPDIR}/${PACKAGE_TOP_DIR}bin/data/settings/ra/structure.ini" 0644

cd "${TOP}"
makeDirectory ${TMPDIR}/openredalert/bin/data/settings/ra
copyFileToTmpDir "Debug/data/settings/ra/talkback.ini" "${TMPDIR}/${PACKAGE_TOP_DIR}bin/data/settings/ra/talkback.ini" 0644

cd "${TOP}"
makeDirectory ${TMPDIR}/openredalert/bin/data/settings/ra
copyFileToTmpDir "Debug/data/settings/ra/templates.ini" "${TMPDIR}/${PACKAGE_TOP_DIR}bin/data/settings/ra/templates.ini" 0644

cd "${TOP}"
makeDirectory ${TMPDIR}/openredalert/bin/data/settings/ra
copyFileToTmpDir "Debug/data/settings/ra/unit.ini" "${TMPDIR}/${PACKAGE_TOP_DIR}bin/data/settings/ra/unit.ini" 0644

cd "${TOP}"
makeDirectory ${TMPDIR}/openredalert/bin/data/settings/ra
copyFileToTmpDir "Debug/data/settings/ra/weapons.ini" "${TMPDIR}/${PACKAGE_TOP_DIR}bin/data/settings/ra/weapons.ini" 0644

cd "${TOP}"
makeDirectory ${TMPDIR}/openredalert/bin/data/settings
copyFileToTmpDir "Debug/data/settings/songs.ini" "${TMPDIR}/${PACKAGE_TOP_DIR}bin/data/settings/songs.ini" 0644

cd "${TOP}"
makeDirectory ${TMPDIR}/openredalert/bin/data/maps
copyFileToTmpDir "Debug/data/maps/MAP1.MPR" "${TMPDIR}/${PACKAGE_TOP_DIR}bin/data/maps/MAP1.MPR" 0644

cd "${TOP}"
makeDirectory ${TMPDIR}/openredalert/bin/data/maps
copyFileToTmpDir "Debug/data/maps/MAP2.MPR" "${TMPDIR}/${PACKAGE_TOP_DIR}bin/data/maps/MAP2.MPR" 0644

cd "${TOP}"
makeDirectory ${TMPDIR}/openredalert/bin/data/maps
copyFileToTmpDir "Debug/data/maps/MAP3.MPR" "${TMPDIR}/${PACKAGE_TOP_DIR}bin/data/maps/MAP3.MPR" 0644


# Generate tar file
cd "${TOP}"
rm -f dist/Release/${PLATFORM}/package/openredalert439.tar
cd ${TMPDIR}
tar -vcf ../../../../dist/Release/${PLATFORM}/package/openredalert439.tar *
checkReturnCode

# Cleanup
cd "${TOP}"
rm -rf ${TMPDIR}
