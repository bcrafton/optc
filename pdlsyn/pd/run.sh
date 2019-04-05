
for v in "cpi_to_axi32"   "cpi_to_axi64"   "cpi_to_axi128"    \
         "cpi_to_ahb32"   "cpi_to_ahb64"   "cpi_to_ahb128"    \
         "ahb32_to_cpi"   "ahb64_to_cpi"   "ahb128_to_cpi"    \
         "ahb32_to_axi32" "ahb64_to_axi64" "ahb128_to_axi128" \
         "axi32_to_cpi"   "axi64_to_cpi"   "axi128_to_cpi"    \
         "axi32_to_ahb32" "axi64_to_ahb64" "axi128_to_ahb128"
 
do 
  cd ../out/pd/$v/pd_template/;
  source :run;
  cd ../../../../pd;
done
