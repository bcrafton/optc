
# make;

rm -rf ../out/pd;
mkdir ../out/pd;

for v in "cpi40_to_axi32"  "cpi40_to_axi64"  "cpi40_to_axi128"  \
         "cpi40_to_ahb32"  "cpi40_to_ahb64"  "cpi40_to_ahb128"  \
         "cpi120_to_axi32" "cpi120_to_axi64" "cpi120_to_axi128" \
         "cpi120_to_ahb32" "cpi120_to_ahb64" "cpi120_to_ahb128" \
         "ahb32_to_cpi40"  "ahb64_to_cpi40"  "ahb128_to_cpi40"  \
         "ahb32_to_cpi120" "ahb64_to_cpi120" "ahb128_to_cpi120" \
         "ahb32_to_axi32"  "ahb64_to_axi64"  "ahb128_to_axi128" \
         "axi32_to_cpi40"  "axi64_to_cpi40"  "axi128_to_cpi40"  \
         "axi32_to_cpi120" "axi64_to_cpi120" "axi128_to_cpi120" \
         "axi32_to_ahb32"  "axi64_to_ahb64"  "axi128_to_ahb128"
 
do 
  mkdir ../out/pd/$v;
  unzip pd_template.zip -d ../out/pd/$v;

  cp ../out/$v.v ../out/pd/$v/pd_template/RTL/top.v
done
