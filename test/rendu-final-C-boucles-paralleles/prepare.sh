#!/bin/bash

cat > ret <<"EOF"
#!/bin/bash
exit $(basename $1)
EOF
chmod u+x ret

cat > limit-parallel <<"EOF"
#!/bin/bash
exec 2>/dev/null
if ! ps | grep limit-para[l]lel | wc -l | xargs test "$1" -ge ; then
  printf "\r%s" "Trop d'itérations en parallèle !"
fi
EOF
chmod u+x limit-parallel

B=dir-30
mkdir -p "$B"
for i in {1..30}; do
    touch "$B/$i"
done
