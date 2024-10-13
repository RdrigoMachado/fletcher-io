#!/bin/bash

cd original
make clean; make -j8
mv ModelagemFletcher.exe ../original.exe
cd ../send_recv
make clean; make -j8
mv ModelagemFletcher.exe ../send_recv.exe
cd ../isend_recv
make clean; make -j8
mv ModelagemFletcher.exe ../isend_recv.exe
cd ../spawn_all_at_once
make clean; make -j8
mv ModelagemFletcher.exe ../spawn_all_at_once.exe   
mv spawn_all_at_once.x ../spawn_all_at_once.x
cd ../spawn_one_at_time
make clean; make -j8
mv ModelagemFletcher.exe ../spawn_one_at_time.exe
mv spawn_one_at_time.x ../spawn_one_at_time.x