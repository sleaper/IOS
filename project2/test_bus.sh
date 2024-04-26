#!/bin/bash
#NesnupejteDrozdi
L=200
Z=10
K=50
TL=100
TB=100

#eval "./proj2 ${L} ${Z} ${K} ${TL} ${TB} > /dev/null"
./proj2 ${L} ${Z} ${K} ${TL} ${TB}


if ! [ $? -eq 0 ]; then
        echo "Error: $?"
        exit 1
fi
INPUT="proj2.out"
OK=0

glo_res="OK"
ma_tam_byt_match() {
  if [ -z "$grep_output" ]; then
      glo_res="FAILED"
  else
      ((OK++))
  fi
}
check(){
  #echo "$1"
  A=$(cat "${INPUT}" | grep -oP "$1" | awk -F: '{ print $1 }' )
  #echo "A: $A"
  if [[ "$chronologicky" -lt $A && $A -ne ""  ]]; then
    ((OK++))
    chronologicky="$A"
  else
    echo "FAILED $1 tam neni/neni ve spravnem poradi "
    glo_res="FAILED"
  fi
}
echo -n "A je popořadě? "
cat ${INPUT} | awk -F: '{print $1}' > cisla.txt
seq 1 $(($(wc -l < "cisla.txt") )) > expected_sequence.txt # OK 1
if cmp -s cisla.txt expected_sequence.txt; then
    ((OK++))
    echo "OK"
else
  echo "FAILED"
  echo -n "diff: cisla.txt expected_sequence.txt"
  diff cisla.txt expected_sequence.txt
fi
#rm cisla.txt
#rm expected_sequence.txt

glo_res="OK"
echo -n "BUS jednou zacina a jednou konci? " # OK 1
start=$(grep -Pc "\d+: BUS: started" ${INPUT})
finish=$(grep -Pc "\d+: BUS: finish" ${INPUT})

if [[ 1 -eq $start && 1 -eq $start ]]; then
  ((OK++))
else
  glo_res="FAILED"
fi

echo "${glo_res}"

echo -n "Kazdy lyzar je chronologicky? " # OK L*4
#echo -n "started-> arrived -> boarding -> going to ski"
glo_res="OK"
for ((i=1; i<=L; i++)); do
    #grep_output=$(cat "${INPUT}" | pcregrep -M "\d+: L ${i}: started\n(.*\n)*?\d+: L ${i}: arrived to \d+(.*\n)*?\d+: L ${i}: boarding(.*\n)*?\d+: L ${i}: going to ski" | grep -c "L ${i}:")
    chronologicky=0
    glo_res="OK"
    check "\d+: L ${i}: started"
    check "\d+: L ${i}: arrived to \d+"
    check "\d+: L ${i}: boarding"
    check "\d+: L ${i}: going to ski"
done
echo "${glo_res}"

glo_res="OK" # 1 OK
vystupujr_na_final=$(cat "${INPUT}" | pcregrep -M "\d+: BUS: arrived to final[ ]*\n(.*\n)*?\d+: BUS: leaving final" | grep -c  "ski")
vystupuje=$(cat "${INPUT}" | grep -c "ski")
echo -n "${vystupujr_na_final} z ${vystupuje} lyzaru, kteri vystupuji, vystupuji na final "
if [ "${vystupujr_na_final}" = "${vystupuje}" ]; then
  ((OK++))
else
  glo_res="FAILED"
fi
echo "${glo_res}"

glo_res="OK"

echo -n "Nastoupili na sprane zastavce, kam prisli? " # OK L
for ((i=1; i<=L; i++)); do
    arrived_zas=$(cat "${INPUT}" | grep -P "\d+: L ${i}: arrived to (\d+)")
    zastavka=$(echo "${arrived_zas}" | grep -oP "\d+[ ]*$" | grep -oP "\d+")
    arrived_A=$(echo "${arrived_zas}" | awk -F: '{ print $1 }')
    #echo "cat proj2.out | grep -P \"(^\d+: BUS: arrived to ${zastavka}$|^\d+: BUS: leaving ${zastavka}$.*|\d+: L ${i}: boarding$)\" | pcregrep -M \"\d+: BUS: arrived to ${zastavka}\n\d+: L ${i}: boarding\n\d+: BUS: leaving ${zastavka}\" | awk -F: '{first_field = \$1} END {print first_field}'"
    boarding=$(cat proj2.out | grep -P "(^\d+: BUS: arrived to ${zastavka}|^\d+: BUS: leaving ${zastavka}|\d+: L ${i}: boarding)" | pcregrep -M "\d+: BUS: arrived to ${zastavka}[ ]*\n\d+: L ${i}: boarding[ ]*\n\d+: BUS: leaving ${zastavka}" | awk -F: '{first_field = $1} END {print first_field}' )
    #echo "ZAS:${zastavka}  ${arrived_A} ${boarding}"
    if [[ ${arrived_A} -lt ${boarding} && -n ${arrived_zas} && -n ${boarding} ]]; then
            ((OK++))
            chronologicky="$A"
          else
            echo "L ${i} nenastoupil na zastavce, kam přišel (${zastavka})"
            glo_res="FAILED"
          fi
done
echo "${glo_res}"

echo "_______________________________________"
echo "OK ${OK}/$((L+(4*L)+2+1))"
