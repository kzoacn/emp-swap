#!/bin/bash
time ./bin/swap 1 12345 &
sleep 0.01
time ./bin/swap 2 12345 &
