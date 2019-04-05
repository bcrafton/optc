
cd ..;

# make;

# move to tmp file
mv config configs/tmp;

###############
##### CPI #####
###############

###########
# cpi 2 axi
###########
cp configs/cpi40_to_axi32 config;
make run;

#cp configs/cpi40_to_axi64 config;
#make run;

#cp configs/cpi40_to_axi128 config;
#make run;

#cpi120
#cp configs/cpi120_to_axi32 config;
#make run;

cp configs/cpi120_to_axi64 config;
make run;

#cp configs/cpi120_to_axi128 config;
#make run;

###########
# cpi 2 ahb
###########
cp configs/cpi40_to_ahb32 config;
make run;

#cp configs/cpi40_to_ahb64 config;
#make run;

#cp configs/cpi40_to_ahb128 config;
#make run;

#cpi120
#cp configs/cpi120_to_ahb32 config;
#make run;

cp configs/cpi120_to_ahb64 config;
make run;

#cp configs/cpi120_to_ahb128 config;
#make run;

###############
##### AHB #####
###############

###########
# ahb 2 cpi
###########
cp configs/ahb32_to_cpi40 config;
make run;

#cp configs/ahb64_to_cpi40 config;
#make run;

#cp configs/ahb128_to_cpi40 config;
#make run;

#cpi120
#cp configs/ahb32_to_cpi120 config;
#make run;

cp configs/ahb64_to_cpi120 config;
make run;

#cp configs/ahb128_to_cpi120 config;
#make run;

###########
# ahb 2 axi
###########
cp configs/ahb32_to_axi32 config;
make run;

cp configs/ahb64_to_axi64 config;
make run;

#cp configs/ahb128_to_axi128 config;
#make run;

###############
##### AXI #####
###############

###########
# axi 2 cpi
###########
cp configs/axi32_to_cpi40 config;
make run;

#cp configs/axi64_to_cpi40 config;
#make run;

#cp configs/axi128_to_cpi40 config;
#make run;

#cpi120
#cp configs/axi32_to_cpi120 config;
#make run;

cp configs/axi64_to_cpi120 config;
make run;

#cp configs/axi128_to_cpi120 config;
#make run;

###########
# axi 2 ahb
###########
cp configs/axi32_to_ahb32 config;
make run;

cp configs/axi64_to_ahb64 config;
make run;

#cp configs/axi128_to_ahb128 config;
#make run;

# move back tmp file
mv configs/tmp config;
