#!/bin/bash
touch vide
printf "\n" > lf

cat > texte <<"EOF"
01 Harpagon       : Lui, qui me paroît si fidèle ?
02 
03 Maître Jacques : Lui−même. Je crois que c'est lui qui vous a dérobé.
04
05 Harpagon       : Et sur quoi le crois−tu ?
06
07 Maître Jacques : Sur quoi ?
08
09 Harpagon       : Oui.
10
11 Maître Jacques : Je le crois... sur ce que je le crois.
12
13 Le Commissaire : Mais il est nécessaire de dire les indices que vous
14 avez.
15
16 Harpagon       : L'as−tu vu rôder autour du lieu où j'avois mis mon argent ?
17 
18 Maître Jacques : Oui, vraiment. Où étoit−il votre argent ?
19 
20 Harpagon       : Dans le jardin.
21
22 Maître Jacques : Justement : je l'ai vu rôder dans le jardin. Et dans
23                  quoi est−ce que cet argent étoit ?
24
25 Harpagon       : Dans une cassette.
26
27 Maître Jacques : Voilà l'affaire : je lui ai vu une cassette.
28
29 Harpagon       : Et cette cassette, comment est−elle faite ? Je verrai bien
30                  si c'est la mienne.
31
32 Maître Jacques : Comment elle est faite ?
33
34 Harpagon       : Oui.
35
36 Maître Jacques : Elle est faite... elle est faite comme une cassette
EOF

cat >> split.sh <<"EOF"
#!/bin/bash
if [ "$#" -lt 1 ]; then N=10; else N="$1"; fi
I=0
while read LINE; do
   if [ "$I" -eq "$N" ]; then exec >&2; fi
   printf "%s\n" "$LINE"
   (( I++ ))
done
EOF
chmod u+x split.sh

cat >> ls.sh <<"EOF"
#!/bin/bash
if [ "$#" -lt 1 ]; then T="."; else T="$1"; fi
find "$T" -not -name ".*" -exec stat -c "%A %n" "{}" ";" | sort
EOF
chmod u+x ls.sh


mkdir -p dossier
