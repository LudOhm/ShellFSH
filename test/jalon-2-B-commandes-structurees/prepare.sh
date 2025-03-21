#!/bin/bash

cat > append <<"EOF"
#!/bin/bash
printf "%s\n" "$2" >> "$1"
EOF
chmod u+x append

cat > append-list <<"EOF"
#!/bin/bash
printf "%s " "$2" >> "$1"
EOF
chmod u+x append-list

cat > trunc <<"EOF"
#!/bin/bash
printf "" > "$1"
EOF
chmod u+x trunc

cat > ret <<"EOF"
#!/bin/bash
exit $(basename $1)
EOF
chmod u+x ret

touch NOTADIR

B=dir-1
mkdir -p $B
for d in guilt mouse house tilt hilt
do mkdir -p $B/$d
done
for f in guilt/{heart,bay,spy} mouse/{kilt,spray} house/{tray,trick,brick,steam} tilt/pay hilt/{hay,proud}
do printf "%s\n" $(basename $f) | rev > $B/$f
done

B=dir-2
mkdir -p $B
for f in stroll team troll steam
do touch $B/$f
done
for d in guilt mouse house tilt hilt
do mkdir -p $B/$d
done
for f in guilt/{kilt,spray} mouse/{hay,proud} house/{heart,bay,spy} tilt/{tray,trick,brick,steam} hilt/pay
do touch $B/$f
done
for d in guilt/{heart,bay,spy} mouse/{kilt,spray} house/{tray,trick,brick,steam} tilt/pay hilt/{hay,proud}
do mkdir -p $B/$d
done

B=dir-3
mkdir -p $B
for f in guilt mouse house tilt hilt .stroll team troll .steam theft hay pull .mass rob proud node say;
do touch $B/$f
done

B=dir-4
mkdir -p $B
touch $B/spread.txt
touch $B/pig.c
touch $B/frown.toc
touch $B/ear.png
touch $B/heal.txt
touch $B/snow
touch $B/crop.png
mkdir $B/nut.c

B=dir-5
mkdir -p $B/heart/bay/spy/guilt
mkdir -p $B/heart/bay/spy/mouse
mkdir -p $B/heart/bay/spray/house
mkdir -p $B/heart/bay/spray/tilt
mkdir -p $B/heart/pay/tray/hilt
mkdir -p $B/heart/pay/trick/stroll
mkdir -p $B/heart/pay/trick/team
mkdir -p $B/heart/pay/brick/troll
mkdir -p $B/heart/pay/brick/steam
mkdir -p $B/choice/gem/check/theft
mkdir -p $B/choice/gem/check/hay
mkdir -p $B/choice/gem/check/pull
mkdir -p $B/choice/gem/check/mass
mkdir -p $B/choice/snail/deck/rob
mkdir -p $B/choice/snail/deck/proud
mkdir -p $B/choice/snail/deck/node
mkdir -p $B/choice/snail/deck/say
touch $B/heart/bay/spy/guilt/spread.txt
touch $B/heart/bay/spy/mouse/pig.c
touch $B/heart/bay/spray/house/frown.toc
touch $B/heart/bay/spray/tilt/ear.png
touch $B/choice/gem/check/hay/heal.txt
touch $B/choice/gem/check/hay/snow
touch $B/choice/snail/deck/node/crop.png
touch $B/choice/snail/deck/node/nut.c

B=numbers
mkdir -p $B
for n in 51 65 4 112 79 32 45 12 202 78
do touch $B/$n
done
