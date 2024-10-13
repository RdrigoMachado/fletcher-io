#!/bin/bash

rm ./TTI*

cd original
make clean ; rm TTI*
cd ../send_recv
make clean ; rm TTI*
cd ../isend_recv
make clean ; rm TTI*
cd ../spawn_all_at_once
make clean ; rm TTI*
cd ../spawn_one_at_time
make clean ; rm TTI*

