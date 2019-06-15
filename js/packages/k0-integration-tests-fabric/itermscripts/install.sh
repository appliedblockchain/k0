#!/bin/sh

for script in 0_k0 1_devmode_setup 2_devmode_chaincode_and_test
do
  osacompile -o ~/Library/Application\ Support/iTerm2/Scripts/fabric_${script}.scpt fabric_${script}.applescript
done
