#!/bin/bash

cat > raise <<"EOF"
#!/bin/bash
kill -s "$1" "$$"
EOF
chmod u+x raise

B=dir-2
mkdir -p "$B"
for i in {1..2}; do
    touch "$B/$i"
done

B=dir-30
mkdir -p "$B"
for i in {1..30}; do
    touch "$B/$i"
done
