#! /bin/bash

################################################################################
# catimg script by Eduardo San Martin Morote aka Posva                         #
# http://posva.net                                                             #
#                                                                              #
# Ouput the content of an image to the stdout using the 256 colors of the      #
# terminal.                                                                    #
# Github: https://github.com/posva/catimg                                      #
################################################################################

function help() {
  echo "Usage catimg [-h] [-w width] [-c char] img"
  echo "By default char is \"  \" and w is the terminal width"
}

# VARIABLES
COLOR_FILE=$(dirname "$0")/colors.png
CHAR="  "

WIDTH=""
IMG=""

while getopts w:c:h opt; do
  case "$opt" in
    w) WIDTH="$OPTARG" ;;
    c) CHAR="$OPTARG" ;;
    h) help; exit ;;
    *) help ; exit 1;;
    esac
  done

while [ "$1" ]; do
  IMG="$1"
  shift
done

if [ "$IMG" = "" -o ! -f "$IMG" ]; then
  help
  exit 1
fi

if [ ! "$WIDTH" ]; then
  (( COLS = $(tput cols) / $(echo -n "$CHAR" | wc -c) ))
else
  (( COLS = WIDTH / $(echo -n "$CHAR" | wc -c) ))
fi
WIDTH=$(convert "$IMG" -print "%w\n" /dev/null)
if [ "$WIDTH" -gt "$COLS" ]; then
  WIDTH=$COLS
fi

REMAP=""
if convert "$IMG" -resize "${COLS}>" +dither -remap "${COLOR_FILE}" /dev/null ; then
  REMAP="-remap ${COLOR_FILE}"
else
  echo "The version of convert is too old, don't expect good results :(" >&2
fi

# Display the image
I=0
convert "$IMG" -resize $COLS\> +dither `echo $REMAP` txt:- |
sed -e 's/.*none.*/NO NO NO/g' -e '1d;s/^.*(\(.*\)[,)].*$/\1/g;y/,/ /' |
while read R G B f; do
  if [ "$R" = "#" ]; then
    (( I++ ))
    echo -en "\033[${I}A" # Move up the cursor
    I=0
  elif [ ! "$R" = "NO" ]; then
    if [ "$R" -eq "$G" -a "$G" -eq "$B" ]; then
      ((
      I++,
      IDX = 232 + R * 23 / 255
      ))
    else
      # the * 36 at the end is intended, it gives better results
      ((
      I++,
      IDX = 16
      + R * 5 / 255 * 36
      + G * 5 / 255 * 6
      + B * 5 / 255
      ))
    fi
    #echo "$R,$G,$B: $IDX"
    echo -n "[48;5;${IDX}m${CHAR}"
  else
    (( I++ ))
    echo -n "[m${CHAR}"
  fi
  # New lines
  (( I % WIDTH )) || echo "[m"
done
