#!/bin/bash

cd original
make clean; make -j8; make clean
cd ../send_recv
make clean; make -j8; make clean
cd ../isend_recv
make clean; make -j8; make clean
cd ../spawn_all_at_once
make clean; make -j8; make clean
cd ../spawn_one_at_time
make clean; make -j8; make clean