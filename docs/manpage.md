# Man Page

## Synopsis

`mixdown [OPTION...] [[input=FILE] output=FILE]`  

## Description

mixdown -- a sound generator for the command line  

## Options

`-a`, `--a4=NUM`  
A4 pitch in Hz \[default=440.0\]  

`-b`, `--bps=NUM`  
Bit per sample (8, 16, 24, 32) \[default=16\]  

`-c`, `--channels=NUM`  
Channels \[default=2\]  

`-f`, `--format=FMT`  
File format (AIFF, WAVE) \[default=WAVE\]  

`-l`, `--length=NUM`  
Length in seconds \[default=1.0\]  

`-r`, `--reverse`  
Reverse samples  

`-s`, `--samplerate=NUM`  
Sample rate in Hz \[default=44100\]  

`-v`, `--verbose`  
Produce verbose output  

`-w`, `--nowarn`  
Suppress some warnings  

`-?`, `--help`  
Give this help list  

`--usage`  
Give a short usage message  

`-V`, `--version`  
Print program version  

## Example

`echo 'sin(A4*t*tau)*fade(t,0,1,l,0);' | mixdown | aplay -q`  
