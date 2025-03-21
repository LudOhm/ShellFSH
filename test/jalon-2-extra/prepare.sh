#!/bin/bash

cat > append <<"EOF"
#!/bin/bash
printf "%s\n" "$2" >> "$1"
EOF
chmod u+x append

cat > trunc <<"EOF"
#!/bin/bash
printf "" > "$1"
EOF
chmod u+x trunc

B=dir-1
mkdir -p $B
for f in guilt heart bay spy mouse kilt spray house tray trick brick steam tilt pay hilt hay proud
do printf "%s\n" $(basename $f) | rev > $B/$f
done

B=dir-2
mkdir -p $B/heart/.bay/spy/guilt
mkdir -p $B/heart/.bay/spy/mouse
mkdir -p $B/heart/.bay/spray/house
mkdir -p $B/heart/.bay/spray/tilt
mkdir -p $B/heart/pay/tray/hilt
mkdir -p $B/heart/pay/trick/stroll
mkdir -p $B/heart/pay/trick/team
mkdir -p $B/heart/pay/brick/troll
mkdir -p $B/heart/pay/brick/steam
mkdir -p $B/.choice/gem/check/theft
mkdir -p $B/.choice/gem/check/hay
mkdir -p $B/.choice/gem/check/pull
mkdir -p $B/.choice/gem/check/mass
mkdir -p $B/.choice/snail/.deck/rob
mkdir -p $B/.choice/snail/.deck/proud
mkdir -p $B/.choice/snail/.deck/node
mkdir -p $B/.choice/snail/.deck/say
touch $B/heart/.bay/spy/guilt/spread.txt
touch $B/heart/.bay/spy/mouse/pig.c
touch $B/heart/.bay/spray/house/frown.toc
touch $B/heart/.bay/spray/tilt/ear.png
touch $B/.choice/gem/check/hay/heal.txt
touch $B/.choice/snail/.deck/node/.nut.c
touch $B/heart/pay/trick/team/snow
touch $B/heart/pay/trick/stroll/crop.png
touch $B/heart/pay/trick/team/.bow

printf "bcdefgh\n" | rev > fichier-1 

printf '%s\n' 'N;s/\(.*\)\n/\1 /' > commande-sed

