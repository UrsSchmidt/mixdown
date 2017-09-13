# Language Description

## Comments

`/* this is a comment */`  
Text in comments will be ignored.  

## Operators

`n1 + n2`  
The sum of the two number n1 and n2. This operator is left associative.  

`n1 - n2`  
The difference of the two number n1 and n2. This operator is left associative.  

`n1 * n2`  
The product of the two number n1 and n2. This operator is left associative.  

`n1 / n2`  
The ratio of the two numbers n1 and n2. This operator is left associative. Dividing by 0 will give you a warning and return n1.  

`n1 % n2`  
The modulo of the two numbers n1 and n2. This operator is left associative. Dividing by 0 will give you a warning and return n1.  

`-n1`  
This will invert the sign of n1.  

`n1 ^ n2`  
The n1 to the power of n2. This operator is right-associative.  

Operator precedence is: `^` before `*/%` before `+-`.  

`(some_expression)`  
Expressions can be put in brackets to alter the order of precedence.  

## Constants

`e`  
The base of the natural logarithm. `= 2.71828182845904523536`  

`H`  
The twelfth root of two. It is the ratio of the two frequencies of a semitone step on the twelve-tone equal temperament scale. `= 1.0594630943592952646`  

`pi`  
The ratio of a circle's circumference to its diameter. `= 3.14159265358979323846`  

`tau`  
Two times pi. This is the period of all oscillators as well as the functions `sin` and `cos`. `= 6.28318530717958647693`  

`[A-G][#b]?[0-8]`  
Keys can be given using their scientific names. They will be converted to their frequency in Hz.  
**Some examples**:  
`A4  /* = 440 */`  
`C#4 /* = 277.183... */`  
`Eb4 /* = 311.127... */`  

## Pre-defined Identifiers

`l`  
The length of the sound in seconds. To set this value use the command line option `-l`.  

`t`  
The current time in seconds \[0..l\].  

## Unit Conversion Functions

`dB(n)`  
**n**: The amplitude in dB.  
Convert from dB to amplitude \[0..1\].  
**Example**:  
`sin(A4*t*tau)*dB(-11);`  

`deg(n)`  
**n**: The angle in degrees \[0..360\].  
Convert from degrees to radians \[0..tau\].  
**Example**:  
`sin(A4*t*tau+deg(90));`  

## Pre-Oscillator Functions

`phi(delta)`  
**delta**: The phase delta, usually a frequency.  
Increments phi by (delta times tau divided by the sample rate), mods it by tau and returns the new value for phi. This function has side-effects.  
**Example**:  
`sin(phi(A4));`  

`symm(t, n)`  
**t**: t  
**n**: The symmetry \[-1..1\], 0: neutral.  
Applies symmetry to an arbitrary signal like `sin` or `tri`.  
**Example**:  
`sin(symm(A4*t*tau,0.75));`  

`sync(x1, x2)`  
**x1**: The x value for signal 1.  
**x2**: The x value for signal 2.  
Hard-syncs two signals.  
**Example**:  
`sin(sync(A4*t*tau,A4*H^14*t*tau));`  

## Oscillator Functions

`circ(x)`  
**x**: f\*t\*tau  
Circle wave signal generator.  
**y**: \[-1..1\]  
**Example**:  
`circ(A4*t*tau);`  

`exptl(x)`  
**x**: f\*t\*tau  
Exponential wave signal generator.  
**y**: \[-1..1\]  
**Example**:  
`exptl(A4*t*tau);`  

`moog(x)`  
**x**: f\*t\*tau  
Moog-like wave signal generator.  
**y**: 0..1/0..-1..0  
**Example**:  
`moog(A4*t*tau);`  

`noise(x)`  
**x**: f\*t\*tau  
White noise generator (periodic).  
**y**: \[-1..1\]  
**Example**:  
`noise(A4*t*tau);`  

`parab(x, n)`  
**x**: f\*t\*tau  
**n**: The exponent \[1..\], 1: triangle wave, 2: not-pure sin wave, 10+: square wave.  
Parabola wave signal generator with exponent.  
**y**: \[-1..1\]  
**Example**:  
`parab(A4*t*tau,2);`  

`pulse(x, n)`  
**x**: f\*t\*tau  
**n**: The pulse width \[-1..1\], 0: neutral.  
Square wave signal generator with pulse width.  
**y**: n+1,n-1  
**Example**:  
`pulse(A4*t*tau,0.5)*0.5;`  

`sawf(x)`  
**x**: f\*t\*tau  
Falling sawtooth wave signal generator.  
**y**: 0..-1/1..0  
**Example**:  
`sawf(A4*t*tau);`  

`sawr(x)`  
**x**: f\*t\*tau  
Rising sawtooth wave signal generator.  
**y**: 0..1/-1..0  
**Example**:  
`sawr(A4*t*tau);`  

`squ(x)`  
**x**: f\*t\*tau  
Square wave signal generator.  
**y**: 1,-1  
**Example**:  
`squ(A4*t*tau);`  

`tri(x)`  
**x**: f\*t\*tau  
Triangle wave signal generator.  
**y**: 0..1..0..-1..0  
**Example**:  
`tri(A4*t*tau);`  

## Envelopes

`adsr(t, t_on, t_off, a, d, s, r)`  
**t**: t  
**t_on**: The note on time-stamp in seconds.  
**t_off**: The note off time-stamp in seconds.  
**a**: The attack in seconds.  
**d**: The decay in seconds.  
**s**: The sustain amplitude \[0..1\].  
**r**: The release in seconds.  
Generates an ADSR signal.  
**y**: 0, 0..1, 1..s, s, s..0, 0  
**Example**:  
`sin(A4*t*tau)*adsr(t,0,0.5*l,0.25,0.25,0.5,0.25);`  

`sfs(t, t_on, t_off, a, d, s, r)`  
**t**: t  
**t_on**: The note on time-stamp in seconds.  
**t_off**: The note off time-stamp in seconds.  
**a**: The attack.  
**d**: The decay.  
**s**: The sustain amplitude \[0..1\].  
**r**: The release.  
Generates an ADSR signal using exponential decay and release like in Soundforum Synth.  
**y**: 0, 0..1, 1..s, s..0  
**Example**:  
`sin(A4*t*tau)*sfs(t,0,0.5*l,0,50,0.25,35);`  

`dahdsr(t, t_on, t_off, dl, a, h, dc, s, r)`  
**t**: t  
**t_on**: The note on time-stamp in seconds.  
**t_off**: The note off time-stamp in seconds.  
**dl**: The delay in seconds.  
**a**: The attack in seconds.  
**h**: The hold in seconds.  
**dc**: The decay in seconds.  
**s**: The sustain amplitude \[0..1\].  
**r**: The release in seconds.  
Generates a DAHDSR signal.  
**y**: 0, 0, 0..1, 1, 1..s, s, s..0, 0  
**Example**:  
`sin(A4*t*tau)*dahdsr(t,0,0.5*l,0.15,0.15,0.15,0.15,0.5,0.15);`  

`fade(t, (t_arr, y_arr)...)`  
**t**: t  
**t_arr**: The time of point n in seconds.  
**y_arr**: The amplitude of point n.  
Generates a fade signal with two or more points.  
**Fade in example**:  
`sin(A4*t*tau)*fade(t,0,0,l,1);`  
**Fade out example**:  
`sin(A4*t*tau)*fade(t,0,1,l,0);`  
**Fade in and out example**:  
`sin(A4*t*tau)*fade(t,0,0,0.5*l,1,l,0);`  

## Filter

`filter2(x_n, b0, b1, b2, a1, a2)`  
**x_n**: The original signal x\[n\].  
**b0**: Coefficient for x\[n\].  
**b1**: Coefficient for x\[n-1\].  
**b2**: Coefficient for x\[n-2\].  
**a1**: Coefficient for y\[n-1\].  
**a2**: Coefficient for y\[n-2\].  
Filters a signal using a biquad (aka 2-pole) IIR filter with the given parameters. This function has side-effects.  

`filterlp2(x_n, fc, bw)`  
**x_n**: The original signal x\[n\].  
**fc**: The cutoff frequency in Hz.  
**bw**: The bandwidth.  
Filters a signal using a biquad (aka 2-pole) IIR lowpass filter with the given parameters. This function has side-effects.  

## Arpeggiators

`arp(t, t2, n_arr...)`  
**t**: t  
**t2**: The rate in seconds.  
**n_arr...**: Some values.  
Arpeggiates between all values given in n_arr.  
**Example**:  
`sin(arp(t,1,C4,C#4,D4,D#4,E4)*t*tau);`  

## Math

`sin(n)`  
**n**: Some number.  
The sine of n. Can also be used as an oscillator.  
**Example**:  
`sin(A4*t*tau);`  

`cos(n)`  
**n**: Some number.  
The cosine of n. Can also be used as an oscillator.  
**Example**:  
`cos(A4*t*tau);`  

`tan(n)`  
**n**: Some number.  
The tangent of n.  

`asin(n)`  
**n**: Some number.  
The arcsin of n.  

`acos(n)`  
**n**: Some number.  
The arccos of n.  

`atan(n)`  
**n**: Some number.  
The arctan of n.  

`exp(n)`  
**n**: Some number.  
e to the power of n.  

`ln(n)`  
**n**: some number  
The natural logarithm (base e) of n.  

`log10(n)`  
**n**: Some number.  
Logarithm (base 10) of n.  

`sqrt(n)`  
**n**: Some number.  
Square root of n.  

`ceil(n)`  
**n**: Some number.  
n rounded up to the next integer.  

`floor(n)`  
**n**: Some number.  
n rounded down to the next integer.  

`max(n_arr...)`  
**n_arr**: Some numbers.  
The maximum of all given numbers.  

`min(n_arr...)`  
**n_arr**: Some numbers.  
The minimum of all given numbers.  

`abs(n)`  
**n**: Some number.  
The absolute value of n.  

`sgn(n)`  
**n**: Some number.  
The signum of n.  
