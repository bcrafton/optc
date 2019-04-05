./chips --viz ./in/wishbone_master ./in/axi_slave      > wishbone2axi &
./chips --viz ./in/axi_master      ./in/wishbone_slave > axi2wishbone &
./chips --viz ./in/wishbone_master ./in/ahb_slave      > wishbone2ahb &
./chips --viz ./in/ahb_master      ./in/wishbone_slave > ahb2wishbone &

