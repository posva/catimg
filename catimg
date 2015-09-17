#!/usr/bin/env bash

################################################################################
# catimg script by Eduardo San Martin Morote aka Posva                         #
# http://posva.net                                                             #
#                                                                              #
# Ouput the content of an image to the stdout using the 256 colors of the      #
# terminal.                                                                    #
# Github: https://github.com/posva/catimg                                      #
################################################################################

help() {
  echo "Usage catimg [-h] [-w width] [-c char] img"
  echo "By default char is \"  \" and w is the terminal width"
}

# VARIABLES
COLOR_FILE=$(dirname "$0")/catimg-ext-colors.png
CHAR="  "

WIDTH=""
IMG=""
LOOP=""

while getopts w:c:l:h opt; do
  case "$opt" in
    w) WIDTH="$OPTARG" ;;
    c) CHAR="$OPTARG" ;;
    h) help; exit ;;
    l) LOOP="$OPTARG" ;;
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

if ! convert "$IMG" -resize "${COLS}>" +dither -remap "${COLOR_FILE}" /dev/null ; then
  echo "The version of convert is too old, don't expect good results :(" >&2
fi

# produce terminal color from rgb value
# term_color r g b
term_color() {
  local r g b
  r="$1"
  g="$2"
  b="$3"
  if [ "$r" -eq "$g" -a "$g" -eq "$b" ]; then
    ((
    IDX = 232 + r * 23 / 255
    ))
  else
    # the * 36 at the end is intended, it gives better results
    ((
    IDX = 16
    + r * 5 / 255 * 36
    + g * 5 / 255 * 6
    + b * 5 / 255
    ))
  fi
}

# Display the image
L=0
if [ ! "$LOOP" ]; then
  LOOP=1
fi
IMAGE="" # Stored image, used when looping

I=0
while [[ "$L" -lt "${LOOP}" || "${LOOP}" = 0 ]]; do
  IND=1 # for array storing
  convert "$IMG" -resize $COLS\> +dither -remap "${COLOR_FILE}" txt:- |
  sed -e 's/.*none.*/NO NO NO/g' -e '1d;s/^.*(\(.*\)[,)].*$/\1/g;y/,/ /' |
  while read R G B f; do
    (( I++ ))
    if [ "$R" = "#" ]; then
      echo -en "\033[${I}A" # Move up the cursor
      I=0
    elif [ ! "$R" = "NO" ]; then
      if [ "$LOOP" ]; then
        if [ ! "${PIXEL[IND]}" ]; then
          term_color "$R" "$G" "$B"
          #echo "$R,$G,$B: $IDX"
          PIXEL[IND]="[48;5;${IDX}m${CHAR}"
        fi
        echo -n "${PIXEL[IND]}"
        IMAGE="${IMAGE}${PIXEL[IND]}"
        (( IND++ ))
        #echo "IND = ${IND}"
      else
        term_color "$R" "$G" "$B"
        #echo "$R,$G,$B: $IDX"
        echo -n "[48;5;${IDX}m${CHAR}"
        IMAGE="${IMAGE}[48;5;${IDX}m${CHAR}"
      fi
    else
      echo -n "[m${CHAR}"
      IMAGE="${IMAGE}[m${CHAR}"
    fi
    # New lines
    (( K = I % WIDTH ))
    if [[ "$K" = 0 ]]; then
      echo "[m"
      IMAGE="${IMAGE}[m
      "
    fi
  done
  echo -en "\033[${I}A" # Move up the cursor
  ((L++))
done
